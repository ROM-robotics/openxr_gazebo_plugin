#include "XRRuntime.hh"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#define XR_USE_PLATFORM_XLIB
#define XR_USE_GRAPHICS_API_OPENGL
#include <openxr/openxr_platform.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
struct FakeRuntime
{
    uint32_t viewCount = 2;
    std::vector<int64_t> formats{0x8058};
    int failCreateAt = 0;
    int failImageAt = 0;
    int emptyImageAt = 0;
    int createCalls = 0;
    int destroyCalls = 0;
    bool invalidCreateInfo = false;
    std::vector<XrSwapchainCreateInfo> creates;
    std::vector<std::string> destructionOrder;
    std::vector<std::string> imageCalls;
    std::string failImageOperation;
    int failEye = 0;
    XrResult imageFailure = XR_ERROR_RUNTIME_FAILURE;
    bool invalidImageIndex = false;
    bool invalidImageInfo = false;
    bool shouldRender = true;
    bool readyEvent = false;
    bool failEndFrame = false;
    bool invalidEndInfo = false;
    int endFrameCalls = 0;
} fake;

GLXFBConfig fakeConfig = reinterpret_cast<GLXFBConfig>(1);

XrResult XRAPI_CALL GraphicsRequirements(
    XrInstance, XrSystemId, XrGraphicsRequirementsOpenGLKHR *requirements)
{
    requirements->minApiVersionSupported = XR_MAKE_VERSION(3, 0, 0);
    requirements->maxApiVersionSupported = XR_MAKE_VERSION(4, 6, 0);
    return XR_SUCCESS;
}

void ResetCalls()
{
    fake.createCalls = 0;
    fake.destroyCalls = 0;
    fake.creates.clear();
    fake.destructionOrder.clear();
    fake.imageCalls.clear();
    fake.failImageOperation.clear();
    fake.failEye = 0;
    fake.imageFailure = XR_ERROR_RUNTIME_FAILURE;
    fake.invalidImageIndex = false;
    fake.invalidImageInfo = false;
    fake.shouldRender = true;
    fake.readyEvent = false;
    fake.failEndFrame = false;
    fake.invalidEndInfo = false;
    fake.endFrameCalls = 0;
}
}

