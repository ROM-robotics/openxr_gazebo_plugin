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

