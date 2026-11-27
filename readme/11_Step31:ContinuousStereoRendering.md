လက်ရှိ အောင်မြင်ထားတဲ့ Architecture
Gazebo Harmonic / OGRE 2
Stereo Camera Rendering
OpenGL Framebuffer Transfer
GPU-to-GPU Texture Copy
OpenXR Stereo Swapchains
Left / Right Eye Images
WiVRn Runtime
Stereo Projection Layer Composition
Meta Quest 3S
Stereo Image Display Working

Continuous Stereo Rendering
အခုက One-shot Camera Rendering ဖြစ်နေသေးလို့ Gazebo Scene ပြောင်းလဲရင် Quest ထဲက Image ကို အလိုအလျောက် Update မလုပ်သေးပါဘူး။

Step 31: Continuous Stereo Camera Rendering ကို တည်ဆောက်ရပါမယ်။

အဲဒီအဆင့်မှာ အဓိကလုပ်ရမှာ -
1. XRCamera::RenderStereoDiagnostic() ကို One-shot ကနေ Frame တိုင်း Update လုပ်နိုင်တဲ့ Rendering Function အဖြစ် ပြောင်းခြင်း။
2. Gazebo MinimalScene ရဲ့ Render Lifecycle နဲ့ မှန်ကန်စွာ Synchronize လုပ်ခြင်း။
3. Gazebo Texture အသစ်တွေကို OpenXR Swapchain ထဲ Frame တိုင်း Copy လုပ်ခြင်း။
4. xrWaitFrame() နဲ့ GPU Rendering Timing ကို ညှိခြင်း။
အထူးသတိ: အရင် Step 25 မှာ Scene::PreRender() Assertion ဖြစ်ခဲ့တဲ့အတွက် Render() ကို Frame တိုင်း ခေါ်ရုံနဲ့ မလုံလောက်ပါဘူး။ Scene::PreRender() → Camera Render() → Camera PostRender() → Scene::PostRender() Lifecycle ကို သေချာထိန်းသိမ်းရပါမယ်။

31.2. Rendering Strategy
Continuous Rendering ကို တန်းပြီး 72 FPS သို့မဟုတ် 90 FPS မလုပ်သေးပါဘူး။

ပထမဆုံး 10 FPS Diagnostic Mode နဲ့ စမ်းမယ်။
Setting	Value
Gazebo Stereo Rendering	10 FPS target
OpenXR Frame Loop	Runtime-controlled
Source Resolution	1024 × 1024 per eye
XR Resolution	2198 × 2302 per eye
Head Tracking Integration	Not yet
Camera Position	Existing diagnostic positions

10 FPS က Debugging အတွက်သာ ဖြစ်ပါတယ်။ Continuous Rendering တည်ငြိမ်လာတဲ့အခါ 30 FPS နဲ့အထက် တဖြည်းဖြည်း တိုးမယ်။

31.3. include/XRCamera.hh ပြင်ပါ
File ရဲ့ အပေါ်ပိုင်းမှာ -
```
#include <chrono>
#include <cstdint>
```

XRCamera Class ရဲ့ public: အောက်မှာ -

```
bool UpdateStereoFrame();
```


ထည့်ပါ။

private: အောက်မှာ -

```
std::chrono::steady_clock::time_point lastStereoRender_{};

std::uint64_t stereoFrameCount_ = 0;
```


ထည့်ပါ။

ရှိပြီးသား RenderStereoDiagnostic(), IsStereoRenderReady() နဲ့ Texture Getters တွေကို မဖျက်ပါနဲ့။

31.4. src/XRCamera.cc မှာ Continuous Renderer ထည့်ပါ
ရှိပြီးသား RenderStereoDiagnostic() ကို မပြောင်းပါနဲ့။ အောက်ပါ Function အသစ်ကို ထပ်ထည့်ပါ။

