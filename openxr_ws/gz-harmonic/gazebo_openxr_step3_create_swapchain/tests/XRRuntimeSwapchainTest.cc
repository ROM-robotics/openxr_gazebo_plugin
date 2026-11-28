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
    std::cout << "Swapchain lifecycle tests passed\n";
    return 0;
}
