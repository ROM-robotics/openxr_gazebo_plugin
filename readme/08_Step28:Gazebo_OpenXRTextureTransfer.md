```Development Status
Step	Feature	Status
23	OpenXR Swapchain Creation	Completed
24	Swapchain Acquire / Wait / Release	Completed
25	Gazebo Camera Rendering	One-shot test completed
26	Coordinated OGRE 2 Rendering	One-shot test completed
27	OpenGL Texture Validation	Completed
28	Gazebo → OpenXR Texture Transfer	Next
29	XR Projection Layer Submission	Pending
30	Continuous Stereo Rendering	Pending
31	6DoF Head Tracking	Pending
```

Step 28 မှာ အရင်ဆုံး Framebuffer Copy ကို စမ်းသပ်မယ်။ သို့သော် Camera ရဲ့ Source Texture က 1024 × 1024 ဖြစ်ပြီး OpenXR Destination Texture က 2198 × 2302 ဖြစ်တာကြောင့် Scaling လိုပါတယ်။

အရေးကြီးတာတစ်ခုက လက်ရှိ Camera Rendering ဟာ One-shot ဖြစ်နေသေးတာပါ။ ဒါကြောင့် ဒီအဆင့်မှာ ရရှိမယ့် Image က Dynamic Simulation မဟုတ်သေးဘဲ Static Frame ဖြစ်ပါမယ်။


Step 28: OpenGL Framebuffer Blit Test
ဒီအဆင့်မှာ OpenXR ပိုင် Swapchain Textures တွေကို မထိခိုက်စေဖို့ Temporary OpenGL Texture တစ်ခုဆောက်ပြီး Gazebo Camera Image ကို Copy လုပ်နိုင်မလား အရင်စမ်းပါမယ်။

အောင်မြင်ရင် Step 29 မှာ XR Swapchain ထဲ တိုက်ရိုက် Transfer လုပ်နိုင်ပါမယ်။

28.2. Update include/XRCamera.hh

public: အောက်မှာ ဒီ Function ထည့်ပါ။
```
bool TestFramebufferBlit();
```


private: အောက်မှာ ဒီ Variable ထည့်ပါ။
```
bool blitTestAttempted_ = false;
```
```
bool XRCamera::TestFramebufferBlit()
{
    if (!stereoRenderReady_ || !textureValidationReported_)
        return false;

    if (blitTestAttempted_)
        return false;

    blitTestAttempted_ = true;

    GLint oldReadFbo = 0;
    GLint oldDrawFbo = 0;
    GLint oldTexture = 0;
    GLint oldReadBuffer = 0;
    GLint oldDrawBuffer = 0;

    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &oldReadFbo);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldDrawFbo);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);
    glGetIntegerv(GL_READ_BUFFER, &oldReadBuffer);
    glGetIntegerv(GL_DRAW_BUFFER, &oldDrawBuffer);

    GLuint readFbo = 0;
    GLuint drawFbo = 0;
    GLuint testTexture = 0;

    bool success = true;

    glGenFramebuffers(1, &readFbo);
    glGenFramebuffers(1, &drawFbo);
    glGenTextures(1, &testTexture);

    glBindTexture(GL_TEXTURE_2D, testTexture);

    constexpr GLint dstWidth = 512;
    constexpr GLint dstHeight = 512;

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        dstWidth, dstHeight, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        testTexture, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)
        != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "[XRCamera] Draw FBO incomplete\n";
        success = false;
    }

    if (success)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);

        glFramebufferTexture2D(
            GL_READ_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            leftTextureId_, 0);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER)
            != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "[XRCamera] Read FBO incomplete\n";
            success = false;
        }
    }

    if (success)
    {
        // 1024x1024 diagnostic source
        // into a 512x512 temporary texture.
        glBlitFramebuffer(
            0, 0, 1024, 1024,
            0, 0, dstWidth, dstHeight,
            GL_COLOR_BUFFER_BIT, GL_LINEAR);

        GLenum error = glGetError();

        if (error != GL_NO_ERROR)
        {
            std::cerr << "[XRCamera] Blit GL error: 0x"
                      << std::hex << error << std::dec
                      << std::endl;
            success = false;
        }
        else
        {
            glFinish(); // Diagnostic only
            std::cout
                << "[XRCamera] Framebuffer Blit SUCCESS"
                << std::endl;
        }
    }

    // Unbind test FBOs before deleting.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, oldReadFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldDrawFbo);

    glReadBuffer(oldReadBuffer);
    glDrawBuffer(oldDrawBuffer);

    glBindTexture(GL_TEXTURE_2D, oldTexture);

    glDeleteFramebuffers(1, &readFbo);
    glDeleteFramebuffers(1, &drawFbo);
    glDeleteTextures(1, &testTexture);

    return success;
}
```



