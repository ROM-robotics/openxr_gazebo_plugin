#include "GazeboOpenXR.hh"

#include <iostream>

#include <QMetaObject>

#include <gz/gui/Application.hh>
#include <gz/gui/GuiEvents.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";

    auto *mainWindow =
        gz::gui::App()->findChild<gz::gui::MainWindow *>();

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
    if (_event->type() == gz::gui::events::Render::kType)
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

        if (runtime_.RequestStop())
        {
            PostStatus("Stopping OpenXR Session...");
        }
        else
        {
            PostStatus(
                QString::fromStdString(runtime_.GetStatus()));
        }
    }

    if (checkRequested_.exchange(false))
    {
        runtime_.Initialize();
        PostStatus(QString::fromStdString(runtime_.GetStatus()));
    }

    if (startRequested_.exchange(false) &&
        !sessionCreated_.load())
    {
        if (!runtime_.Initialize())
        {
            PostStatus(
                QString::fromStdString(runtime_.GetStatus()));
            return;
        }

        if (!runtime_.CreateSessionFromCurrentContext())
        {
            PostStatus(
                QString::fromStdString(runtime_.GetStatus()));
            return;
        }

        sessionCreated_.store(true);
        PostStatus("OpenXR Session Created");
    }

    if (!sessionCreated_.load())
    {
        return;
    }

    if (runtime_.GetSession() == XR_NULL_HANDLE)
    {
        sessionCreated_.store(false);
        xrCamera_.Reset();
        viewInspectionAttempted_ = false;
        cameraCreationFailed_ = false;
        swapchainCreationFailed_ = false;
        PostStatus("OpenXR Session Stopped");
        return;
    }

    const bool wasRunning = runtime_.IsSessionRunning();

    if (!runtime_.PollEvents())
    {
        PostStatus(QString::fromStdString(runtime_.GetStatus()));
        sessionCreated_.store(runtime_.GetSession() != XR_NULL_HANDLE);
        if (runtime_.GetSession() == XR_NULL_HANDLE)
        {
            xrCamera_.Reset();
            viewInspectionAttempted_ = false;
            cameraCreationFailed_ = false;
            swapchainCreationFailed_ = false;
        }
        return;
    }

    if (runtime_.GetSession() == XR_NULL_HANDLE)
    {
        sessionCreated_.store(false);
        xrCamera_.Reset();
        viewInspectionAttempted_ = false;
        cameraCreationFailed_ = false;
        swapchainCreationFailed_ = false;
        PostStatus("OpenXR Session Stopped");
        return;
    }

    if (runtime_.IsSessionRunning())
    {
        if (!wasRunning)
        {
            PostStatus("VR Frame Loop Running");
        }

        if (!viewInspectionAttempted_)
        {
            viewInspectionAttempted_ = true;
            if (!runtime_.InspectViewConfiguration())
            {
                PostStatus(QString::fromStdString(runtime_.GetStatus()));
            }
        }

        if (!cameraCreationFailed_ && xrCamera_.FindScene() &&
            !xrCamera_.CreateStereoCameras())
        {
            cameraCreationFailed_ = true;
            PostStatus("Stereo camera creation failed; see terminal");
        }

        if (!swapchainCreationFailed_ &&
            !runtime_.AreColorSwapchainsReady())
        {
            if (!runtime_.CreateColorSwapchains())
            {
                swapchainCreationFailed_ = true;
                const QString error =
                    QString::fromStdString(runtime_.GetStatus());
                if (!runtime_.RequestStop())
                {
                    PostStatus(error + "\n" +
                        QString::fromStdString(runtime_.GetStatus()));
                }
                else
                {
                    PostStatus(error);
                }
            }
            else
            {
                PostStatus("Stereo swapchains ready (diagnostic)");
            }
        }

        if (!runtime_.RenderFrame())
        {
            const QString error = QString::fromStdString(runtime_.GetStatus());
            if (!runtime_.RequestStop())
            {
                PostStatus(error + "\n" +
                    QString::fromStdString(runtime_.GetStatus()));
            }
            else
            {
                PostStatus(error);
            }
        }
    }
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
