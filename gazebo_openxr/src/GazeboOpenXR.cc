#include "GazeboOpenXR.hh"

#include <iostream>
#include <QMetaObject>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/GuiEvents.hh>

#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";

    auto *mainWindow =
        gz::gui::App()->findChild<
            gz::gui::MainWindow *>();

    if (mainWindow)
    {
        mainWindow->installEventFilter(this);

        std::cout
            << "[GazeboOpenXR] Render event filter installed"
            << std::endl;
    }
}

QString GazeboOpenXR::Status() const
{
    return status_;
}

void GazeboOpenXR::SetStatus(
    const QString &status)
{
    status_ = status;

    emit StatusChanged();

    std::cout << "[GazeboOpenXR] "
              << status.toStdString()
              << std::endl;
}

void GazeboOpenXR::PostStatus(
    const QString &status)
{
    QMetaObject::invokeMethod(
        this,
        [this, status]()
        {
            SetStatus(status);
        },
        Qt::QueuedConnection
    );
}

void GazeboOpenXR::CheckRuntime()
{
    checkRequested_.store(true);
}

void GazeboOpenXR::StartVR()
{
    SetStatus("Requesting OpenXR session...");
    startRequested_.store(true);
}

void GazeboOpenXR::StopVR()
{
    stopRequested_.store(true);
}

bool GazeboOpenXR::eventFilter(
    QObject *_obj,
    QEvent *_event)
{
    if (_event->type() ==
        gz::gui::events::Render::kType)
    {
        OnRender();
    }

    return QObject::eventFilter(_obj, _event);
}

void GazeboOpenXR::OnRender()
{
    if (stopRequested_.exchange(false))
    {
        startRequested_.store(false);

        runtime_.DestroySession();

        sessionCreated_.store(false);

        PostStatus("OpenXR Session Stopped");

        return;
    }

    if (checkRequested_.exchange(false))
    {
        runtime_.Initialize();

        PostStatus(
            QString::fromStdString(
                runtime_.GetStatus()));
    }

    if (startRequested_.exchange(false))
    {
        if (sessionCreated_.load())
            return;

        std::cout
            << "[GazeboOpenXR] Start VR request received"
            << std::endl;

        if (!runtime_.Initialize())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        if (!runtime_.ProbeGraphicsContext())
        {
            PostStatus(
                "No current GLX context in render event");

            return;
        }

        if (!runtime_.CreateSessionFromCurrentContext())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        sessionCreated_.store(true);

        PostStatus(
            "OpenXR Session Created Successfully");
    }
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