28.4. Update GazeboOpenXR.cc
လက်ရှိ Camera Rendering Block အောက်မှာ ဒီ Code ထည့်ပါ။


```
if (xrCamera_.IsStereoRenderReady())
{
    if (xrCamera_.ValidateStereoTextures())
    {
        xrCamera_.TestFramebufferBlit();
    }
}
```

### Error


```
[XRCamera] Stereo Textures Validated
Segmentation fault (Address not mapped to object [(nil)])
```

2.1. Header စစ်ပါ
src/XRCamera.cc အပေါ်ဆုံးမှာ

```
#include <GL/glew.h>
#include <GL/glx.h>
```

ရှိပါစေ။ glew.h ကို OpenGL Headers တွေမတိုင်ခင် Include လုပ်ထားရပါမယ်။

2.2. GLEW Initialization Function ထည့်ပါ
src/XRCamera.cc ထဲမှာ အောက်ပါ Function ကို ထည့်ပါ။

```
bool InitializeGLFunctions()
{
    if (glXGetCurrentContext() == nullptr)
    {
        std::cerr
            << "[XRCamera] ERROR: No current GLX context\n";
        return false;
    }

    glewExperimental = GL_TRUE;

    GLenum result = glewInit();

    if (result != GLEW_OK)
    {
        std::cerr
            << "[XRCamera] GLEW initialization failed: "
            << glewGetErrorString(result)
            << std::endl;
        return false;
    }

    // GLEW may leave GL_INVALID_ENUM on core profiles.
    while (glGetError() != GL_NO_ERROR) {}

    if (!glGenFramebuffers ||
        !glBindFramebuffer ||
        !glFramebufferTexture2D ||
        !glCheckFramebufferStatus ||
        !glBlitFramebuffer ||
        !glDeleteFramebuffers)
    {
        std::cerr
            << "[XRCamera] Missing OpenGL framebuffer functions\n";
        return false;
    }

    std::cout
        << "[XRCamera] GLEW Initialized: "
        << glewGetString(GLEW_VERSION)
        << "\n[XRCamera] Framebuffer functions available"
        << std::endl;

    return true;
}
```

2.3. TestFramebufferBlit() မှာ ထည့်ပါ
Function ရဲ့ အစပိုင်းမှာရှိတဲ့

```
blitTestAttempted_ = true;
```


ရဲ့ အောက်တည့်တည့်မှာ

```
if (!InitializeGLFunctions())
{
    std::cerr
        << "[XRCamera] OpenGL function loading failed"
        << std::endl;
    return false;
}

```

3. CMakeLists.txt စစ်ပါ

```
vim CMakeLists.txt
```


find_package() တွေရှိတဲ့နေရာမှာ
```
find_package(GLEW REQUIRED)
```


ထည့်ပါ။

```
target_link_libraries(GazeboOpenXR PRIVATE ...) ထဲမှာ
GLEW::GLEW
```

static ပြင်လိုက်တာ ဘာကြောင့် အဆင်ပြေသွားလဲ?
သင်ပြင်ထားတဲ့

```
static bool InitializeGLFunctions();

static bool InitializeGLFunctions()
{
    if (glXGetCurrentContext() == nullptr)
    {
        // ...
    }
}
```