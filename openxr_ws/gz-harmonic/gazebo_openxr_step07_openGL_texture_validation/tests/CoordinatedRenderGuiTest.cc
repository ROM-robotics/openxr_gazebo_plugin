#include <atomic>
#include <iostream>

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
#include <tinyxml2.h>
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
    REQUIRE(button);
    auto *mainWindow = app.findChild<gz::gui::MainWindow *>();
    REQUIRE(mainWindow && mainWindow->QuickWindow());
    RenderCounter counter;
    mainWindow->installEventFilter(&counter);
    mainWindow->QuickWindow()->resize(1024, 720);
    mainWindow->QuickWindow()->show();
    REQUIRE(QMetaObject::invokeMethod(button, "clicked"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString().contains(
            "Stereo textures validated");
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
            "Stereo textures validated");
    }, 10000));
    const int secondFrame = counter.frames.load();
    REQUIRE(WaitFor([&] { return counter.frames.load() >= secondFrame + 30; },
                    10000));
    REQUIRE(QMetaObject::invokeMethod(plugin.get(), "StopVR"));
    REQUIRE(WaitFor([&] {
        return plugin->property("status").toString() == "OpenXR Session Stopped";
    }, 10000));
    mainWindow->removeEventFilter(&counter);
    std::cout << "Coordinated render and subsequent GUI frames passed\n";
    return 0;
}