extern "C"
{
Display *glXGetCurrentDisplay() { return reinterpret_cast<Display *>(1); }
GLXContext glXGetCurrentContext() { return reinterpret_cast<GLXContext>(1); }
GLXDrawable glXGetCurrentDrawable() { return 1; }
const GLubyte *glGetString(GLenum)
{
    return reinterpret_cast<const GLubyte *>("mock");
}
int glXQueryContext(Display *, GLXContext, int, int *value)
{
    *value = 1;
    return 0;
}
GLXFBConfig *glXChooseFBConfig(Display *, int, const int *, int *count)
{
    *count = 1;
    return &fakeConfig;
}
int glXGetFBConfigAttrib(Display *, GLXFBConfig, int, int *value)
{
    *value = 1;
    return 0;
}
int XFree(void *) { return 0; }

XrResult xrEnumerateInstanceExtensionProperties(
    const char *, uint32_t capacity, uint32_t *count,
    XrExtensionProperties *properties)
{
    *count = 1;
    if (capacity)
    {
        std::strcpy(properties[0].extensionName,
                    XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);
    }
    return XR_SUCCESS;
}
XrResult xrCreateInstance(const XrInstanceCreateInfo *, XrInstance *instance)
{
    *instance = reinterpret_cast<XrInstance>(1);
    return XR_SUCCESS;
}
XrResult xrDestroyInstance(XrInstance)
{
    fake.destructionOrder.emplace_back("instance");
    return XR_SUCCESS;
}
XrResult xrGetInstanceProperties(XrInstance, XrInstanceProperties *properties)
{
    std::strcpy(properties->runtimeName, "mock runtime");
    return XR_SUCCESS;
}
XrResult xrGetSystem(XrInstance, const XrSystemGetInfo *, XrSystemId *id)
{
    *id = 1;
    return XR_SUCCESS;
}
XrResult xrGetSystemProperties(XrInstance, XrSystemId, XrSystemProperties *properties)
{
    std::strcpy(properties->systemName, "mock system");
    return XR_SUCCESS;
}
XrResult xrGetInstanceProcAddr(XrInstance, const char *, PFN_xrVoidFunction *function)
{
    *function = reinterpret_cast<PFN_xrVoidFunction>(GraphicsRequirements);
    return XR_SUCCESS;
}
XrResult xrCreateSession(XrInstance, const XrSessionCreateInfo *, XrSession *session)
{
    *session = reinterpret_cast<XrSession>(1);
    return XR_SUCCESS;
}
XrResult xrDestroySession(XrSession)
{
    fake.destructionOrder.emplace_back("session");
    return XR_SUCCESS;
}
XrResult xrCreateReferenceSpace(
    XrSession, const XrReferenceSpaceCreateInfo *, XrSpace *space)
{
    *space = reinterpret_cast<XrSpace>(1);
    return XR_SUCCESS;
}
XrResult xrDestroySpace(XrSpace)
{
    fake.destructionOrder.emplace_back("space");
    return XR_SUCCESS;
}
XrResult xrEnumerateViewConfigurationViews(
    XrInstance, XrSystemId, XrViewConfigurationType type,
    uint32_t capacity, uint32_t *count, XrViewConfigurationView *views)
{
    if (type != XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO)
    {
        return XR_ERROR_VALIDATION_FAILURE;
    }
    *count = fake.viewCount;
    if (capacity)
    {
        for (uint32_t i = 0; i < fake.viewCount; ++i)
        {
            if (views[i].type != XR_TYPE_VIEW_CONFIGURATION_VIEW)
            {
                return XR_ERROR_VALIDATION_FAILURE;
            }
            views[i].recommendedImageRectWidth = 1000 + i;
            views[i].recommendedImageRectHeight = 1200 + i;
            views[i].recommendedSwapchainSampleCount = 1 + i;
        }
    }
    return XR_SUCCESS;
}
XrResult xrEnumerateSwapchainFormats(
    XrSession, uint32_t capacity, uint32_t *count, int64_t *formats)
{
    *count = fake.formats.size();
    if (capacity)
    {
        std::copy(fake.formats.begin(), fake.formats.end(), formats);
    }
    return XR_SUCCESS;
}
XrResult xrCreateSwapchain(
    XrSession, const XrSwapchainCreateInfo *info, XrSwapchain *swapchain)
{
    ++fake.createCalls;
    fake.creates.push_back(*info);
    if (fake.createCalls == fake.failCreateAt)
    {
        return XR_ERROR_RUNTIME_FAILURE;
    }
    if (info->type != XR_TYPE_SWAPCHAIN_CREATE_INFO ||
        info->usageFlags != XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT ||
        info->faceCount != 1 || info->arraySize != 1 || info->mipCount != 1)
    {
        fake.invalidCreateInfo = true;
    }
    *swapchain = reinterpret_cast<XrSwapchain>(
        static_cast<uintptr_t>(fake.createCalls));
    return XR_SUCCESS;
}
XrResult xrDestroySwapchain(XrSwapchain)
{
    ++fake.destroyCalls;
    fake.destructionOrder.emplace_back("swapchain");
    return XR_SUCCESS;
}
XrResult xrEnumerateSwapchainImages(
    XrSwapchain swapchain, uint32_t capacity, uint32_t *count,
    XrSwapchainImageBaseHeader *baseImages)
{
    const auto index = reinterpret_cast<uintptr_t>(swapchain);
    if (static_cast<int>(index) == fake.emptyImageAt)
    {
        *count = 0;
        return XR_SUCCESS;
    }
    if (static_cast<int>(index) == fake.failImageAt && capacity)
    {
        return XR_ERROR_RUNTIME_FAILURE;
    }
    *count = 2;
    if (capacity)
    {
        auto *images = reinterpret_cast<XrSwapchainImageOpenGLKHR *>(baseImages);
        for (uint32_t i = 0; i < *count; ++i)
        {
            if (images[i].type != XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR)
            {
                return XR_ERROR_VALIDATION_FAILURE;
            }
            images[i].image = static_cast<uint32_t>(index * 10 + i);
        }
    }
    return XR_SUCCESS;
}
XrResult xrPollEvent(XrInstance, XrEventDataBuffer *event)
{
    if (!fake.readyEvent)
    {
        return XR_EVENT_UNAVAILABLE;
    }
    fake.readyEvent = false;
    auto *state = reinterpret_cast<XrEventDataSessionStateChanged *>(event);
    state->type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED;
    state->session = reinterpret_cast<XrSession>(1);
    state->state = XR_SESSION_STATE_READY;
    return XR_SUCCESS;
}
XrResult xrBeginSession(XrSession, const XrSessionBeginInfo *)
{
    return XR_SUCCESS;
}
XrResult xrWaitFrame(XrSession, const XrFrameWaitInfo *, XrFrameState *state)
{
    state->shouldRender = fake.shouldRender ? XR_TRUE : XR_FALSE;
    state->predictedDisplayTime = 123;
    return XR_SUCCESS;
}
XrResult xrBeginFrame(XrSession, const XrFrameBeginInfo *)
{
    return XR_SUCCESS;
}
XrResult xrEndFrame(XrSession, const XrFrameEndInfo *info)
{
    ++fake.endFrameCalls;
    fake.invalidEndInfo = info->layerCount != 0 || info->layers != nullptr ||
        info->displayTime != 123;
    return fake.failEndFrame ? XR_ERROR_RUNTIME_FAILURE : XR_SUCCESS;
}
XrResult xrAcquireSwapchainImage(
    XrSwapchain swapchain, const XrSwapchainImageAcquireInfo *info, uint32_t *index)
{
    const int eye = static_cast<int>(reinterpret_cast<uintptr_t>(swapchain));
    fake.imageCalls.push_back("acquire" + std::to_string(eye));
    fake.invalidImageInfo |= info->type != XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
    if (fake.failImageOperation == "acquire" && fake.failEye == eye)
    {
        return fake.imageFailure;
    }
    *index = fake.invalidImageIndex ? 2 : 1;
    return XR_SUCCESS;
}
XrResult xrWaitSwapchainImage(
    XrSwapchain swapchain, const XrSwapchainImageWaitInfo *info)
{
    const int eye = static_cast<int>(reinterpret_cast<uintptr_t>(swapchain));
    fake.imageCalls.push_back("wait" + std::to_string(eye));
    fake.invalidImageInfo |= info->type != XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO ||
        info->timeout != XR_INFINITE_DURATION;
    if (fake.failImageOperation == "wait" && fake.failEye == eye)
    {
        return fake.imageFailure;
    }
    return XR_SUCCESS;
}
XrResult xrReleaseSwapchainImage(
    XrSwapchain swapchain, const XrSwapchainImageReleaseInfo *info)
{
    const int eye = static_cast<int>(reinterpret_cast<uintptr_t>(swapchain));
    fake.imageCalls.push_back("release" + std::to_string(eye));
    fake.invalidImageInfo |= info->type != XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;
    if (fake.failImageOperation == "release" && fake.failEye == eye)
    {
        return fake.imageFailure;
    }
    return XR_SUCCESS;
}
}

