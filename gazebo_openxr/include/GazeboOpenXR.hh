#pragma once

#include <QString>
#include <QEvent>
#include <atomic>
#include "XRCamera.hh"

#include <gz/gui/Plugin.hh>

#include "XRRuntime.hh"

class GazeboOpenXR : public gz::gui::Plugin
{
    Q_OBJECT

    Q_PROPERTY(QString status
               READ Status
               NOTIFY StatusChanged)

public:
    GazeboOpenXR();
    ~GazeboOpenXR() override;

    void LoadConfig(
        const tinyxml2::XMLElement *_pluginElem
    ) override;

    QString Status() const;

    Q_INVOKABLE void CheckRuntime();
    Q_INVOKABLE void StartVR();
    Q_INVOKABLE void StopVR();

signals:
    void StatusChanged();

protected:
    bool eventFilter(
        QObject *_obj,
        QEvent *_event
    ) override;

private:
    XRRuntime runtime_;
    XRCamera xrCamera_;

    QString status_ = "OpenXR not initialized";

    std::atomic<bool> checkRequested_{false};
    std::atomic<bool> startRequested_{false};
    std::atomic<bool> stopRequested_{false};
    std::atomic<bool> sessionCreated_{false};

    void SetStatus(const QString &status);
    void PostStatus(const QString &status);
    void OnRender();
};
