#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
    bool InspectViewConfiguration();
    bool CreateColorSwapchains();
    bool AreColorSwapchainsReady() const;

    XrInstance GetInstance() const;
    XrSystemId GetSystemId() const;
    XrSession GetSession() const;
    XrSpace GetLocalSpace() const;

    std::string GetStatus() const;

private:
    struct EyeSwapchain
    {
        XrSwapchain handle = XR_NULL_HANDLE;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint32_t> textureIds;
    };

    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;
    XrSession session_ = XR_NULL_HANDLE;
    XrSpace localSpace_ = XR_NULL_HANDLE;
    XrSessionState sessionState_ = XR_SESSION_STATE_UNKNOWN;

    bool initialized_ = false;
    bool sessionRunning_ = false;
    bool exitRequested_ = false;
    bool viewConfigurationInspected_ = false;
    bool swapchainsReady_ = false;
    uint64_t frameCount_ = 0;
    std::vector<EyeSwapchain> eyeSwapchains_;

    std::string status_ = "Not initialized";

    bool Fail(const std::string &message, XrResult result);
    bool ProbeGraphicsContext() const;
    bool QueryStereoViews(std::vector<XrViewConfigurationView> &views);
    bool QuerySwapchainFormats(std::vector<int64_t> &formats);
    void DestroyColorSwapchains();
    void DestroySession();
};
