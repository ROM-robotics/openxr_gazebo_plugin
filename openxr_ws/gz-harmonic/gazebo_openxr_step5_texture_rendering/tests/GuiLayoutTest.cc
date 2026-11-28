#include <iostream>

#include <QEventLoop>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTimer>
#include <QWheelEvent>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/Plugin.hh>

namespace
{
void WaitForLayout()
{
    QEventLoop loop;
    QTimer::singleShot(400, &loop, &QEventLoop::quit);
    loop.exec();
}

bool InsideViewport(QQuickItem *item, QQuickItem *viewport)
{
    const QPointF topLeft = item->mapToItem(viewport, QPointF(0, 0));
    return topLeft.y() >= -0.5 &&
        topLeft.y() + item->height() <= viewport->height() + 0.5;
}
}

#define REQUIRE(condition) \
    do { if (!(condition)) { \
        std::cerr << "Failed at line " << __LINE__ << ": " #condition << '\n'; \
        return 1; \
    } } while (false)

int main(int argc, char **argv)
{
    gz::gui::Application app(argc, argv);
    REQUIRE(app.CreateMainWindow());

    QString pluginName;
    QObject::connect(&app, &gz::gui::Application::PluginAdded,
                     [&](const QString &name) { pluginName = name; });
    REQUIRE(app.LoadPlugin("GazeboOpenXR"));
    REQUIRE(app.LoadPlugin("GazeboOpenXR"));
    REQUIRE(app.LoadPlugin("GazeboOpenXR"));

    auto plugin = app.PluginByName(pluginName.toStdString());
    REQUIRE(plugin && plugin->CardItem() && plugin->PluginItem());
    auto *card = plugin->CardItem();
    auto *item = plugin->PluginItem();
    auto *mainWindow = app.findChild<gz::gui::MainWindow *>();
    REQUIRE(mainWindow && mainWindow->QuickWindow());
    auto *window = mainWindow->QuickWindow();
    window->resize(1024, 720);
    WaitForLayout();

    auto *scroll = item->findChild<QQuickItem *>("statusScroll");
    auto *status = item->findChild<QQuickItem *>("runtimeStatus");
    auto *check = item->findChild<QQuickItem *>("runtimeButton");
    auto *start = item->findChild<QQuickItem *>("startButton");
    auto *stop = item->findChild<QQuickItem *>("stopButton");
    REQUIRE(scroll && status && check && start && stop);
    REQUIRE(card->width() >= 280 && card->height() >= 360);
    REQUIRE(InsideViewport(check, scroll));
    REQUIRE(InsideViewport(start, scroll));
    REQUIRE(InsideViewport(stop, scroll));

    REQUIRE(card->setProperty("state", "floating"));
    WaitForLayout();
    REQUIRE(card->width() >= 280 && card->height() >= 360);
    REQUIRE(InsideViewport(stop, scroll));

    card->setHeight(200);
    status->setProperty("text", QString(600, QLatin1Char('x')));
    WaitForLayout();
    auto *flick = scroll->property("contentItem").value<QQuickItem *>();
    REQUIRE(flick);
    REQUIRE(scroll->property("contentHeight").toDouble() > scroll->height());

    const QPointF pos = scroll->mapToScene(
        QPointF(scroll->width() / 2, scroll->height() / 2));
    QWheelEvent wheel(pos, pos, QPoint(), QPoint(0, -120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QCoreApplication::sendEvent(window, &wheel);
    WaitForLayout();
    REQUIRE(flick->property("contentY").toDouble() > 0);

    flick->setProperty("contentY",
        scroll->property("contentHeight").toDouble() - scroll->height());
    WaitForLayout();
    REQUIRE(InsideViewport(stop, scroll));
    std::cout << "Docked, floating, and scrolling GUI layout passed\n";
    return 0;
}
