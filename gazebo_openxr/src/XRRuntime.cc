#include "XRRuntime.hh"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#define XR_USE_PLATFORM_XLIB
#define XR_USE_GRAPHICS_API_OPENGL

#include <openxr/openxr_platform.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

XRRuntime::~XRRuntime()
{
    Shutdown();
}

bool XRRuntime::Initialize()
{
    if (initialized_)
        return true;

    uint32_t count = 0;

    XrResult result =
        xrEnumerateInstanceExtensionProperties(
            nullptr, 0, &count, nullptr);

    if (XR_FAILED(result))
        return Fail("Extension enumeration", result);

    std::vector<XrExtensionProperties> extensions(
        count, {XR_TYPE_EXTENSION_PROPERTIES});

    result = xrEnumerateInstanceExtensionProperties(
        nullptr, count, &count, extensions.data());

    if (XR_FAILED(result))
        return Fail("Extension properties", result);

    bool openGLSupported = false;

    for (const auto& extension : extensions)
    {
        if (std::strcmp(
                extension.extensionName,
                XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) == 0)
        {
            openGLSupported = true;
        }
    }

    if (!openGLSupported)
    {
        status_ = "OpenGL extension not supported";
        return false;
    }

    const char* enabledExtensions[] = {
        XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
    };

    XrInstanceCreateInfo createInfo{
        XR_TYPE_INSTANCE_CREATE_INFO
    };

    std::strncpy(
        createInfo.applicationInfo.applicationName,
        "GazeboOpenXR",
        XR_MAX_APPLICATION_NAME_SIZE - 1);

    createInfo.applicationInfo.applicationVersion = 1;
    createInfo.applicationInfo.apiVersion =
        XR_MAKE_VERSION(1, 0, 0);

    createInfo.enabledExtensionCount = 1;
    createInfo.enabledExtensionNames = enabledExtensions;

    result = xrCreateInstance(&createInfo, &instance_);

    if (XR_FAILED(result))
        return Fail("xrCreateInstance", result);

    XrInstanceProperties instanceProperties{
        XR_TYPE_INSTANCE_PROPERTIES
    };

    result = xrGetInstanceProperties(
        instance_, &instanceProperties);

    if (XR_FAILED(result))
        return Fail("xrGetInstanceProperties", result);

    XrSystemGetInfo systemInfo{
        XR_TYPE_SYSTEM_GET_INFO
    };

    systemInfo.formFactor =
        XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(
        instance_, &systemInfo, &systemId_);

    if (XR_FAILED(result))
        return Fail("xrGetSystem", result);

    XrSystemProperties systemProperties{
        XR_TYPE_SYSTEM_PROPERTIES
    };

    result = xrGetSystemProperties(
        instance_, systemId_, &systemProperties);

    if (XR_FAILED(result))
        return Fail("xrGetSystemProperties", result);

    PFN_xrGetOpenGLGraphicsRequirementsKHR
        getGraphicsRequirements = nullptr;

    result = xrGetInstanceProcAddr(
        instance_,
        "xrGetOpenGLGraphicsRequirementsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(
            &getGraphicsRequirements));

    if (XR_FAILED(result) || !getGraphicsRequirements)
        return Fail("OpenGL function lookup",
                    XR_FAILED(result)
                    ? result : XR_ERROR_FUNCTION_UNSUPPORTED);

    XrGraphicsRequirementsOpenGLKHR requirements{
        XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR
    };

    result = getGraphicsRequirements(
        instance_, systemId_, &requirements);

    if (XR_FAILED(result))
        return Fail("OpenGL graphics requirements", result);

    std::ostringstream output;

    output << "Runtime: "
           << instanceProperties.runtimeName
           << "\nSystem: "
           << systemProperties.systemName
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
    const std::string& message, XrResult result)
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

XrInstance XRRuntime::GetInstance() const
{
    return instance_;
}

XrSystemId XRRuntime::GetSystemId() const
{
    return systemId_;
}

std::string XRRuntime::GetStatus() const
{
    return status_;
}

bool XRRuntime::ProbeGraphicsContext()
{
    Display* display = glXGetCurrentDisplay();

    GLXContext context = glXGetCurrentContext();

    GLXDrawable drawable = glXGetCurrentDrawable();

    if (!display || !context || !drawable)
    {
        std::cout
            << "[XRRuntime] No current GLX context"
            << std::endl;

        return false;
    }

    std::cout
        << "[XRRuntime] GLX Context Ready\n"
        << "OpenGL Vendor: "
        << glGetString(GL_VENDOR)
        << "\nOpenGL Renderer: "
        << glGetString(GL_RENDERER)
        << "\nOpenGL Version: "
        << glGetString(GL_VERSION)
        << std::endl;

    return true;
}

bool XRRuntime::CreateSessionFromCurrentContext()
{
    if (session_ != XR_NULL_HANDLE)
        return true;

    if (!initialized_)
        return false;

    if (!ProbeGraphicsContext())
        return false;

    Display* display = glXGetCurrentDisplay();
    GLXContext context = glXGetCurrentContext();
    GLXDrawable drawable = glXGetCurrentDrawable();

    int configId = 0;
    int screen = 0;

    if (glXQueryContext(display, context,
                        GLX_FBCONFIG_ID, &configId) != 0 ||
        glXQueryContext(display, context,
                        GLX_SCREEN, &screen) != 0)
    {
        status_ = "GLX context query failed";
        return false;
    }

    int attributes[] = {
        GLX_FBCONFIG_ID, configId,
        None
    };

    int count = 0;

    GLXFBConfig* configs = glXChooseFBConfig(
        display, screen, attributes, &count);

    if (!configs || count == 0)
    {
        if (configs)
            XFree(configs);

        status_ = "GLX FBConfig not found";
        return false;
    }

    GLXFBConfig config = configs[0];
    XFree(configs);

    int visualId = 0;

    if (glXGetFBConfigAttrib(
            display, config,
            GLX_VISUAL_ID, &visualId) != 0 ||
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

    XrSessionCreateInfo createInfo{
        XR_TYPE_SESSION_CREATE_INFO
    };

    createInfo.next = &binding;
    createInfo.systemId = systemId_;

    XrResult result = xrCreateSession(
        instance_, &createInfo, &session_);

    if (XR_FAILED(result))
    {
        status_ = "xrCreateSession failed: " +
            std::to_string(static_cast<int>(result));

        session_ = XR_NULL_HANDLE;
        return false;
    }

    XrReferenceSpaceCreateInfo spaceInfo{
        XR_TYPE_REFERENCE_SPACE_CREATE_INFO
    };

    spaceInfo.referenceSpaceType =
        XR_REFERENCE_SPACE_TYPE_LOCAL;

    spaceInfo.poseInReferenceSpace.orientation.w = 1.0f;

    result = xrCreateReferenceSpace(
        session_, &spaceInfo, &localSpace_);

    if (XR_FAILED(result))
    {
        status_ = "Reference space creation failed: " +
            std::to_string(static_cast<int>(result));

        DestroySession();
        return false;
    }

    status_ = "OpenXR Session Created Successfully";

    std::cout << "[XRRuntime] "
              << status_ << std::endl;

    return true;
}

void XRRuntime::DestroySession()
{
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
}

XrSession XRRuntime::GetSession() const
{
    return session_;
}