```
bool XRCamera::UpdateStereoFrame()
{
    if (!stereoRenderReady_ ||
        !scene_ ||
        !leftEye_ ||
        !rightEye_)
    {
        return false;
    }

    using Clock = std::chrono::steady_clock;

    const auto now = Clock::now();

    // Diagnostic: limit stereo camera rendering to 10 FPS.
    constexpr auto renderInterval =
        std::chrono::milliseconds(100);

    if (lastStereoRender_ != Clock::time_point{} &&
        now - lastStereoRender_ < renderInterval)
    {
        // No new rendering needed.
        // Previously rendered textures remain available.
        return true;
    }

    // Must execute on Gazebo's rendering thread.
    //
    // Start a complete, balanced OGRE2 scene cycle.
    scene_->PreRender();

    // Render both eyes within one scene cycle.
    leftEye_->Render();
    rightEye_->Render();

    // Finalize both camera render passes.
    leftEye_->PostRender();
    rightEye_->PostRender();

    // Required to finish the scene cycle.
    scene_->PostRender();

    // Fetch the current camera texture handles.
    const unsigned int leftId =
        leftEye_->RenderTextureGLId();

    const unsigned int rightId =
        rightEye_->RenderTextureGLId();

    if (leftId == 0 || rightId == 0)
    {
        std::cerr
            << "[XRCamera] Stereo texture unavailable"
            << std::endl;

        return false;
    }

    leftTextureId_ = leftId;
    rightTextureId_ = rightId;

    lastStereoRender_ = Clock::now();
    ++stereoFrameCount_;

    if (stereoFrameCount_ % 60 == 0)
    {
        std::cout
            << "[XRCamera] Continuous Stereo Frame: "
            << stereoFrameCount_
            << " | Left: " << leftTextureId_
            << " | Right: " << rightTextureId_
            << std::endl;
    }

    return true;
}
```

ဒီ Function မှာ အရေးကြီးတဲ့ Design သုံးခု ပါပါတယ်။
1. scene_->PreRender() နဲ့ scene_->PostRender() ကို တစ်စုံတစ်တွဲတည်း ခေါ်ထားပါတယ်။
2. Left/Right Camera နှစ်လုံးကို Scene Cycle တစ်ခုတည်းအတွင်း Render လုပ်ပါတယ်။
3. Render ပြီးတိုင်း လက်ရှိ Texture IDs ကို ပြန်ရယူထားပါတယ်။
တစ်ခု သတိထားရမှာက Scene::PostRender() ဟာ OGRE 2 Frame Finalization အတွက် အရေးကြီးတာကြောင့် အဲဒီ Function မရောက်ခင် Early Return မလုပ်ထားပါဘူး။

ဒီ Integration က Step 26 မှာ One-shot အောင်မြင်ခဲ့တဲ့ Pattern ကို ထပ်ခါတလဲလဲ အသုံးပြုထားတာဖြစ်ပြီး Continuous Operation အတွက် စမ်းသပ်ရဦးမှာပါ။

31.5. GazeboOpenXR.cc မှာ Update Loop ချိတ်ပါ
လက်ရှိ OnRender() ထဲက XRCamera Block ကို ရှာပါ။

အရင် Step တွေမှာ ဒီလိုမျိုး ရေးထားပါတယ်။
```
if (xrCamera_.FindScene())
{
    if (xrCamera_.CreateStereoCameras())
    {
        if (!xrCamera_.IsStereoRenderReady())
        {
            xrCamera_.RenderStereoDiagnostic();
        }
    }
}
```

ဒီ Block ကို အောက်ပါ Code နဲ့ အစားထိုးပါ။
// STEP 31: Continuous Gazebo stereo rendering

```
if (xrCamera_.FindScene())
{
    if (xrCamera_.CreateStereoCameras())
    {
        // First frame: initialize camera render targets.
        if (!xrCamera_.IsStereoRenderReady())
        {
            xrCamera_.RenderStereoDiagnostic();
        }
        else
        {
            // Subsequent frames: update both stereo cameras.
            if (!xrCamera_.UpdateStereoFrame())
            {
                std::cerr
                    << "[GazeboOpenXR] Stereo frame update failed"
                    << std::endl;
            }
        }
    }
}
```

ဒီ Code ကို Swapchain Creation နဲ့ runtime_.RenderFrame(leftTexture, rightTexture) မခေါ်ခင် ထားပါ။

