#pragma once

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#include <openxr/openxr.h>

#include <cstdint>
#include <string>
#include <vector>

#ifndef XR_USE_PLATFORM_XLIB
#define XR_USE_PLATFORM_XLIB
#endif

#ifndef XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_GRAPHICS_API_OPENGL
#endif

#include <openxr/openxr_platform.h>

#ifdef Status
#undef Status
#endif

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

    bool CreateColorSwapchains();

    void DestroyColorSwapchains();

    bool AreColorSwapchainsReady() const;

    bool TestSwapchainImageCycle();

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

    struct EyeSwapchain
    {
        XrSwapchain handle = XR_NULL_HANDLE;

        uint32_t width = 0;
        uint32_t height = 0;

        std::vector<XrSwapchainImageOpenGLKHR> images;
    };

    std::vector<EyeSwapchain> eyeSwapchains_;

    bool swapchainsReady_ = false;
};

