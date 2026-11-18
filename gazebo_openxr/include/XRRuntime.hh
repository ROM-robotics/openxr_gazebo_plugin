#pragma once

#include <openxr/openxr.h>
#include <string>

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

private:
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;

    XrSession session_ = XR_NULL_HANDLE;
    XrSpace localSpace_ = XR_NULL_HANDLE;

    bool initialized_ = false;

    std::string status_ = "Not initialized";

    bool Fail(const std::string& message,
              XrResult result);
};

