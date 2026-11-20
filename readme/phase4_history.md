### Step 20. Gazebo Scene Access
ဒီအဆင့်မှာ Gazebo GUI ရဲ့ 3D Scene ကို gz-rendering8 API နဲ့ ရယူပြီး OpenXR Plugin ကနေ အသုံးပြုနိုင်ကြောင်း စမ်းသပ်မယ်။
Gazebo Scene
OGRE 2
XRCamera
Scene Access


OpenXR
Swapchain (Next)
Quest 3S
Stereo Display (Next)


Gazebo Harmonic 8 မှာ Client-side Rendering Scene ကို Render Engine ကနေ ရယူနိုင်ပြီး Render Event အတွင်း Rendering API တွေကို အသုံးပြုနိုင်ပါတယ်။

```
vim include/XRCamera.hh
```

```
#pragma once

#include <gz/rendering/Scene.hh>
#include <gz/rendering/RenderTypes.hh>

class XRCamera
{
public:
    bool FindScene();

    bool IsReady() const;

private:
    gz::rendering::ScenePtr scene_;

    bool reported_ = false;
};
```

### 20.3. Create XRCamera Source

```
vim src/XRCamera.cc
```

```#include "XRCamera.hh"

#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/Visual.hh>

#include <iostream>

bool XRCamera::FindScene()
{
    if (scene_ && scene_->IsInitialized() &&
        scene_->RootVisual())
    {
        return true;
    }

    scene_.reset();

    // Reuse the existing Gazebo rendering scene.
    // Do not create a separate rendering engine.
    auto engines = gz::rendering::loadedEngines();

    if (engines.empty())
        return false;

    if (engines.size() != 1)
        return false;

    auto *engine = gz::rendering::engine(engines[0]);

    if (!engine || engine->SceneCount() != 1)
        return false;

    auto candidate = engine->SceneByIndex(0);

    if (!candidate || !candidate->IsInitialized() ||
        !candidate->RootVisual())
    {
        return false;
    }

    scene_ = candidate;

    if (!reported_)
    {
        std::cout
            << "[XRCamera] Gazebo Scene Ready\n"
            << "Rendering Engine: "
            << engines[0]
            << "\nScene Name: "
            << scene_->Name()
            << "\nRoot Visual: Available"
            << std::endl;

        reported_ = true;
    }

    return true;
}

bool XRCamera::IsReady() const
{
    return scene_ &&
           scene_->IsInitialized() &&
           scene_->RootVisual() != nullptr;
}
```

20.4. Update GazeboOpenXR.hh

```
vim include/GazeboOpenXR.hh
```

အပေါ်ဆုံး Include အပိုင်းမှာ ထပ်ထည့်ပါ။

```
#include "XRCamera.hh"
```

Class ရဲ့ private: အောက်မှာ ထည့်ပါ။
```
XRCamera xrCamera_;
```

### 20.5 Update GazeboOpenXR.cc

လက်ရှိ OnRender() Function ရဲ့ အောက်ဆုံးနားမှာ ရှိတဲ့

```
if (runtime_.IsSessionRunning())
{
```

ဆိုတဲ့ Block ကိုရှာပါ။

အဲဒီ Block ထဲမှာ runtime_.RenderFrame() မခေါ်ခင် အောက်ပါ Code ကို ထည့်ပါ။

```
// Execute inside Gazebo's render event.
xrCamera_.FindScene();
```
### 20.6. Update CMakeLists.txt
find_package() အပိုင်းမှာ ထပ်ထည့်ပါ။

```
find_package(gz-rendering8 REQUIRED)
```

```
add_library() အပိုင်းကို ပြင်ပါ။
add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    src/XRRuntime.cc
    src/XRCamera.cc
    ${MOC_SOURCES}
    ${QRC_SOURCES}
)
```


target_link_libraries() ထဲမှာ ထပ်ထည့်ပါ။

```
gz-rendering8::gz-rendering8
```


ကျန်တဲ့ CMake Configuration ကို မပြောင်းပါနဲ့။

### 20.7 Rebuild

```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```
Dependency နဲ့ Symbol Error စစ်ပါ။

```
ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```

Output မရှိရင် Unresolved Symbol မတွေ့တာဖြစ်ပါတယ်။

