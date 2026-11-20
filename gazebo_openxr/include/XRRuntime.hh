#pragma once

#include <openxr/openxr.h>
#include <string>
#include <cstdint>

class XRRuntime
{
public:
    XRRuntime() = default;
    ~XRRuntime();

    XRRuntime(const XRRuntime&) = delete;
    XRRuntime& operator=(const XRRuntime&) = delete;

    bool Initialize();
    void Shutdown();

    bool IsInitialized() const;

    XrInstance GetInstance() const;
    XrSystemId GetSystemId() const;

    std::string GetStatus() const;

    bool ProbeGraphicsContext();

    bool CreateSessionFromCurrentContext();

    void DestroySession();

    XrSession GetSession() const;

    bool PollEvents();

    bool RequestStop();

    bool IsSessionRunning() const;

    bool RenderFrame();

    XrSpace GetLocalSpace() const;

    bool InspectViewConfiguration();

private:
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;

    XrSession session_ = XR_NULL_HANDLE;
    XrSpace localSpace_ = XR_NULL_HANDLE;

    bool initialized_ = false;

    std::string status_ = "Not initialized";

    bool Fail(const std::string& message,
              XrResult result);

    XrSessionState sessionState_ =
    XR_SESSION_STATE_UNKNOWN;

    bool sessionRunning_ = false;

    bool exitRequested_ = false;

    uint64_t frameCount_ = 0;

    bool viewConfigurationInspected_ = false;
};

