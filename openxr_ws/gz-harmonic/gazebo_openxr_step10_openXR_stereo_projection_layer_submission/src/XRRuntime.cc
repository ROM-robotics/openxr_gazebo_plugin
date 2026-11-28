#include "XRRuntime.hh"
#include "TextureBlit.hh"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#define XR_USE_PLATFORM_XLIB
#define XR_USE_GRAPHICS_API_OPENGL
#include <openxr/openxr_platform.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

XRRuntime::~XRRuntime()
{
    Shutdown();
}

bool XRRuntime::Initialize()
{
    if (initialized_)
    {
        return true;
    }

    uint32_t extensionCount = 0;
    XrResult result = xrEnumerateInstanceExtensionProperties(
        nullptr, 0, &extensionCount, nullptr);

    if (XR_FAILED(result))
    {
        return Fail("Extension enumeration", result);
    }

    std::vector<XrExtensionProperties> extensions(
        extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});

    result = xrEnumerateInstanceExtensionProperties(
        nullptr, extensionCount, &extensionCount, extensions.data());

    if (XR_FAILED(result))
    {
        return Fail("Extension properties", result);
    }

    const bool openGlSupported = std::any_of(
        extensions.begin(),
        extensions.end(),
        [](const XrExtensionProperties &extension)
        {
            return std::strcmp(
                extension.extensionName,
                XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) == 0;
        });

    if (!openGlSupported)
    {
        status_ = "OpenGL extension not supported";
        return false;
    }

    const char *enabledExtensions[] = {
        XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
    };

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    std::strncpy(
        createInfo.applicationInfo.applicationName,
        "GazeboOpenXR",
        XR_MAX_APPLICATION_NAME_SIZE - 1);
    createInfo.applicationInfo.applicationVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);
    createInfo.enabledExtensionCount = 1;
    createInfo.enabledExtensionNames = enabledExtensions;

    result = xrCreateInstance(&createInfo, &instance_);
    if (XR_FAILED(result))
    {
        return Fail("xrCreateInstance", result);
    }

    XrInstanceProperties instanceProperties{
        XR_TYPE_INSTANCE_PROPERTIES
    };
    result = xrGetInstanceProperties(instance_, &instanceProperties);
    if (XR_FAILED(result))
    {
        return Fail("xrGetInstanceProperties", result);
    }

    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(instance_, &systemInfo, &systemId_);
    if (XR_FAILED(result))
    {
        return Fail("xrGetSystem", result);
    }

    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    result = xrGetSystemProperties(
        instance_, systemId_, &systemProperties);
    if (XR_FAILED(result))
    {
        return Fail("xrGetSystemProperties", result);
    }

    uint32_t modeCount = 0;
    result = xrEnumerateEnvironmentBlendModes(
        instance_, systemId_, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        0, &modeCount, nullptr);
    if (XR_FAILED(result))
    {
        return Fail("Environment blend mode count", result);
    }
    if (!modeCount)
    {
        return Fail("Opaque environment blend mode unavailable",
                    XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED);
    }
    std::vector<XrEnvironmentBlendMode> modes(modeCount);
    result = xrEnumerateEnvironmentBlendModes(
        instance_, systemId_, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        modeCount, &modeCount, modes.data());
    if (XR_FAILED(result))
    {
        return Fail("Environment blend mode enumeration", result);
    }
    if (modeCount > modes.size())
    {
        return Fail("Invalid environment blend mode count", XR_ERROR_SIZE_INSUFFICIENT);
    }
    modes.resize(modeCount);
    if (std::find(modes.begin(), modes.end(), XR_ENVIRONMENT_BLEND_MODE_OPAQUE) ==
        modes.end())
    {
        return Fail("Opaque environment blend mode unavailable",
                    XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED);
    }

    PFN_xrGetOpenGLGraphicsRequirementsKHR getGraphicsRequirements =
        nullptr;
    result = xrGetInstanceProcAddr(
        instance_,
        "xrGetOpenGLGraphicsRequirementsKHR",
        reinterpret_cast<PFN_xrVoidFunction *>(
            &getGraphicsRequirements));

    if (XR_FAILED(result) || !getGraphicsRequirements)
    {
        return Fail(
            "OpenGL function lookup",
            XR_FAILED(result) ? result : XR_ERROR_FUNCTION_UNSUPPORTED);
    }

    XrGraphicsRequirementsOpenGLKHR requirements{
        XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR
    };
    result = getGraphicsRequirements(
        instance_, systemId_, &requirements);
    if (XR_FAILED(result))
    {
        return Fail("OpenGL graphics requirements", result);
    }

    std::ostringstream output;
    output << "Runtime: " << instanceProperties.runtimeName
           << "\nSystem: " << systemProperties.systemName
           << "\nOpenGL Extension: Supported"
           << "\nMinimum OpenGL: "
           << XR_VERSION_MAJOR(requirements.minApiVersionSupported)
           << "."
           << XR_VERSION_MINOR(requirements.minApiVersionSupported)
           << "\nMaximum Tested OpenGL: "
           << XR_VERSION_MAJOR(requirements.maxApiVersionSupported)
           << "."
           << XR_VERSION_MINOR(requirements.maxApiVersionSupported)
           << "\nOpenXR Runtime Ready";

    status_ = output.str();
    initialized_ = true;
    return true;
}