### 20.8. Test Gazebo Scene Access

WiVRn Server နဲ့ Meta Quest ကို ချိတ်ဆက်ထားပြီး Gazebo ကို Run ပါ။

```
cd ~/openxr_workspace/gazebo_openxr

export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```

Plugin ကို ဖွင့်ပြီး Start VR Session ကို နှိပ်ပါ။

မျှော်လင့်ထားတဲ့ Log က အောက်ပါပုံစံဖြစ်ပါတယ်။

```
[GazeboOpenXR] Runtime: WiVRn 'v26.9'
System: Meta Quest 3 on WiVRn
OpenGL Extension: Supported
Minimum OpenGL: 3.0
Maximum Tested OpenGL: 4.6
OpenXR Runtime Ready
[GazeboOpenXR] Requesting OpenXR session...
[XRRuntime] GLX Context Ready
OpenGL Vendor: NVIDIA Corporation
OpenGL Renderer: NVIDIA GeForce RTX 3060/PCIe/SSE2
OpenGL Version: 4.6.0 NVIDIA 580.178.04
[XRRuntime] OpenXR Session Created Successfully
[XRRuntime] Session state: 1
[XRRuntime] Session state: 2
[XRRuntime] Session started
[GazeboOpenXR] OpenXR Session Created
[XRCamera] Gazebo Scene Ready
Rendering Engine: ogre2
Scene Name: scene
Root Visual: Available
[GazeboOpenXR] VR Frame Loop Running
[XRRuntime] Session state: 3
[XRRuntime] Session state: 4
[XRRuntime] Session state: 5
[XRRuntime] Frame: 120 | shouldRender: 1 | predictedDisplayTime: 4711673937324
[XRRuntime] Frame: 240 | shouldRender: 1 | predictedDisplayTime: 4713673937304
[XRRuntime] Frame: 360 | shouldRender: 1 | predictedDisplayTime: 4715673937284


```

ဒီအဆင့်မှာ Quest ထဲ Gazebo World မပေါ်သေးပါဘူး။ Scene Pointer ရရှိတာကိုသာ အတည်ပြုနေတာဖြစ်ပါတယ်။

Gazebo Harmonic ရဲ့ OGRE 2 Rendering Scene ကို OpenXR Plugin ကနေ ရယူနိုင်ပြီဖြစ်ပါတယ်။

```
1. Log Analysis
Component	Result
WiVRn v26.9	Success
Meta Quest 3S Connection	Success
OpenGL 4.6 / NVIDIA RTX 3060	Success
xrCreateSession()	Success
xrBeginSession()	Success
XR_SESSION_STATE_FOCUSED (5)	Reached
Gazebo OGRE 2 Scene	Success
Root Visual	Available
XR Frame Loop	Running
shouldRender	1
Left / Right Eye Cameras	Not created yet
XR Swapchain	Not created yet
```
shouldRender: 1 ဆိုတာ Runtime က Frame Rendering လုပ်နိုင်တဲ့ အခြေအနေဖြစ်ကြောင်း ဖော်ပြတာပါ။ အမှန်တကယ် Gazebo Scene ကို Quest ထဲမှာ Render လုပ်ပြီးပြီလို့ မဆိုလိုသေးပါဘူး။

Frame Log တွေအရ 120 frames ကို ခန့်မှန်း 2 စက္ကန့်အတွင်း Advance ဖြစ်နေတာကြောင့် Diagnostic Loop က 60 FPS ဝန်းကျင်နဲ့ တိုးနေပါတယ်။ ဒါက ခန့်မှန်းချက်ဖြစ်ပြီး VR Display FPS Benchmark မဟုတ်ပါဘူး။

### Step 21: Stereo Camera Creation

ဒီအဆင့်မှာ Camera နှစ်လုံးကို ဖန်တီးပြီး Scene Graph ထဲမှာ Register လုပ်မယ်။ Texture Rendering, Head Tracking, OpenXR Swapchain မပါသေးပါဘူး။ တစ်ဆင့်ချင်း စစ်သွားတာက GPU Rendering ပြဿနာတွေကို ရှာရလွယ်စေပါတယ်။

### 21.2. Update include/XRCamera.hh
ရှိပြီးသား File ကို အောက်ပါ Code နဲ့ အစားထိုးပါ။

