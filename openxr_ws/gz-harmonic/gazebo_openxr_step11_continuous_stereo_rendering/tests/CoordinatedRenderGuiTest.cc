#define GL_GLEXT_PROTOTYPES
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <QElapsedTimer>
#include <QEventLoop>
#include <QMetaObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QThread>

#include <gz/gui/Application.hh>
#include <gz/gui/GuiEvents.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/Plugin.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <tinyxml2.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

namespace
{
class RenderCounter : public QObject
{
public:
    std::atomic<int> frames{0};

protected:
    bool eventFilter(QObject *, QEvent *event) override
    {
        if (event->type() == gz::gui::events::Render::kType)
        {
            ++frames;
        }
        return false;
    }
};

class SceneProbe : public QObject
{
public:
    std::atomic<int> desired{0};
    std::atomic<int> applied{0};
    std::atomic<int> observed{0};
    std::atomic<int> samples{0};
    std::atomic<bool> failed{false};

protected:
    bool eventFilter(QObject *, QEvent *event) override
    {
        if (event->type() != gz::gui::events::Render::kType || !desired.load() ||
            gz::rendering::loadedEngines().empty())
            return false;
        auto *engine = gz::rendering::engine("ogre2");
        auto scene = engine ? engine->SceneByName("coordinated_test") : nullptr;
        if (!scene || !scene->IsInitialized())
            return false;
        const int color = desired.load();
        if (applied.load() != color)
        {
            scene->SetBackgroundColor(color == 1 ? 1.0 : 0.0,
                                      color == 2 ? 1.0 : 0.0,
                                      color == 3 ? 1.0 : 0.0);
            applied.store(color);
        }
        const auto now = std::chrono::steady_clock::now();
        if (now - lastSample_ < std::chrono::milliseconds(50))
            return false;
        lastSample_ = now;
        auto left = std::dynamic_pointer_cast<gz::rendering::Camera>(
            scene->SensorByName("xr_left_eye"));
        auto right = std::dynamic_pointer_cast<gz::rendering::Camera>(
            scene->SensorByName("xr_right_eye"));
        if (!left || !right || !left->RenderTextureGLId() || !right->RenderTextureGLId())
            return false;
        const int leftColor = ReadColor(left->RenderTextureGLId());
        const int rightColor = ReadColor(right->RenderTextureGLId());
        if (leftColor < 0 || rightColor < 0)
            failed.store(true);
        observed.store(leftColor == rightColor ? leftColor : 0);
        ++samples;
        return false;
    }

private:
    int ReadColor(unsigned int texture)
    {
        if (!glXGetCurrentContext())
            return -1;
        GLint binding = 0, packBuffer = 0, alignment = 0, rowLength = 0;
        GLint skipRows = 0, skipPixels = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &binding);
        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &packBuffer);
        glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
        glGetIntegerv(GL_PACK_ROW_LENGTH, &rowLength);
        glGetIntegerv(GL_PACK_SKIP_ROWS, &skipRows);
        glGetIntegerv(GL_PACK_SKIP_PIXELS, &skipPixels);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glBindTexture(GL_TEXTURE_2D, texture);
        GLint width = 0, height = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        int color = -1;
        if (width > 0 && height > 0 && width <= 4096 && height <= 4096)
        {
            std::vector<unsigned char> pixels(
                static_cast<std::size_t>(width) * height * 4);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            const auto offset = ((height / 10) * width + width / 10) * 4;
            color = 0;
            for (int channel = 0; channel < 3; ++channel)
            {
                if (pixels[offset + channel] > 128 &&
                    pixels[offset + (channel + 1) % 3] < 64 &&
                    pixels[offset + (channel + 2) % 3] < 64)
                    color = channel + 1;
            }
        }
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(binding));
        glBindBuffer(GL_PIXEL_PACK_BUFFER, static_cast<GLuint>(packBuffer));
        glPixelStorei(GL_PACK_ALIGNMENT, alignment);
        glPixelStorei(GL_PACK_ROW_LENGTH, rowLength);
        glPixelStorei(GL_PACK_SKIP_ROWS, skipRows);
        glPixelStorei(GL_PACK_SKIP_PIXELS, skipPixels);
        return glGetError() == GL_NO_ERROR ? color : -1;
    }

    std::chrono::steady_clock::time_point lastSample_{};
};

template <typename Predicate>
bool WaitFor(Predicate predicate, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        if (predicate())
        {
            return true;
        }
        QThread::msleep(10);
    }
    return false;
}
}

#define REQUIRE(condition) \
    do { if (!(condition)) { \
        std::cerr << "Failed at line " << __LINE__ << ": " #condition << '\n'; \
        return 1; \
    } } while (false)