#define REQUIRE(condition) \
    do { if (!(condition)) { \
        std::cerr << "Failed at line " << __LINE__ << ": " #condition << '\n'; \
        return 1; \
    } } while (false)

int main()
{
    XRRuntime runtime;
    REQUIRE(!runtime.CreateColorSwapchains());
    REQUIRE(!runtime.AreColorSwapchainsReady());
    REQUIRE(runtime.Initialize());
    REQUIRE(runtime.CreateSessionFromCurrentContext());

    fake.viewCount = 1;
    REQUIRE(!runtime.CreateColorSwapchains());
    REQUIRE(runtime.GetStatus().find("Expected two stereo views") != std::string::npos);
    REQUIRE(fake.createCalls == 0);
    fake.viewCount = 2;

    fake.formats = {0x1234};
    REQUIRE(!runtime.CreateColorSwapchains());
    REQUIRE(runtime.GetStatus().find("No compatible RGBA") != std::string::npos);
    REQUIRE(fake.createCalls == 0);
    fake.formats = {0x8058};

    fake.failCreateAt = 2;
    REQUIRE(!runtime.CreateColorSwapchains());
    REQUIRE(fake.destroyCalls == 1);
    REQUIRE(!runtime.AreColorSwapchainsReady());
    fake.failCreateAt = 0;
    ResetCalls();

    fake.failImageAt = 2;
    REQUIRE(!runtime.CreateColorSwapchains());
    REQUIRE(runtime.GetStatus().find("Swapchain image enumeration failed") !=
        std::string::npos);
    REQUIRE(fake.destroyCalls == 2);
    REQUIRE(!runtime.AreColorSwapchainsReady());
    fake.failImageAt = 0;
    ResetCalls();

    fake.emptyImageAt = 2;
    REQUIRE(!runtime.CreateColorSwapchains());
    REQUIRE(runtime.GetStatus() == "Swapchain has no images");
    REQUIRE(fake.destroyCalls == 2);
    fake.emptyImageAt = 0;
    ResetCalls();

    REQUIRE(runtime.CreateColorSwapchains());
    REQUIRE(runtime.AreColorSwapchainsReady());
    REQUIRE(!fake.invalidCreateInfo);
    REQUIRE(fake.creates.size() == 2);
    REQUIRE(fake.creates[0].format == 0x8058 && fake.creates[1].format == 0x8058);
    REQUIRE(fake.creates[0].width == 1000 && fake.creates[1].width == 1001);
    REQUIRE(fake.creates[0].height == 1200 && fake.creates[1].height == 1201);
    REQUIRE(fake.creates[0].sampleCount == 1 && fake.creates[1].sampleCount == 2);
    REQUIRE(runtime.CreateColorSwapchains());
    REQUIRE(fake.createCalls == 2);
    REQUIRE(runtime.RequestStop());
    REQUIRE(!runtime.AreColorSwapchainsReady());
    REQUIRE(fake.destructionOrder ==
        std::vector<std::string>({"swapchain", "swapchain", "space", "session"}));

    ResetCalls();
    fake.formats = {0x8058, 0x8c43};
    REQUIRE(runtime.CreateSessionFromCurrentContext());
    REQUIRE(runtime.CreateColorSwapchains());
    REQUIRE(fake.creates[0].format == 0x8c43 && fake.creates[1].format == 0x8c43);
    runtime.Shutdown();
    REQUIRE(fake.destructionOrder == std::vector<std::string>(
        {"swapchain", "swapchain", "space", "session", "instance"}));

    for (const std::string &scenario :
         {"success", "no-render", "no-swapchain", "acquire", "wait", "timeout",
          "release", "invalid-index", "cycle-and-end"})
    {
        ResetCalls();
        REQUIRE(runtime.Initialize());
        REQUIRE(runtime.CreateSessionFromCurrentContext());
        fake.readyEvent = true;
        REQUIRE(runtime.PollEvents());
        REQUIRE(runtime.IsSessionRunning());
        if (scenario != "no-swapchain")
        {
            REQUIRE(runtime.CreateColorSwapchains());
        }

        fake.shouldRender = scenario != "no-render";
        fake.failEye = 2;
        if (scenario == "acquire" || scenario == "wait" || scenario == "release")
        {
            fake.failImageOperation = scenario;
        }
        else if (scenario == "timeout")
        {
            fake.failImageOperation = "wait";
            fake.imageFailure = XR_TIMEOUT_EXPIRED;
        }
        else if (scenario == "invalid-index")
        {
            fake.invalidImageIndex = true;
        }
        else if (scenario == "cycle-and-end")
        {
            fake.failImageOperation = "acquire";
            fake.failEndFrame = true;
        }

        const bool succeeded = runtime.RenderFrame();
        REQUIRE(succeeded == (scenario == "success" || scenario == "no-render" ||
            scenario == "no-swapchain"));
        REQUIRE(fake.endFrameCalls == 1);
        REQUIRE(!fake.invalidEndInfo);
        REQUIRE(!fake.invalidImageInfo);
        if (scenario == "success")
        {
            REQUIRE(fake.imageCalls == std::vector<std::string>(
                {"acquire1", "wait1", "release1", "acquire2", "wait2", "release2"}));
        }
        else if (scenario == "no-render" || scenario == "no-swapchain")
        {
            REQUIRE(fake.imageCalls.empty());
        }
        else
        {
            if (scenario == "invalid-index")
            {
                REQUIRE(fake.imageCalls == std::vector<std::string>(
                    {"acquire1", "wait1", "release1"}));
                REQUIRE(runtime.GetStatus().find("Invalid acquired image index") !=
                    std::string::npos);
            }
            else
            {
                const std::string operation =
                    scenario == "timeout" ? "wait" :
                    scenario == "cycle-and-end" ? "acquire" : scenario;
                std::vector<std::string> expected{
                    "acquire1", "wait1", "release1", "acquire2"};
                if (operation != "acquire")
                    expected.push_back("wait2");
                if (operation == "release")
                    expected.push_back("release2");
                REQUIRE(fake.imageCalls == expected);
                REQUIRE(runtime.GetStatus().find("eye 1") != std::string::npos);
            }
            if (scenario == "cycle-and-end")
            {
                REQUIRE(runtime.GetStatus().find("xrEndFrame failed") !=
                    std::string::npos);
            }
            const auto priorCalls = fake.imageCalls;
            fake.failEndFrame = false;
            REQUIRE(runtime.RenderFrame());
            REQUIRE(fake.imageCalls == priorCalls);
            REQUIRE(fake.endFrameCalls == 2);
        }
        runtime.Shutdown();
    }

    ResetCalls();
    REQUIRE(runtime.Initialize());
    REQUIRE(runtime.CreateSessionFromCurrentContext());
    fake.readyEvent = true;
    REQUIRE(runtime.PollEvents());
    REQUIRE(runtime.CreateColorSwapchains());
    std::ostringstream logs;
    auto *oldBuffer = std::cout.rdbuf(logs.rdbuf());
    bool framesSucceeded = true;
    for (int i = 0; i < 119; ++i)
    {
        framesSucceeded = runtime.RenderFrame() && framesSucceeded;
    }
    const std::string before120 = logs.str();
    framesSucceeded = runtime.RenderFrame() && framesSucceeded;
    std::cout.rdbuf(oldBuffer);
    REQUIRE(framesSucceeded);
    REQUIRE(before120.find("acquired image") == std::string::npos);
    REQUIRE(logs.str().find("Eye 0 acquired image 1 | Texture ID: 11") !=
        std::string::npos);
    REQUIRE(logs.str().find("Eye 1 acquired image 1 | Texture ID: 21") !=
        std::string::npos);
    runtime.Shutdown();
    std::cout << "Swapchain lifecycle tests passed\n";
    return 0;
}