```
#pragma once

#include <gz/rendering/Scene.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderTypes.hh>

class XRCamera
{
public:
    bool FindScene();
    bool IsReady() const;

    bool CreateStereoCameras();

private:
    gz::rendering::ScenePtr scene_;

    gz::rendering::CameraPtr leftEye_;
    gz::rendering::CameraPtr rightEye_;

    bool reported_ = false;
    bool stereoReported_ = false;
};
```

### 21.3. Update src/XRCamera.cc

ရှိပြီးသား FindScene() နဲ့ IsReady() Function တွေကို မဖျက်ပါနဲ့။

File အပေါ်ပိုင်းမှာ Include တွေ ထည့်ပါ။

```
#include <gz/math/Pose3.hh>
#include <gz/math/Angle.hh>
```

ပြီးရင် File အောက်ဆုံးမှာ ဒီ Function ကို ထည့်ပါ။

```
bool XRCamera::CreateStereoCameras()
{
    if (!IsReady())
        return false;

    if (leftEye_ && rightEye_)
        return true;

    // Diagnostic resolution only.
    // Replace with OpenXR recommended sizes later.
    constexpr unsigned int width = 1024;
    constexpr unsigned int height = 1024;

    // Temporary diagnostic IPD (64 mm).
    constexpr double halfIPD = 0.032;

    auto createEye =
        [&](const std::string &name, double y)
        -> gz::rendering::CameraPtr
    {
        auto camera = scene_->CreateCamera(name);

        if (!camera)
            return nullptr;

        camera->SetImageWidth(width);
        camera->SetImageHeight(height);
        camera->SetAspectRatio(
            static_cast<double>(width) / height);

        camera->SetHFOV(gz::math::Angle(1.57));
        camera->SetNearClipPlane(0.05);
        camera->SetFarClipPlane(1000.0);

        camera->SetLocalPose(
            gz::math::Pose3d(0, y, 1.6, 0, 0, 0));

        scene_->RootVisual()->AddChild(camera);

        return camera;
    };

    if (!leftEye_)
        leftEye_ = createEye("xr_left_eye", halfIPD);

    if (!rightEye_)
        rightEye_ = createEye("xr_right_eye", -halfIPD);

    if (!leftEye_ || !rightEye_)
    {
        std::cerr
            << "[XRCamera] Stereo camera creation failed"
            << std::endl;

        return false;
    }

    if (!stereoReported_)
    {
        std::cout
            << "[XRCamera] Stereo Cameras Created\n"
            << "Left: " << leftEye_->Name() << "\n"
            << "Right: " << rightEye_->Name() << "\n"
            << "Resolution: "
            << width << " x " << height
            << " per eye\n"
            << "Diagnostic IPD: 0.064 m"
            << std::endl;

        stereoReported_ = true;
    }

    return true;
}
```

Gazebo Rendering 8 မှာ CreateCamera(), SetImageWidth(), SetImageHeight(), SetHFOV() နဲ့ Camera ကို Root Visual မှာ Attach လုပ်နိုင်တဲ့ API တွေ ပါဝင်ပါတယ်။ 
gazebosim.org+2


မှတ်ချက်: 64 mm IPD နဲ့ 1024 × 1024 ဟာ ယာယီ Diagnostic Values တွေဖြစ်ပါတယ်။ Quest ရဲ့ လက်တွေ့ Eye Pose နဲ့ Resolution ကို ကိုယ်စားမပြုပါဘူး။ Camera Pose တွေကိုလည်း Gazebo Coordinate System ထဲမှာ ယာယီ သတ်မှတ်ထားတာပါ။

### 21.4. Update GazeboOpenXR.cc
OnRender() ထဲက

```
xrCamera_.FindScene();
```

ဆိုတဲ့ စာကြောင်းကို အောက်ပါအတိုင်း ပြောင်းပါ။

```
if (xrCamera_.FindScene())
{
    xrCamera_.CreateStereoCameras();
}
```

ဒါက Render Event တစ်ခုချင်းစီမှာ Camera ထပ်မဖန်တီးဘဲ ရှိပြီးသား Camera နှစ်လုံးကို Reuse လုပ်ပါမယ်။

