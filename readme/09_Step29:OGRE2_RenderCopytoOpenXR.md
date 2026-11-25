### Step 29 မှာ ပြင်ဆင်ရမယ့် Modules

File	လုပ်ဆောင်ချက်
XRRenderer.hh	Texture Transfer Interface
XRRenderer.cc	OpenGL FBO Copy
XRCamera.hh	Left/Right Texture Getters
XRRuntime.cc	Acquired XR Image ဆီ Copy ခေါ်ခြင်း
GazeboOpenXR.cc	Modules ချိတ်ဆက်ခြင်း

Gazebo OGRE 2 Camera က Render လုပ်ထားတဲ့ Left/Right Texture တွေကို WiVRn OpenXR Swapchain ထဲ GPU-to-GPU Copy လုပ်တဲ့အဆင့် ကို ဆက်လုပ်မယ်။

Step 28 မှာ Framebuffer Blit SUCCESS ရထားတဲ့အတွက် OpenGL Blit နည်းလမ်းကို ဆက်သုံးမယ်။ ဒါပေမဲ့ ဒီတစ်ကြိမ်မှာ Temporary Texture မဟုတ်တော့ဘဲ OpenXR Runtime က Acquire လုပ်ပေးတဲ့ Texture ထဲကို Copy လုပ်မှာပါ။

```
Gazebo Left Eye
1024 × 1024

Gazebo Right Eye
1024 × 1024


XRRenderer::BlitTexture()
OpenGL Read FBO → Draw FBO
XR Left Swapchain
2198 × 2302

XR Right Swapchain
2198 × 2302
```


OpenXR Specification အရ Swapchain Image တစ်ခုကို Acquire လုပ်ပြီး Wait အောင်မြင်မှ Write လုပ်ရပါတယ်။ Rendering Commands Submit လုပ်ပြီးမှ Release လုပ်ရပါတယ်။ 


1. include/XRRenderer.hh
File ထဲမှာ အောက်ပါ Code အပြည့်အစုံ ထည့်ပါ။

```
#pragma once

class XRRenderer
{
public:
    // Copy a Gazebo OpenGL texture into an
    // acquired OpenXR swapchain texture.
    //
    // Must be called on the rendering thread
    // with a valid, current OpenGL context.

    static bool BlitTexture(
        unsigned int sourceTexture,
        unsigned int destinationTexture,
        int sourceWidth,
        int sourceHeight,
        int destinationWidth,
        int destinationHeight);
};
```

2. src/XRRenderer.cc
အောက်ပါ Implementation ကို File တစ်ခုလုံးမှာ ထည့်ပါ။