bool XRRuntime::ProbeGraphicsContext() const
{
    Display *display = glXGetCurrentDisplay();
    GLXContext context = glXGetCurrentContext();
    GLXDrawable drawable = glXGetCurrentDrawable();

    if (!display || !context || !drawable)
    {
        std::cout << "[XRRuntime] No current GLX context" << std::endl;
        return false;
    }

    std::cout << "[XRRuntime] GLX Context Ready\n"
              << "OpenGL Vendor: " << glGetString(GL_VENDOR)
              << "\nOpenGL Renderer: " << glGetString(GL_RENDERER)
              << "\nOpenGL Version: " << glGetString(GL_VERSION)
              << std::endl;

    return true;
}

bool XRRuntime::CreateSessionFromCurrentContext()
{
    if (session_ != XR_NULL_HANDLE)
    {
        return true;
    }

    if (!initialized_)
    {
        status_ = "OpenXR runtime is not initialized";
        return false;
    }

    if (!ProbeGraphicsContext())
    {
        status_ = "No current GLX context in render event";
        return false;
    }

    Display *display = glXGetCurrentDisplay();
    GLXContext context = glXGetCurrentContext();
    GLXDrawable drawable = glXGetCurrentDrawable();
    int configId = 0;
    int screen = 0;

    if (glXQueryContext(display, context, GLX_FBCONFIG_ID, &configId) != 0 ||
        glXQueryContext(display, context, GLX_SCREEN, &screen) != 0)
    {
        status_ = "GLX context query failed";
        return false;
    }

    const int attributes[] = {GLX_FBCONFIG_ID, configId, None};
    int configCount = 0;
    GLXFBConfig *configs = glXChooseFBConfig(
        display, screen, attributes, &configCount);

    if (!configs || configCount == 0)
    {
        if (configs)
        {
            XFree(configs);
        }

        status_ = "GLX FBConfig not found";
        return false;
    }

    const GLXFBConfig config = configs[0];
    XFree(configs);

    int visualId = 0;
    if (glXGetFBConfigAttrib(
            display, config, GLX_VISUAL_ID, &visualId) != 0 ||
        visualId == 0)
    {
        status_ = "GLX Visual ID unavailable";
        return false;
    }

    XrGraphicsBindingOpenGLXlibKHR binding{
        XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR
    };
    binding.xDisplay = display;
    binding.visualid = visualId;
    binding.glxFBConfig = config;
    binding.glxDrawable = drawable;
    binding.glxContext = context;

    XrSessionCreateInfo createInfo{XR_TYPE_SESSION_CREATE_INFO};
    createInfo.next = &binding;
    createInfo.systemId = systemId_;

    XrResult result = xrCreateSession(instance_, &createInfo, &session_);
    if (XR_FAILED(result))
    {
        session_ = XR_NULL_HANDLE;
        return Fail("xrCreateSession", result);
    }
    sessionContext_ = context;

    XrReferenceSpaceCreateInfo spaceInfo{
        XR_TYPE_REFERENCE_SPACE_CREATE_INFO
    };
    spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    spaceInfo.poseInReferenceSpace.orientation.w = 1.0f;

    result = xrCreateReferenceSpace(session_, &spaceInfo, &localSpace_);
    if (XR_FAILED(result))
    {
        DestroySession();
        return Fail("Reference space creation", result);
    }

    status_ = "OpenXR Session Created Successfully";
    std::cout << "[XRRuntime] " << status_ << std::endl;
    return true;
}