Camera တွေကို OpenXR Session ရပ်တိုင်း မဖျက်သေးပါဘူး။ ဒီ Diagnostic Version မှာ Gazebo Scene တစ်ခုတည်းကို အသုံးပြုနေတယ်လို့ ယူဆထားပါတယ်။ World ပြောင်းခြင်းနဲ့ Plugin Unload လုပ်ခြင်းအတွက် Resource Cleanup ကို နောက်အဆင့်မှာ ထည့်ရပါမယ်။

### 21.5. Rebuild
CMakeLists.txt ကို ထပ်ပြင်ဖို့ မလိုပါဘူး။ XRCamera.cc ကို Step 20 မှာ ထည့်ထားပြီးသား ဖြစ်ပါတယ်။

```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

Dependencies စစ်ပါ။

```
ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```

### 21.6. Launch Gazebo

```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```
WiVRn နဲ့ Meta Quest 3S ချိတ်ထားပြီး Start VR Session ကို နှိပ်ပါ။

### Step 22 : OpenXR View Configuration & Swapchain Preparation

22.2. Update include/XRRuntime.hh

```
public: အောက်မှာ ထည့်ပါ။
bool InspectViewConfiguration();
```


private: အောက်မှာ ထည့်ပါ။

```
bool viewConfigurationInspected_ = false;
```
### 22.3. Update src/XRRuntime.cc

File အပေါ်ပိုင်းမှာ <iomanip> ကို ထည့်ပါ။
```
#include <iomanip>
```

File အောက်ဆုံးမှာ ဒီ Function ကို ထပ်ထည့်ပါ။

```
bool XRRuntime::InspectViewConfiguration()
{
    if (instance_ == XR_NULL_HANDLE ||
        systemId_ == XR_NULL_SYSTEM_ID ||
        session_ == XR_NULL_HANDLE)
        return false;

    if (viewConfigurationInspected_)
        return true;

    constexpr XrViewConfigurationType viewType =
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

    uint32_t count = 0;

    XrResult result = xrEnumerateViewConfigurationViews(
        instance_, systemId_, viewType,
        0, &count, nullptr);

    if (XR_FAILED(result) || count == 0)
    {
        status_ = "View configuration enumeration failed";
        return false;
    }

    std::vector<XrViewConfigurationView> views(count);

    for (auto &view : views)
        view.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;

    result = xrEnumerateViewConfigurationViews(
        instance_, systemId_, viewType,
        count, &count, views.data());

    if (XR_FAILED(result))
    {
        status_ = "Unable to read view configurations";
        return false;
    }

    std::cout
        << "[XRRuntime] View count: " << count
        << std::endl;

    for (uint32_t i = 0; i < count; ++i)
    {
        const auto &view = views[i];

        std::cout
            << "[XRRuntime] Eye " << i
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

    uint32_t formatCount = 0;

    result = xrEnumerateSwapchainFormats(
        session_, 0, &formatCount, nullptr);

    if (XR_FAILED(result) || formatCount == 0)
    {
        status_ = "Swapchain format enumeration failed";
        return false;
    }

    std::vector<int64_t> formats(formatCount);

    result = xrEnumerateSwapchainFormats(
        session_, formatCount,
        &formatCount, formats.data());

    if (XR_FAILED(result))
    {
        status_ = "Unable to read swapchain formats";
        return false;
    }

    std::cout
        << "[XRRuntime] Supported OpenGL formats:"
        << std::endl;

    for (auto format : formats)
    {
        std::cout
            << "  0x"
            << std::hex << format << std::dec
            << std::endl;
    }

    viewConfigurationInspected_ = true;

    return true;
}

```

### 22.4 DestroySession() Update

ရှိပြီးသား DestroySession() Function ထဲက Cleanup အဆုံးပိုင်းမှာ အောက်ပါစာကြောင်းကို ထပ်ထည့်ပါ။

```
viewConfigurationInspected_ = false;
```

### 22.5. GazeboOpenXR.cc Update
OnRender() ထဲမှာ ဒီ Block ကို ရှာပါ။

```
if (runtime_.IsSessionRunning())
{
```
### 22.6. Rebuild and Test

```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```
```
ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```
WiVRn Server နဲ့ Quest 3S ချိတ်ပြီး Gazebo ကို ပြန်ဖွင့်ပါ။

```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```