```
#include <GL/glew.h>
#include <GL/glx.h>

#include "XRRenderer.hh"

#include <iostream>

bool XRRenderer::BlitTexture(
    unsigned int sourceTexture,
    unsigned int destinationTexture,
    int sourceWidth,
    int sourceHeight,
    int destinationWidth,
    int destinationHeight)
{
    if (!glXGetCurrentContext())
    {
        std::cerr
            << "[XRRenderer] No current GLX context\n";
        return false;
    }

    if (!sourceTexture || !destinationTexture ||
        sourceWidth <= 0 || sourceHeight <= 0 ||
        destinationWidth <= 0 || destinationHeight <= 0)
    {
        std::cerr
            << "[XRRenderer] Invalid texture or dimensions\n";
        return false;
    }

    if (glIsTexture(sourceTexture) != GL_TRUE ||
        glIsTexture(destinationTexture) != GL_TRUE)
    {
        std::cerr
            << "[XRRenderer] Texture is not valid in this GL context\n";
        return false;
    }

    // GLEW must already be initialized.
    if (!glGenFramebuffers ||
        !glBindFramebuffer ||
        !glFramebufferTexture2D ||
        !glCheckFramebufferStatus ||
        !glBlitFramebuffer ||
        !glDeleteFramebuffers)
    {
        std::cerr
            << "[XRRenderer] Missing framebuffer functions\n";
        return false;
    }

    GLint previousReadFbo = 0;
    GLint previousDrawFbo = 0;
    GLint previousReadBuffer = 0;
    GLint previousDrawBuffer = 0;

    glGetIntegerv(
        GL_READ_FRAMEBUFFER_BINDING,
        &previousReadFbo);

    glGetIntegerv(
        GL_DRAW_FRAMEBUFFER_BINDING,
        &previousDrawFbo);

    glGetIntegerv(GL_READ_BUFFER, &previousReadBuffer);
    glGetIntegerv(GL_DRAW_BUFFER, &previousDrawBuffer);

    const GLboolean scissorEnabled =
        glIsEnabled(GL_SCISSOR_TEST);

    const GLboolean srgbEnabled =
        glIsEnabled(GL_FRAMEBUFFER_SRGB);

    GLuint sourceFbo = 0;
    GLuint destinationFbo = 0;

    bool success = false;

    glGenFramebuffers(1, &sourceFbo);
    glGenFramebuffers(1, &destinationFbo);

    if (!sourceFbo || !destinationFbo)
        goto cleanup;

    // Gazebo camera texture -> READ FBO
    glBindFramebuffer(
        GL_READ_FRAMEBUFFER, sourceFbo);

    glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        sourceTexture,
        0);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(
            GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr
            << "[XRRenderer] Source FBO incomplete\n";
        goto cleanup;
    }

    // OpenXR swapchain texture -> DRAW FBO
    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER, destinationFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        destinationTexture,
        0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(
            GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr
            << "[XRRenderer] Destination FBO incomplete\n";
        goto cleanup;
    }

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_FRAMEBUFFER_SRGB);

    glBlitFramebuffer(
        0, 0, sourceWidth, sourceHeight,
        0, 0, destinationWidth, destinationHeight,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR);

    if (const GLenum error = glGetError();
        error != GL_NO_ERROR)
    {
        std::cerr
            << "[XRRenderer] glBlitFramebuffer failed: 0x"
            << std::hex << error << std::dec
            << std::endl;
        goto cleanup;
    }

    success = true;

cleanup:
    // Restore Gazebo's framebuffer bindings.
    glBindFramebuffer(
        GL_READ_FRAMEBUFFER, previousReadFbo);

    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER, previousDrawFbo);

    glReadBuffer(previousReadBuffer);
    glDrawBuffer(previousDrawBuffer);

    if (scissorEnabled)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

    if (srgbEnabled)
        glEnable(GL_FRAMEBUFFER_SRGB);
    else
        glDisable(GL_FRAMEBUFFER_SRGB);

    if (sourceFbo)
        glDeleteFramebuffers(1, &sourceFbo);

    if (destinationFbo)
        glDeleteFramebuffers(1, &destinationFbo);

    return success;
}
```

ဒီ Implementation က glBlitFramebuffer() ကို အသုံးပြုပြီး GPU ပေါ်မှာ Texture Copy လုပ်တာပါ။

အရေးကြီးတာက GL_FRAMEBUFFER_SRGB ကို ယာယီ Disable လုပ်ထားပြီး မူလ State ကို ပြန်ထားတာဖြစ်ပါတယ်။ အခု Source/Destination Format နှစ်ခုလုံး 0x8C43 ဖြစ်တဲ့ Diagnostic အတွက် သင့်တော်ပေမဲ့ Color Accuracy ကို နောက်ပိုင်း ပြန်စစ်ရပါမယ်။

3. CMakeLists.txt စစ်ဆေးခြင်း
လက်ရှိ CMake မှာ XRRenderer.cc ကို Source List ထဲ ထည့်ထားပြီးသားလား စစ်ပါ။

```
grep -n "XRRenderer" CMakeLists.txt
```