အရေးကြီးတဲ့အချက်က RenderStereoDiagnostic() နဲ့ UpdateStereoFrame() ကို Render Event တစ်ခုတည်းမှာ တစ်ပြိုင်တည်း မခေါ်တာပါ။
လက်ရှိ OnRender() အစီအစဉ်

```
// 1. Handle start / stop requests.

// 2. Poll OpenXR events.

// 3. Confirm OpenXR session is running.

// 4. Inspect view configuration.

// 5. Find Gazebo scene and initialize stereo cameras.

// 6. Render/update stereo camera textures.

// 7. Ensure OpenXR swapchains exist.

// 8. Read current camera texture IDs.

// 9. XRRuntime::RenderFrame(leftTexture, rightTexture).
```


XRRuntime::RenderFrame() ထဲမှာ Step 30 ရဲ့ xrLocateViews()၊ CopyStereoFrame() နဲ့ Projection Layer Submission တို့ကို ဆက်သုံးပါ။

ဒါမှသာ Gazebo က အသစ် Render လုပ်ထားတဲ့ Camera Texture ကို XR Swapchain ထဲ Copy လုပ်နိုင်မှာပါ။


31.6. Build

```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j8
```

Compilation Error မရှိရင် -
```
ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```
စစ်ပါ။
31.7. Runtime Test
```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
  "$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```

Quest 3S နဲ့ WiVRn ချိတ်ပြီး VR Session ကို Start လုပ်ပါ။

Expected Logs
```
[XRCamera] Coordinated Stereo Render SUCCESS
[XRRuntime] Stereo Swapchains Ready

[XRCamera] Continuous Stereo Frame: 60
[XRCamera] Continuous Stereo Frame: 120
[XRCamera] Continuous Stereo Frame: 180

[XRRuntime] Eye 0 | ... | Copy SUCCESS
[XRRuntime] Eye 1 | ... | Copy SUCCESS
[XRRuntime] Submitted layers: 1
```


ဒီ Logs တွေက မျှော်မှန်းထားတဲ့ Output ဖြစ်ပါတယ်။ လက်တွေ့ရလဒ်ကို စစ်ရပါမယ်။

Actual Output

```
[XRCamera] Gazebo Scene Ready
Rendering Engine: ogre2
Scene Name: scene
Root Visual: Available
[XRCamera] Stereo Cameras Created
Left: xr_left_eye
Right: xr_right_eye
Resolution: 1024 x 1024 per eye
Diagnostic IPD: 0.064 m
[XRCamera] Starting coordinated stereo render
[XRCamera] Left texture ID: 31
[XRCamera] Right texture ID: 43
[XRCamera] Coordinated Stereo Render SUCCESS
[XRRuntime] Creating stereo swapchains
[XRRuntime] Eye 0 | Resolution: 2198x2302 | Image count: 3 | Format: 0x8c43
  Image[0] GL Texture ID: 49
  Image[1] GL Texture ID: 50
  Image[2] GL Texture ID: 51
[XRRuntime] Eye 1 | Resolution: 2198x2302 | Image count: 3 | Format: 0x8c43
  Image[0] GL Texture ID: 52
  Image[1] GL Texture ID: 53
  Image[2] GL Texture ID: 54
[XRRuntime] Stereo Swapchains Ready
[GazeboOpenXR] OpenXR Session Created
[GazeboOpenXR] VR Frame Loop Running
[XRRuntime] Session state: 3
[XRRuntime] Session state: 4
[XRRuntime] Session state: 5
[XRRenderer] Missing framebuffer functions
[GazeboOpenXR] OpenGL framebuffer copy failed

Exception sending a multicast message:Network is unreachable
```

```[XRRenderer] Missing framebuffer functions
[GazeboOpenXR] OpenGL framebuffer copy failed
```

1. Root Cause Analysis
အရင် Step 28 မှာ XRCamera.cc ထဲက InitializeGLFunctions() ကို အသုံးပြုပြီး glewInit() လုပ်ခဲ့ပါတယ်။

ဒါပေမဲ့ လက်ရှိ Step 31 Log မှာ -
[XRCamera] Coordinated Stereo Render SUCCESS
[XRRuntime] Stereo Swapchains Ready
[XRRenderer] Missing framebuffer functions