bool XRRuntime::PollEvents()
{
    if (instance_ == XR_NULL_HANDLE)
    {
        return false;
    }

    XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};
    while (true)
    {
        event = {};
        event.type = XR_TYPE_EVENT_DATA_BUFFER;

        XrResult result = xrPollEvent(instance_, &event);
        if (result == XR_EVENT_UNAVAILABLE)
        {
            break;
        }

        if (XR_FAILED(result))
        {
            status_ = "xrPollEvent failed: " +
                std::to_string(static_cast<int>(result));
            return false;
        }

        if (event.type == XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING)
        {
            status_ = "OpenXR instance loss pending";
            Shutdown();
            return false;
        }

        if (event.type != XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
        {
            continue;
        }

        const auto *stateEvent =
            reinterpret_cast<const XrEventDataSessionStateChanged *>(
                &event);
        if (stateEvent->session != session_)
        {
            continue;
        }

        sessionState_ = stateEvent->state;
        std::cout << "[XRRuntime] Session state: "
                  << static_cast<int>(sessionState_) << std::endl;

        switch (sessionState_)
        {
            case XR_SESSION_STATE_READY:
            {
                XrSessionBeginInfo beginInfo{
                    XR_TYPE_SESSION_BEGIN_INFO
                };
                beginInfo.primaryViewConfigurationType =
                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

                result = xrBeginSession(session_, &beginInfo);
                if (XR_FAILED(result))
                {
                    status_ = "xrBeginSession failed: " +
                        std::to_string(static_cast<int>(result));
                    return false;
                }

                sessionRunning_ = true;
                frameCount_ = 0;
                std::cout << "[XRRuntime] Session started" << std::endl;
                break;
            }

            case XR_SESSION_STATE_STOPPING:
                if (sessionRunning_)
                {
                    result = xrEndSession(session_);
                    if (XR_FAILED(result))
                    {
                        status_ = "xrEndSession failed: " +
                            std::to_string(static_cast<int>(result));
                        return false;
                    }
                }

                sessionRunning_ = false;
                std::cout << "[XRRuntime] Session ended" << std::endl;
                break;

            case XR_SESSION_STATE_EXITING:
            case XR_SESSION_STATE_LOSS_PENDING:
                DestroySession();
                status_ = "OpenXR session closed";
                return true;

            default:
                break;
        }
    }

    if (exitRequested_ &&
        sessionState_ == XR_SESSION_STATE_IDLE &&
        !sessionRunning_)
    {
        DestroySession();
        status_ = "OpenXR Session Stopped";
    }

    return true;
}

bool XRRuntime::RequestStop()
{
    if (session_ == XR_NULL_HANDLE)
    {
        return true;
    }

    if (exitRequested_)
    {
        return true;
    }

    if (!sessionRunning_)
    {
        DestroySession();
        return true;
    }

    const XrResult result = xrRequestExitSession(session_);
    if (XR_FAILED(result))
    {
        status_ = "xrRequestExitSession failed: " +
            std::to_string(static_cast<int>(result));
        return false;
    }

    exitRequested_ = true;
    return true;
}