မရှိသေးရင် add_library() ထဲမှာ ထည့်ပါ။
```
add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    src/XRRuntime.cc
    src/XRCamera.cc
    src/XRRenderer.cc
    src/XRInput.cc
    ${MOC_SOURCES}
    ${QRC_SOURCES}
)
```
GLEW နဲ့ OpenGL Linkage လည်း ရှိထားပါစေ။
```
find_package(GLEW REQUIRED)
```
```
target_link_libraries(GazeboOpenXR PRIVATE
    GLEW::GLEW
)
```

4. Build စမ်းသပ်ခြင်း
အခုအဆင့်မှာ XRRenderer ကို Compile အောင်လုပ်မယ်။
```
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j8
```
Compile အောင်မြင်ရင်
```
nm -DC build/libGazeboOpenXR.so | grep "XRRenderer::BlitTexture"
```
Expected Symbol:
```
XRRenderer::BlitTexture(unsigned int, unsigned int, int, int, int, int)
```

5. နောက်ထပ် ချိတ်ဆက်မယ့်နေရာ
ဒီအဆင့်ပြီးရင် -
1. XRCamera က Left/Right Texture IDs ကိုပေးမယ်။
2. XRRuntime က Swapchain Image ကို Acquire/Wait လုပ်မယ်။
3. XRRenderer::BlitTexture() က Gazebo Texture ကို XR Texture ထဲ Copy လုပ်မယ်။
4. GPU Commands ပြီးဆုံးကြောင်း သေချာစေပြီး Swapchain Image ကို Release လုပ်မယ်။
5. xrEndFrame() နဲ့ Frame Lifecycle ကို အဆုံးသတ်မယ်။
သတိ: ယခု XRRenderer.cc က C++17 Syntax (if initializer) သုံးထားတာကြောင့် CMake မှာ အနည်းဆုံး CXX_STANDARD 17 သတ်မှတ်ထားရပါမယ်။ မရှိရင် -
```
set_target_properties(GazeboOpenXR PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
```

Step 29.5: XRRenderer ကို XRRuntime နဲ့ ချိတ်ဆက်ခြင်း
အခု XRRenderer.hh နဲ့ XRRenderer.cc ကို ရေးပြီး Build အောင်မြင်ထားတယ်လို့ယူပြီး Gazebo Camera Textures → OpenXR Swapchain Textures ကို ချိတ်ဆက်မယ်။

ဒီအဆင့်မှာ အဓိက Files သုံးဖိုင်ကို ပြင်ရပါမယ်။
File	ပြင်ဆင်မယ့်အရာ
XRCamera.hh/.cc	Left/Right Texture Getters
XRRuntime.hh/.cc	Acquire → Wait → Copy → Release
GazeboOpenXR.cc	Camera Textures ကို Runtime ဆီပေးခြင်း

2. XRCamera Texture Getters
include/XRCamera.hh ရဲ့ public: အောက်မှာ -

```
unsigned int GetLeftTextureId() const;
unsigned int GetRightTextureId() const;
```

src/XRCamera.cc မှာ -
```
unsigned int XRCamera::GetLeftTextureId() const
{
    return stereoRenderReady_ ? leftTextureId_ : 0;
}

unsigned int XRCamera::GetRightTextureId() const
{
    return stereoRenderReady_ ? rightTextureId_ : 0;
}
```

3. XRRuntime Header Update
include/XRRuntime.hh ထဲမှာ လက်ရှိ
```
bool RenderFrame();
```


ကို
```
bool RenderFrame(
    unsigned int leftTexture,
    unsigned int rightTexture);
```


လို့ ပြောင်းပါ။

private: အောက်မှာ -

```
bool CopyStereoFrame(
    unsigned int leftTexture,
    unsigned int rightTexture);
```

ထည့်ပါ။

TestSwapchainImageCycle() ကို မဖျက်သေးပါနဲ့။ Diagnostic အတွက် ဆက်ထားနိုင်ပါတယ်။ ဒါပေမဲ့ RenderFrame() မှာတော့ ထပ်မခေါ်ရပါဘူး။