ဖြစ်နေပါတယ်။

ဖြစ်နိုင်ခြေအများဆုံးအကြောင်းရင်း: XRRenderer::BlitTexture() မခေါ်ခင် လက်ရှိ OpenGL Context အတွက် GLEW Initialization မလုပ်ရသေးခြင်းပါ။

အရင် Step 28 မှာ TestFramebufferBlit() ကို ခေါ်ခဲ့လို့ GLEW Initialize ဖြစ်ခဲ့ပေမဲ့ အခု Run မှာ အဲဒီ Function ကို မခေါ်တော့တာ ဖြစ်နိုင်ပါတယ်။

static bool InitializeGLFunctions() ကလည်း XRCamera.cc File အတွင်းမှာသာ အသုံးပြုနိုင်တဲ့ Helper ဖြစ်တဲ့အတွက် XRRenderer.cc ထဲကနေ တိုက်ရိုက်ခေါ်လို့ မရပါဘူး။
2. Step 31.1: XRRenderer.cc ကို ပြင်ဆင်ပါ
လက်ရှိ File ရဲ့ အပေါ်ဆုံးမှာ ဒီ Headers တွေရှိပါစေ။

```
#include <GL/glew.h>
#include <GL/glx.h>

#include "XRRenderer.hh"

#include <iostream>
```

အဲဒီနောက် XRRenderer::BlitTexture() မတိုင်ခင် အောက်ပါ Helper Function ထည့်ပါ။

```
namespace
{
bool EnsureGLEWInitialized()
{
    const GLXContext currentContext =
        glXGetCurrentContext();

    if (!currentContext)
    {
        std::cerr
            << "[XRRenderer] No current GLX context\n";
        return false;
    }

    // Diagnostic: track the last initialized GL context
    // on this rendering thread.
    static thread_local GLXContext initializedContext =
        nullptr;

    if (initializedContext == currentContext)
        return true;

    glewExperimental = GL_TRUE;

    const GLenum result = glewInit();

    if (result != GLEW_OK)
    {
        std::cerr
            << "[XRRenderer] GLEW initialization failed: "
            << reinterpret_cast<const char*>(
                   glewGetErrorString(result))
            << std::endl;

        return false;
    }

    // Clear a possible GLEW initialization error.
    (void)glGetError();

    if (!glGenFramebuffers ||
        !glBindFramebuffer ||
        !glFramebufferTexture2D ||
        !glCheckFramebufferStatus ||
        !glBlitFramebuffer ||
        !glDeleteFramebuffers)
    {
        std::cerr
            << "[XRRenderer] Required FBO functions missing\n";

        return false;
    }

    initializedContext = currentContext;

    std::cout
        << "[XRRenderer] GLEW Initialized: "
        << reinterpret_cast<const char*>(
               glewGetString(GLEW_VERSION))
        << std::endl;

    std::cout
        << "[XRRenderer] OpenGL FBO functions ready"
        << std::endl;

    return true;
}
}
```


ဒီ Code က လက်ရှိ Rendering Thread ရဲ့ GLX Context ကို စစ်ပြီး GLEW Initialize လုပ်ပါတယ်။

ဒါက Diagnostic Fix ဖြစ်ပါတယ်။ Production မှာ GL Context Recreation နဲ့ Multi-context GLEW State ကို ပိုပြီး စနစ်တကျ ကိုင်တွယ်ရပါမယ်။
3. BlitTexture() Function ကို Update လုပ်ပါ
src/XRRenderer.cc ထဲက -

```
bool XRRenderer::BlitTexture(
    unsigned int sourceTexture,
    unsigned int destinationTexture,
    int sourceWidth,
    int sourceHeight,
    int destinationWidth,
    int destinationHeight)
{
    ```


ရဲ့ အောက်မှာ အောက်ပါ Code ကို ထည့်ပါ။

```
    if (!EnsureGLEWInitialized())
    {
        std::cerr
            << "[XRRenderer] OpenGL initialization failed\n";
        return false;
    }
```

ပြီးရင် ကျန်တဲ့ BlitTexture() Code ကို မပြောင်းပါနဲ့။