int main(int argc, char **argv)
{
    Display *display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "An accessible X11 display is required" << std::endl;
        return 77;
    }
    XCloseDisplay(display);

    gz::gui::Application app(argc, argv);
    REQUIRE(app.CreateMainWindow());
    QString pluginName;
    QObject::connect(&app, &gz::gui::Application::PluginAdded,
                     [&](const QString &name) { pluginName = name; });
    tinyxml2::XMLDocument config;
    REQUIRE(config.Parse("<plugin filename=\"MinimalScene\" name=\"MinimalScene\">"
                         "<engine>ogre2</engine>"
                         "<scene>coordinated_test</scene></plugin>") ==
            tinyxml2::XML_SUCCESS);
    REQUIRE(app.LoadPlugin("MinimalScene", config.RootElement()));
    REQUIRE(app.LoadPlugin("GazeboOpenXR"));
    auto plugin = app.PluginByName(pluginName.toStdString());
    REQUIRE(plugin && plugin->PluginItem());
    auto *button = plugin->PluginItem()->findChild<QQuickItem *>(
        "renderDiagnosticButton");
    auto *startContinuous = plugin->PluginItem()->findChild<QQuickItem *>(
        "startContinuousButton");
    auto *stopContinuous = plugin->PluginItem()->findChild<QQuickItem *>(
        "stopContinuousButton");
    REQUIRE(button && startContinuous && stopContinuous);
    auto *mainWindow = app.findChild<gz::gui::MainWindow *>();
    REQUIRE(mainWindow && mainWindow->QuickWindow());
    RenderCounter counter;
    SceneProbe probe;
    mainWindow->installEventFilter(&counter);
    mainWindow->installEventFilter(&probe);
    mainWindow->QuickWindow()->resize(1024, 720);
    mainWindow->QuickWindow()->show();
    REQUIRE(QMetaObject::invokeMethod(button, "clicked"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString().contains(
            "Framebuffer blit succeeded");
    }, 20000));
    const int firstFrame = counter.frames.load();
    REQUIRE(WaitFor([&] { return counter.frames.load() >= firstFrame + 30; },
                    10000));

    REQUIRE(QMetaObject::invokeMethod(plugin.get(), "StopVR"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString() == "OpenXR Session Stopped";
    }, 10000));
    REQUIRE(QMetaObject::invokeMethod(button, "clicked"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString().contains(
            "Framebuffer blit succeeded");
    }, 10000));
    const int secondFrame = counter.frames.load();
    REQUIRE(WaitFor([&] { return counter.frames.load() >= secondFrame + 30; },
                    10000));

    REQUIRE(QMetaObject::invokeMethod(startContinuous, "clicked"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString() == "Continuous stereo test running";
    }, 10000));
    probe.desired.store(1);
    REQUIRE(WaitFor([&] { return probe.observed.load() == 1 || probe.failed.load(); }, 10000));
    REQUIRE(!probe.failed.load());
    probe.desired.store(2);
    REQUIRE(WaitFor([&] { return probe.observed.load() == 2 || probe.failed.load(); }, 10000));
    REQUIRE(!probe.failed.load());
    QElapsedTimer runningTimer;
    runningTimer.start();
    REQUIRE(WaitFor([&] { return runningTimer.elapsed() >= 3200; }, 5000));
    REQUIRE(!probe.failed.load());

    REQUIRE(QMetaObject::invokeMethod(stopContinuous, "clicked"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString() == "Continuous stereo test stopped";
    }, 10000));
    const int priorSamples = probe.samples.load();
    probe.desired.store(3);
    REQUIRE(WaitFor([&] {
        return probe.applied.load() == 3 && probe.samples.load() >= priorSamples + 5;
    }, 10000));
    REQUIRE(!probe.failed.load() && probe.observed.load() == 2);
    REQUIRE(QMetaObject::invokeMethod(startContinuous, "clicked"));
    REQUIRE(WaitFor([&] { return probe.observed.load() == 3 || probe.failed.load(); }, 10000));
    REQUIRE(!probe.failed.load());
    REQUIRE(QMetaObject::invokeMethod(stopContinuous, "clicked"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString() == "Continuous stereo test stopped";
    }, 10000));
    probe.desired.store(0);
    REQUIRE(QMetaObject::invokeMethod(plugin.get(), "StopVR"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString() == "OpenXR Session Stopped";
    }, 10000));
    mainWindow->removeEventFilter(&counter);
    mainWindow->removeEventFilter(&probe);
    std::cout << "Coordinated render and subsequent GUI frames passed\n";
    return 0;
}