4. XRRuntime.cc မှာ Texture Transfer Function ရေးခြင်း
File အပေါ်ပိုင်းမှာ

```
#include "XRRenderer.hh"
#include <GL/glew.h>
#include <iostream>
```


ထည့်ပါ။ GLEW Header က တခြား OpenGL Headers တွေထက် အရင် Include ဖြစ်ဖို့လိုပါတယ်။ လက်ရှိ OpenXR Headers နဲ့ မတိုက်မိအောင် Include Order ကိုလည်း စစ်ပါ။

ပြီးရင် CopyStereoFrame() Function ထည့်ပါ။

```
bool XRRuntime::CopyStereoFrame(
    unsigned int leftTexture,
    unsigned int rightTexture)
{
    if (!swapchainsReady_ || eyeSwapchains_.size() != 2)
    {
        status_ = "Stereo swapchains are not ready";
        return false;
    }

    const unsigned int sourceTextures[2] = {
        leftTexture,
        rightTexture
    };

    if (!sourceTextures[0] || !sourceTextures[1])
    {
        status_ = "Gazebo camera textures are not ready";
        return false;
    }

    // Current diagnostic camera resolution.
    constexpr int sourceWidth = 1024;
    constexpr int sourceHeight = 1024;

    for (size_t eye = 0; eye < 2; ++eye)
    {
        auto &swapchain = eyeSwapchains_[eye];

        XrSwapchainImageAcquireInfo acquireInfo{
            XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO
        };

        uint32_t imageIndex = 0;

        XrResult result = xrAcquireSwapchainImage(
            swapchain.handle,
            &acquireInfo,
            &imageIndex);

        if (XR_FAILED(result))
        {
            status_ = "xrAcquireSwapchainImage failed";
            return false;
        }

        XrSwapchainImageWaitInfo waitInfo{
            XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO
        };

        waitInfo.timeout = XR_INFINITE_DURATION;

        result = xrWaitSwapchainImage(
            swapchain.handle,
            &waitInfo);

        // XR_TIMEOUT_EXPIRED is a positive status code.
        // Only XR_SUCCESS means the image is ready.
        if (result != XR_SUCCESS)
        {
            status_ = "xrWaitSwapchainImage did not succeed";
            return false;
        }

        if (imageIndex >= swapchain.images.size())
        {
            status_ = "Invalid OpenXR image index";
            return false;
        }

        const unsigned int destinationTexture =
            swapchain.images[imageIndex].image;

        bool copied = XRRenderer::BlitTexture(
            sourceTextures[eye],
            destinationTexture,
            sourceWidth,
            sourceHeight,
            static_cast<int>(swapchain.width),
            static_cast<int>(swapchain.height));

        if (copied)
        {
            // Conservative GPU synchronization for testing.
            // Replace with an optimized approach later.
            glFinish();
        }

        XrSwapchainImageReleaseInfo releaseInfo{
            XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO
        };

        result = xrReleaseSwapchainImage(
            swapchain.handle,
            &releaseInfo);

        if (XR_FAILED(result))
        {
            status_ = "xrReleaseSwapchainImage failed";
            return false;
        }

        if (!copied)
        {
            status_ = "OpenGL framebuffer copy failed";
            return false;
        }

        if (frameCount_ % 120 == 0)
        {
            std::cout
                << "[XRRuntime] Eye " << eye
                << " | Source Texture: "
                << sourceTextures[eye]
                << " | XR Texture: "
                << destinationTexture
                << " | Copy SUCCESS"
                << std::endl;
        }
    }

    return true;
}
```
ဒီ Code က Eye တစ်ဖက်ချင်းစီအတွက် Swapchain Image ကို Acquire လုပ်ပြီး Wait အောင်မြင်တဲ့အခါမှ Texture ထဲ Copy လုပ်ပါတယ်။

XR_TIMEOUT_EXPIRED က XR_FAILED() အရ Failure မဟုတ်နိုင်တာကြောင့် Wait Result ကို XR_SUCCESS နဲ့ တိုက်ရိုက်စစ်ထားပါတယ်။ 
registry.khronos.org


