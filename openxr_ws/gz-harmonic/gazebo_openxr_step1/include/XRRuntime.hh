#pragma once

#include <cstdint>
#include <string>

#include <openxr/openxr.h>

class XRRuntime
{
public:
    XRRuntime() = default;
    ~XRRuntime();

    XRRuntime(const XRRuntime &) = delete;
    XRRuntime &operator=(const XRRuntime &) = delete;

    bool Initialize();
    void Shutdown();

    bool IsInitialized() const;
    bool IsSessionRunning() const;
    bool PollEvents();
    bool RequestStop();
    bool CreateSessionFromCurrentContext();
    bool RenderFrame();

    XrInstance GetInstance() const;
    XrSystemId GetSystemId() const;
    XrSession GetSession() const;
    XrSpace GetLocalSpace() const;

    std::string GetStatus() const;

private:
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;
    XrSession session_ = XR_NULL_HANDLE;
    XrSpace localSpace_ = XR_NULL_HANDLE;
    XrSessionState sessionState_ = XR_SESSION_STATE_UNKNOWN;

    bool initialized_ = false;
    bool sessionRunning_ = false;
    bool exitRequested_ = false;
    uint64_t frameCount_ = 0;

    std::string status_ = "Not initialized";

    bool Fail(const std::string &message, XrResult result);
    bool ProbeGraphicsContext() const;
    void DestroySession();
};