bool XRRuntime::RenderFrame(uint32_t leftTexture, uint32_t rightTexture)
{
    if (!sessionRunning_ || session_ == XR_NULL_HANDLE)
    {
        return false;
    }

    XrFrameWaitInfo waitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    XrResult result = xrWaitFrame(session_, &waitInfo, &frameState);
    if (XR_FAILED(result))
    {
        status_ = "xrWaitFrame failed: " +
            std::to_string(static_cast<int>(result));
        return false;
    }

    XrFrameBeginInfo beginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    result = xrBeginFrame(session_, &beginInfo);
    if (XR_FAILED(result))
    {
        status_ = "xrBeginFrame failed: " +
            std::to_string(static_cast<int>(result));
        return false;
    }

    XrView views[2]{{XR_TYPE_VIEW}, {XR_TYPE_VIEW}};
    XrCompositionLayerProjectionView projectionViews[2]{
        {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW},
        {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW}};
    XrCompositionLayerProjection projection{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    const XrCompositionLayerBaseHeader *layers[1]{};
    uint32_t layerCount = 0;
    bool frameSucceeded = true;
    if (frameState.shouldRender && swapchainsReady_ && !framePreparationFailed_ &&
        leftTexture && rightTexture)
    {
        waitingForCameraReported_ = false;
        const auto location = LocateStereoViews(frameState.predictedDisplayTime, views);
        if (location == ViewLocationResult::Error)
        {
            frameSucceeded = false;
        }
        else if (location == ViewLocationResult::TrackingUnavailable)
        {
            if (!trackingUnavailable_)
            {
                std::cerr << "[XRRuntime] Tracking unavailable; projection skipped"
                          << std::endl;
            }
            trackingUnavailable_ = true;
        }
        else
        {
            trackingUnavailable_ = false;
            frameSucceeded = CopyStereoFrame(leftTexture, rightTexture);
            if (frameSucceeded)
            {
                if (!stereoFrameCopied_)
                {
                    stereoFrameCopied_ = true;
                    std::cout << "[XRRuntime] Stereo swapchain copy SUCCESS"
                              << std::endl;
                }
                for (std::size_t eye = 0; eye < 2; ++eye)
                {
                    projectionViews[eye].pose = views[eye].pose;
                    projectionViews[eye].fov = views[eye].fov;
                    projectionViews[eye].subImage.swapchain = eyeSwapchains_[eye].handle;
                    projectionViews[eye].subImage.imageRect.offset = {0, 0};
                    projectionViews[eye].subImage.imageRect.extent = {
                        static_cast<int32_t>(eyeSwapchains_[eye].width),
                        static_cast<int32_t>(eyeSwapchains_[eye].height)};
                    projectionViews[eye].subImage.imageArrayIndex = 0;
                }
                projection.space = localSpace_;
                projection.viewCount = 2;
                projection.views = projectionViews;
                layers[0] = reinterpret_cast<const XrCompositionLayerBaseHeader *>(
                    &projection);
                layerCount = 1;
            }
        }
        if (!frameSucceeded)
        {
            framePreparationFailed_ = true;
            std::cerr << "[XRRuntime] Frame preparation failed: "
                      << status_ << std::endl;
        }
    }
    else if (frameState.shouldRender && swapchainsReady_ &&
             !framePreparationFailed_ && !waitingForCameraReported_)
    {
        std::cerr << "[XRRuntime] Waiting for validated stereo camera textures"
                  << std::endl;
        waitingForCameraReported_ = true;
    }

    XrFrameEndInfo endInfo{XR_TYPE_FRAME_END_INFO};
    endInfo.displayTime = frameState.predictedDisplayTime;
    endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    endInfo.layerCount = layerCount;
    endInfo.layers = layerCount ? layers : nullptr;

    result = xrEndFrame(session_, &endInfo);
    if (XR_FAILED(result))
    {
        if (!frameSucceeded)
        {
            status_ += "\n";
        }
        else
        {
            status_.clear();
        }
        status_ += "xrEndFrame failed: " +
            std::to_string(static_cast<int>(result));
        return false;
    }

    if (layerCount && !projectionLayerSubmitted_)
    {
        projectionLayerSubmitted_ = true;
        std::cout << "[XRRuntime] Stereo projection layer submitted" << std::endl;
    }
    ++frameCount_;
    if (frameCount_ % 120 == 0)
    {
        std::cout << "[XRRuntime] Frame: " << frameCount_
                  << " | shouldRender: " << frameState.shouldRender
                  << " | predictedDisplayTime: "
                  << frameState.predictedDisplayTime
                  << " | Submitted layers: " << layerCount
                  << std::endl;
    }

    return frameSucceeded;
}

XRRuntime::ViewLocationResult XRRuntime::LocateStereoViews(
    XrTime displayTime, XrView (&views)[2])
{
    if (session_ == XR_NULL_HANDLE || localSpace_ == XR_NULL_HANDLE)
    {
        status_ = "Session or local reference space is not ready";
        return ViewLocationResult::Error;
    }
    XrViewLocateInfo info{XR_TYPE_VIEW_LOCATE_INFO};
    info.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    info.displayTime = displayTime;
    info.space = localSpace_;
    XrViewState state{XR_TYPE_VIEW_STATE};
    uint32_t count = 0;
    const XrResult result = xrLocateViews(session_, &info, &state, 2, &count, views);
    if (XR_FAILED(result))
    {
        status_ = "xrLocateViews failed: " +
            std::to_string(static_cast<int>(result));
        return ViewLocationResult::Error;
    }
    if (count != 2)
    {
        status_ = "Unexpected OpenXR view count: " + std::to_string(count);
        return ViewLocationResult::Error;
    }
    constexpr XrViewStateFlags required = XR_VIEW_STATE_POSITION_VALID_BIT |
        XR_VIEW_STATE_ORIENTATION_VALID_BIT;
    if ((state.viewStateFlags & required) != required)
    {
        return ViewLocationResult::TrackingUnavailable;
    }
    if ((frameCount_ + 1) % 120 == 0)
    {
        for (std::size_t eye = 0; eye < 2; ++eye)
        {
            const auto &position = views[eye].pose.position;
            std::cout << "[XRRuntime] View " << eye << " | Position: "
                      << position.x << ", " << position.y << ", "
                      << position.z << std::endl;
        }
    }
    return ViewLocationResult::Ready;
}

bool XRRuntime::TestSwapchainImageCycle()
{
    return ProcessSwapchainImages(nullptr);
}

bool XRRuntime::CopyStereoFrame(uint32_t leftTexture, uint32_t rightTexture)
{
    if (!sessionContext_ || glXGetCurrentContext() != sessionContext_)
    {
        status_ = "OpenXR session GLX context is not current";
        return false;
    }
    const uint32_t sourceTextures[] = {leftTexture, rightTexture};
    return ProcessSwapchainImages(sourceTextures);
}

bool XRRuntime::ProcessSwapchainImages(const uint32_t *sourceTextures)
{
    if (eyeSwapchains_.size() != 2)
    {
        status_ = "Expected two ready eye swapchains";
        return false;
    }

    for (std::size_t eye = 0; eye < eyeSwapchains_.size(); ++eye)
    {
        const auto &swapchain = eyeSwapchains_[eye];
        XrSwapchainImageAcquireInfo acquireInfo{
            XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        uint32_t imageIndex = 0;
        XrResult result = xrAcquireSwapchainImage(
            swapchain.handle, &acquireInfo, &imageIndex);
        if (XR_FAILED(result))
        {
            status_ = "xrAcquireSwapchainImage failed for eye " +
                std::to_string(eye) + ": " +
                std::to_string(static_cast<int>(result));
            return false;
        }

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        result = xrWaitSwapchainImage(swapchain.handle, &waitInfo);
        if (result != XR_SUCCESS)
        {
            status_ = "xrWaitSwapchainImage failed for eye " +
                std::to_string(eye) + ": " +
                std::to_string(static_cast<int>(result));
            return false;
        }

        const bool validIndex = imageIndex < swapchain.textureIds.size();
        bool copied = validIndex;
        if (!validIndex)
        {
            status_ = "Invalid acquired image index for eye " +
                std::to_string(eye) + ": " + std::to_string(imageIndex);
        }
        else if (sourceTextures)
        {
            copied = texture_blit::BlitTexture(
                sourceTextures[eye], swapchain.textureIds[imageIndex]);
            if (!copied)
            {
                status_ = "OpenGL framebuffer copy failed for eye " +
                    std::to_string(eye);
            }
            else if ((frameCount_ + 1) % 120 == 0)
            {
                std::cout << "[XRRuntime] Eye " << eye << " | Source texture: "
                          << sourceTextures[eye] << " | XR texture: "
                          << swapchain.textureIds[imageIndex]
                          << " | Copy SUCCESS" << std::endl;
            }
        }
        else if ((frameCount_ + 1) % 120 == 0)
        {
            std::cout << "[XRRuntime] Eye " << eye << " acquired image "
                      << imageIndex << " | Texture ID: "
                      << swapchain.textureIds[imageIndex] << std::endl;
        }

        XrSwapchainImageReleaseInfo releaseInfo{
            XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        result = xrReleaseSwapchainImage(swapchain.handle, &releaseInfo);
        if (XR_FAILED(result))
        {
            if (!copied)
            {
                status_ += "\n";
            }
            else
            {
                status_.clear();
            }
            status_ += "xrReleaseSwapchainImage failed for eye " +
                std::to_string(eye) + ": " +
                std::to_string(static_cast<int>(result));
            return false;
        }
        if (!copied)
        {
            return false;
        }
    }
    return true;
}

bool XRRuntime::QueryStereoViews(
    std::vector<XrViewConfigurationView> &views)
{
    if (instance_ == XR_NULL_HANDLE ||
        systemId_ == XR_NULL_SYSTEM_ID ||
        session_ == XR_NULL_HANDLE)
    {
        status_ = "OpenXR session is not ready for view inspection";
        return false;
    }

    constexpr XrViewConfigurationType viewType =
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    uint32_t count = 0;
    XrResult result = xrEnumerateViewConfigurationViews(
        instance_, systemId_, viewType, 0, &count, nullptr);
    if (XR_FAILED(result) || count == 0)
    {
        status_ = XR_FAILED(result)
            ? "View configuration enumeration failed: " +
                std::to_string(static_cast<int>(result))
            : "No stereo view configurations available";
        return false;
    }

    views.assign(count, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    result = xrEnumerateViewConfigurationViews(
        instance_, systemId_, viewType, count, &count, views.data());
    if (XR_FAILED(result))
    {
        status_ = "Unable to read view configurations: " +
            std::to_string(static_cast<int>(result));
        return false;
    }
    if (count == 0 || count > views.size())
    {
        status_ = "Invalid stereo view configuration count";
        return false;
    }
    views.resize(count);
    return true;
}

bool XRRuntime::QuerySwapchainFormats(std::vector<int64_t> &formats)
{
    uint32_t formatCount = 0;
    XrResult result = xrEnumerateSwapchainFormats(
        session_, 0, &formatCount, nullptr);
    if (XR_FAILED(result) || formatCount == 0)
    {
        status_ = XR_FAILED(result)
            ? "Swapchain format enumeration failed: " +
                std::to_string(static_cast<int>(result))
            : "No swapchain formats available";
        return false;
    }

    formats.resize(formatCount);
    result = xrEnumerateSwapchainFormats(
        session_, formatCount, &formatCount, formats.data());
    if (XR_FAILED(result))
    {
        status_ = "Unable to read swapchain formats: " +
            std::to_string(static_cast<int>(result));
        return false;
    }
    if (formatCount == 0 || formatCount > formats.size())
    {
        status_ = "Invalid swapchain format count";
        return false;
    }
    formats.resize(formatCount);
    return true;
}

bool XRRuntime::InspectViewConfiguration()
{
    if (viewConfigurationInspected_)
    {
        return true;
    }

    std::vector<XrViewConfigurationView> views;
    if (!QueryStereoViews(views))
    {
        return false;
    }

    std::cout << "[XRRuntime] View count: " << views.size() << std::endl;
    for (std::size_t i = 0; i < views.size(); ++i)
    {
        const auto &view = views[i];
        std::cout << "[XRRuntime] Eye " << i
                  << " | Recommended: "
                  << view.recommendedImageRectWidth << "x"
                  << view.recommendedImageRectHeight
                  << " | Maximum: "
                  << view.maxImageRectWidth << "x"
                  << view.maxImageRectHeight
                  << " | Recommended samples: "
                  << view.recommendedSwapchainSampleCount
                  << std::endl;
    }

    std::vector<int64_t> formats;
    if (!QuerySwapchainFormats(formats))
    {
        return false;
    }

    std::cout << "[XRRuntime] Supported OpenGL formats:" << std::endl;
    for (const auto format : formats)
    {
        std::cout << "  0x" << std::hex << format << std::dec
                  << std::endl;
    }

    viewConfigurationInspected_ = true;
    return true;
}

bool XRRuntime::CreateColorSwapchains()
{
    if (swapchainsReady_)
    {
        return true;
    }

    std::vector<XrViewConfigurationView> views;
    if (!QueryStereoViews(views))
    {
        return false;
    }
    if (views.size() != 2)
    {
        status_ = "Expected two stereo views, got " +
            std::to_string(views.size());
        return false;
    }

    std::vector<int64_t> formats;
    if (!QuerySwapchainFormats(formats))
    {
        return false;
    }
    constexpr int64_t srgb8Alpha8 = 0x8c43;
    constexpr int64_t rgba8 = 0x8058;
    const auto supported = [&](int64_t format)
    {
        return std::find(formats.begin(), formats.end(), format) != formats.end();
    };
    const int64_t selectedFormat =
        supported(srgb8Alpha8) ? srgb8Alpha8 : (supported(rgba8) ? rgba8 : 0);
    if (!selectedFormat)
    {
        status_ = "No compatible RGBA swapchain format";
        return false;
    }

    eyeSwapchains_.reserve(views.size());
    for (std::size_t eye = 0; eye < views.size(); ++eye)
    {
        const auto &view = views[eye];
        if (!view.recommendedImageRectWidth ||
            !view.recommendedImageRectHeight ||
            !view.recommendedSwapchainSampleCount ||
            view.recommendedImageRectWidth >
                static_cast<uint32_t>(std::numeric_limits<int32_t>::max()) ||
            view.recommendedImageRectHeight >
                static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
        {
            status_ = "Invalid recommended swapchain dimensions or sample count";
            DestroyColorSwapchains();
            return false;
        }

        eyeSwapchains_.emplace_back();
        auto &swapchain = eyeSwapchains_.back();
        swapchain.width = view.recommendedImageRectWidth;
        swapchain.height = view.recommendedImageRectHeight;

        XrSwapchainCreateInfo createInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        createInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.format = selectedFormat;
        createInfo.sampleCount = 1;
        createInfo.width = swapchain.width;
        createInfo.height = swapchain.height;
        createInfo.faceCount = 1;
        createInfo.arraySize = 1;
        createInfo.mipCount = 1;

        XrResult result = xrCreateSwapchain(
            session_, &createInfo, &swapchain.handle);
        if (XR_FAILED(result))
        {
            status_ = "xrCreateSwapchain failed: " +
                std::to_string(static_cast<int>(result));
            DestroyColorSwapchains();
            return false;
        }

        uint32_t imageCount = 0;
        result = xrEnumerateSwapchainImages(
            swapchain.handle, 0, &imageCount, nullptr);
        if (XR_FAILED(result) || imageCount == 0)
        {
            status_ = XR_FAILED(result)
                ? "Swapchain image count failed: " +
                    std::to_string(static_cast<int>(result))
                : "Swapchain has no images";
            DestroyColorSwapchains();
            return false;
        }

        std::vector<XrSwapchainImageOpenGLKHR> images(
            imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR});
        result = xrEnumerateSwapchainImages(
            swapchain.handle, imageCount, &imageCount,
            reinterpret_cast<XrSwapchainImageBaseHeader *>(images.data()));
        if (XR_FAILED(result))
        {
            status_ = "Swapchain image enumeration failed: " +
                std::to_string(static_cast<int>(result));
            DestroyColorSwapchains();
            return false;
        }
        if (imageCount == 0 || imageCount > images.size())
        {
            status_ = "Invalid swapchain image count";
            DestroyColorSwapchains();
            return false;
        }

        swapchain.textureIds.reserve(imageCount);
        std::cout << "[XRRuntime] Eye " << eye << " | Resolution: "
                  << swapchain.width << "x" << swapchain.height
                  << " | Image count: " << imageCount << " | Format: 0x"
                  << std::hex << selectedFormat << std::dec << std::endl;
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            swapchain.textureIds.push_back(images[i].image);
            std::cout << "  Image[" << i << "] GL Texture ID: "
                      << images[i].image << std::endl;
        }
    }

    swapchainsReady_ = true;
    std::cout << "[XRRuntime] Stereo Swapchains Ready" << std::endl;
    return true;
}

bool XRRuntime::AreColorSwapchainsReady() const
{
    return swapchainsReady_;
}

bool XRRuntime::HasCopiedStereoFrame() const
{
    return stereoFrameCopied_;
}

bool XRRuntime::HasSubmittedProjectionLayer() const
{
    return projectionLayerSubmitted_;
}

bool XRRuntime::IsTrackingUnavailable() const
{
    return trackingUnavailable_;
}

void XRRuntime::DestroyColorSwapchains()
{
    for (auto &eye : eyeSwapchains_)
    {
        if (eye.handle != XR_NULL_HANDLE)
        {
            const XrResult result = xrDestroySwapchain(eye.handle);
            if (XR_FAILED(result))
            {
                std::cerr << "[XRRuntime] xrDestroySwapchain failed: "
                          << static_cast<int>(result) << std::endl;
            }
        }
    }
    eyeSwapchains_.clear();
    swapchainsReady_ = false;
}

void XRRuntime::DestroySession()
{
    DestroyColorSwapchains();

    if (localSpace_ != XR_NULL_HANDLE)
    {
        xrDestroySpace(localSpace_);
        localSpace_ = XR_NULL_HANDLE;
    }

    if (session_ != XR_NULL_HANDLE)
    {
        xrDestroySession(session_);
        session_ = XR_NULL_HANDLE;
    }

    sessionRunning_ = false;
    sessionContext_ = nullptr;
    sessionState_ = XR_SESSION_STATE_UNKNOWN;
    exitRequested_ = false;
    framePreparationFailed_ = false;
    stereoFrameCopied_ = false;
    projectionLayerSubmitted_ = false;
    trackingUnavailable_ = false;
    waitingForCameraReported_ = false;
    frameCount_ = 0;
    viewConfigurationInspected_ = false;
}

void XRRuntime::Shutdown()
{
    DestroySession();

    if (instance_ != XR_NULL_HANDLE)
    {
        xrDestroyInstance(instance_);
        instance_ = XR_NULL_HANDLE;
    }

    systemId_ = XR_NULL_SYSTEM_ID;
    initialized_ = false;
}

bool XRRuntime::Fail(
    const std::string &message,
    XrResult result)
{
    status_ = message + " failed: " +
        std::to_string(static_cast<int>(result));
    Shutdown();
    return false;
}

bool XRRuntime::IsInitialized() const
{
    return initialized_;
}

bool XRRuntime::IsSessionRunning() const
{
    return sessionRunning_;
}

XrInstance XRRuntime::GetInstance() const
{
    return instance_;
}

XrSystemId XRRuntime::GetSystemId() const
{
    return systemId_;
}

XrSession XRRuntime::GetSession() const
{
    return session_;
}

XrSpace XRRuntime::GetLocalSpace() const
{
    return localSpace_;
}

std::string XRRuntime::GetStatus() const
{
    return status_;
}