သတိ: Wait မအောင်မြင်တဲ့ Exception Path မှာ Acquired Image ကျန်နိုင်ပါတယ်။ ဒါကြောင့် ဒီ Version ကို Diagnostic အတွက်သာ သုံးပြီး Error တက်ရင် Session ကို ရပ်ပြီး ပြန်တည်ဆောက်ရပါမယ်။ Production Version အတွက် Swapchain Ownership State နဲ့ Recovery Logic ထပ်ရေးဖို့ လိုပါတယ်။

5. XRRuntime::RenderFrame() ကို Update လုပ်ခြင်း
ရှိပြီးသား Function Definition ကို

```
bool XRRuntime::RenderFrame(
    unsigned int leftTexture,
    unsigned int rightTexture)
```


လို့ ပြောင်းပါ။

xrBeginFrame() ခေါ်ပြီးတဲ့နောက်မှာ အရင် Step 24 Diagnostic Code ဖြစ်တဲ့

```
TestSwapchainImageCycle();
```


ကို မခေါ်တော့ပါနဲ့။

အဲဒီနေရာမှာ -

```
bool copySuccess = true;

if (frameState.shouldRender &&
    swapchainsReady_ &&
    leftTexture != 0 &&
    rightTexture != 0)
{
    copySuccess = CopyStereoFrame(
        leftTexture,
        rightTexture);
}
```


ထည့်ပါ။

ရှိပြီးသား xrEndFrame() ကို ဆက်အသုံးပြုပါ။

```
endInfo.layerCount = 0;
endInfo.layers = nullptr;
```


ဒီတန်ဖိုးတွေကို Step 29 မှာ မပြောင်းသေးပါဘူး။

အရေးကြီးဆုံး: xrBeginFrame() အောင်မြင်ပြီးနောက် Copy မအောင်မြင်တာကြောင့် xrEndFrame() ကို Skip မလုပ်ပါနဲ့။ Error ကို မှတ်ထားပြီး Frame ကို End လုပ်ပြီးမှ false ပြန်ပေးပါ။

ဥပမာ ရှိပြီးသား xrEndFrame() အပြီးမှာ -
```
if (XR_FAILED(endResult))
{
    status_ = "xrEndFrame failed";
    return false;
}

if (!copySuccess)
{
    return false;
}

return true;
```


အသုံးပြုနိုင်ပါတယ်။ ဒီမှာ endResult က မူလ xrEndFrame() ရဲ့ Return Value ဖြစ်ပါတယ်။

6. GazeboOpenXR.cc Integration
လက်ရှိ OnRender() ထဲမှာ ရှိတဲ့
```
if (!runtime_.RenderFrame())
```


ကို အောက်ပါအတိုင်း အစားထိုးပါ။

```const unsigned int leftTexture =
    xrCamera_.GetLeftTextureId();

const unsigned int rightTexture =
    xrCamera_.GetRightTextureId();

if (!runtime_.RenderFrame(leftTexture, rightTexture))
{
    PostStatus(QString::fromStdString(
        runtime_.GetStatus()));

    runtime_.DestroySession();
    sessionCreated_.store(false);
    return;
}```

7. Build & Test

```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j8
```
Shared Library မှာ Missing Dependencies မရှိကြောင်း စစ်ပါ။
```
ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```

Ouptput:
```
[XRCamera] Coordinated Stereo Render SUCCESS
[XRCamera] Stereo Textures Validated
[XRCamera] Framebuffer Blit SUCCESS

[XRRuntime] Stereo Swapchains Ready

[XRRuntime] Eye 0 | Source Texture: 32 | XR Texture: 51 | Copy SUCCESS
[XRRuntime] Eye 1 | Source Texture: 43 | XR Texture: 54 | Copy SUCCESS

[XRRuntime] Frame: 120 | shouldRender: 1
[XRRuntime] Frame: 240 | shouldRender: 1
```