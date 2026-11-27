

1. OpenXR Rendering Pipeline
Gazebo OGRE 2 Scene
Left / Right Camera Textures
XR Swapchain
Acquire → Wait → Copy → Release
xrLocateViews()
Retrieve predicted eye poses and FOV
XrCompositionLayerProjection
Two XrCompositionLayerProjectionView objects
xrEndFrame()
Submit Stereo Layer → WiVRn → Quest 3S

Step 30 ရဲ့ Scope: Static Stereo Image ကို Quest ထဲ Render လုပ်ကြည့်မှာပါ။ 
Camera Pose ကို Head Tracking နဲ့ မချိတ်သေးတဲ့အတွက် Head လှည့်တဲ့အခါ 3D World မှန်မှန်ကန်ကန် လိုက်မရွေ့သေးပါဘူး။

3. XRRuntime.hh ပြင်ခြင်း

private: အောက်မှာ Function Declaration ထည့်ပါ။
```
bool LocateStereoViews(
    XrTime displayTime,
    XrView (&views)[2]);
```
လက်ရှိ Project မှာ OpenXR Reference Space တည်ဆောက်ထားပြီးသားဖြစ်ရပါမယ်။ ယခင် Implementation ထဲမှာ Reference Space Handle ကို referenceSpace_ လို့ ခေါ်ထားတယ်လို့ ယူဆထားပါတယ်။

အဲဒီ Handle ဟာ XR_NULL_HANDLE မဟုတ်ကြောင်း စစ်ဖို့လိုပါတယ်။ တခြားနာမည်နဲ့ သိမ်းထားရင် အောက်က Code မှာလည်း အဲဒီနာမည်ကို အသုံးပြုပါ။

4. XRRuntime.cc မှာ LocateStereoViews() ထည့်ခြင်း

```
bool XRRuntime::LocateStereoViews(
    XrTime displayTime,
    XrView (&views)[2])
{
    if (session_ == XR_NULL_HANDLE ||
        referenceSpace_ == XR_NULL_HANDLE)
    {
        status_ = "Session or reference space not ready";
        return false;
    }

    for (auto &view : views)
    {
        view = {XR_TYPE_VIEW};
    }

    XrViewLocateInfo locateInfo{
        XR_TYPE_VIEW_LOCATE_INFO
    };

    locateInfo.viewConfigurationType =
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

    locateInfo.displayTime = displayTime;
    locateInfo.space = referenceSpace_;

    XrViewState viewState{
        XR_TYPE_VIEW_STATE
    };

    uint32_t viewCount = 0;

    XrResult result = xrLocateViews(
        session_,
        &locateInfo,
        &viewState,
        2,
        &viewCount,
        views);

    if (XR_FAILED(result))
    {
        status_ = "xrLocateViews failed";
        return false;
    }

    if (viewCount != 2)
    {
        status_ = "Unexpected OpenXR view count";
        return false;
    }

    constexpr XrViewStateFlags requiredFlags =
        XR_VIEW_STATE_POSITION_VALID_BIT |
        XR_VIEW_STATE_ORIENTATION_VALID_BIT;

    if ((viewState.viewStateFlags & requiredFlags)
        != requiredFlags)
    {
        // Tracking is not valid for this display time.
        // Skip the projection layer for this frame.
        return false;
    }

    if (frameCount_ % 120 == 0)
    {
        std::cout
            << "[XRRuntime] Stereo Views Located"
            << std::endl;

        for (uint32_t eye = 0; eye < 2; ++eye)
        {
            const auto &p = views[eye].pose.position;

            std::cout
                << "[XRRuntime] View " << eye
                << " | Position: "
                << p.x << ", "
                << p.y << ", "
                << p.z
                << std::endl;
        }
    }

    return true;
}
```

XrViewStateFlags ကို စစ်ထားတာက Runtime က ပြန်ပေးတဲ့ Eye Pose အချက်အလက်တွေ Valid ဖြစ်မဖြစ် စစ်ဆေးဖို့ပါ။ Tracking မမှန်တဲ့ Frame မှာ Projection Layer ကို မ Submit လုပ်သင့်ပါဘူး။

5. XRRuntime::RenderFrame() မှာ Projection Layer ထည့်ခြင်း
ဒီနေရာမှာ အရင် Step 29 က CopyStereoFrame() ကို ဆက်သုံးပါမယ်။

RenderFrame() ရဲ့ xrBeginFrame() အောင်မြင်ပြီးတဲ့နောက်၊ မူလ xrEndFrame() မခေါ်မီ အပိုင်းကို ပြင်ပါ။

အောက်ပါ Code မှာ frameState ဟာ လက်ရှိ xrWaitFrame() က ရရှိထားတဲ့ XrFrameState ဖြစ်ပါတယ်။

```
// STEP 30: Prepare stereo projection layer.
//
// This block belongs after a successful xrBeginFrame()
// and before the existing xrEndFrame().

XrView xrViews[2] = {
    {XR_TYPE_VIEW},
    {XR_TYPE_VIEW}
};

XrCompositionLayerProjectionView projectionViews[2] = {
    {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW},
    {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW}
};

XrCompositionLayerProjection projectionLayer{
    XR_TYPE_COMPOSITION_LAYER_PROJECTION
};

const XrCompositionLayerBaseHeader* layers[1] = {
    nullptr
};

uint32_t layerCount = 0;

bool copySuccess = true;

if (frameState.shouldRender && swapchainsReady_)
{
    const bool haveSourceTextures =
        leftTexture != 0 &&
        rightTexture != 0;

    bool viewsValid = false;

    if (haveSourceTextures)
    {
        viewsValid = LocateStereoViews(
            frameState.predictedDisplayTime,
            xrViews);
    }

    if (haveSourceTextures && viewsValid)
    {
        copySuccess = CopyStereoFrame(
            leftTexture,
            rightTexture);

        if (copySuccess)
        {
            for (uint32_t eye = 0; eye < 2; ++eye)
            {
                projectionViews[eye].pose =
                    xrViews[eye].pose;

                projectionViews[eye].fov =
                    xrViews[eye].fov;

                projectionViews[eye].subImage.swapchain =
                    eyeSwapchains_[eye].handle;

                projectionViews[eye].subImage.imageRect.offset =
                    {0, 0};

                projectionViews[eye].subImage.imageRect.extent = {
                    static_cast<int32_t>(
                        eyeSwapchains_[eye].width),
                    static_cast<int32_t>(
                        eyeSwapchains_[eye].height)
                };

                projectionViews[eye].subImage.imageArrayIndex = 0;
            }

            projectionLayer.space = referenceSpace_;
            projectionLayer.viewCount = 2;
            projectionLayer.views = projectionViews;

            layers[0] =
                reinterpret_cast<
                    const XrCompositionLayerBaseHeader*>(
                        &projectionLayer);

            layerCount = 1;
        }
    }
}

// Keep your original XrFrameEndInfo object.
// Replace its old layerCount=0 and layers=nullptr
// assignments with:
endInfo.layerCount = layerCount;
endInfo.layers = layerCount ? layers : nullptr;
```

အရေးကြီးသောအချက်များ
- CopyStereoFrame() ထဲက Acquire → Wait → Copy → Release ကို ပြန်မရေးပါနဲ့။
- TestSwapchainImageCycle() ကို Frame တစ်ခုတည်းမှာ ထပ်မခေါ်ပါနဲ့။
- projectionViews[eye].pose နဲ့ .fov ကို xrLocateViews() ရလဒ်ကနေ ယူထားပါတယ်။
- imageArrayIndex = 0 က လက်ရှိ Eye တစ်ဖက်စီမှာ သီးခြား Swapchain တစ်ခု အသုံးပြုထားတဲ့ Design အတွက် မှန်ပါတယ်။
- layerCount = 1 က Eye တစ်ဖက်တည်း ကို မဆိုလိုပါဘူး။ Projection Layer တစ်ခုအတွင်း Eye Views နှစ်ခု ပါဝင်ပါတယ်။
xrEndFrame() က Projection Layer ထဲမှာ သတ်မှတ်ထားတဲ့ Swapchain တွေရဲ့ နောက်ဆုံး Release လုပ်ထားတဲ့ Images ကို Composition အတွက် အသုံးပြုပါတယ်။

6. xrEndFrame() စစ်ဆေးခြင်း
ရှိပြီးသား endInfo မှာ displayTime နဲ့ environmentBlendMode ကို ဆက်လက်သတ်မှတ်ထားရပါမယ်။

```
endInfo.displayTime = frameState.predictedDisplayTime;

// For an opaque immersive VR session.
// Must be supported by the runtime.
endInfo.environmentBlendMode =
    XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
```

WiVRn က OPAQUE ကို Support လုပ်ကြောင်း အတည်ပြုထားဖို့ လိုပါတယ်။ xrEnumerateEnvironmentBlendModes() ကနေ ရရှိတဲ့ Supported Modes ထဲမှာ မပါရင် ဒီတန်ဖိုးကို မသုံးရပါဘူး။

ပြီးရင် ရှိပြီးသား xrEndFrame() ကို တစ်ကြိမ်သာ ခေါ်ပါ။

```
XrResult endResult = xrEndFrame(
    session_,
    &endInfo);

if (XR_FAILED(endResult))
{
    std::cerr
        << "[XRRuntime] xrEndFrame failed: "
        << endResult
        << std::endl;

    status_ = "xrEndFrame failed";
    return false;
}

if (!copySuccess)
{
    return false;
}```

ရှိပြီးသား Frame Counter Update နဲ့ အခြား Cleanup Logic ကို မဖျက်ပါနဲ့။

Debug Logging ထည့်ခြင်း
xrEndFrame() အောင်မြင်ပြီးနောက်မှာ -
```
if (frameCount_ % 120 == 0)
{
    std::cout
        << "[XRRuntime] Submitted layers: "
        << layerCount
        << std::endl;
}
```
```
Expected Output:
[XRRuntime] Stereo Views Located
[XRRuntime] Eye 0 | Source Texture: ... | XR Texture: ... | Copy SUCCESS
[XRRuntime] Eye 1 | Source Texture: ... | XR Texture: ... | Copy SUCCESS
[XRRuntime] Submitted layers: 1
```

```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j8
```

```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
  "$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```

WiVRn Server နဲ့ Meta Quest 3S ချိတ်ပြီး Start VR Session နှိပ်ပါ။
Step 30 ရဲ့ စမ်းသပ်ရမယ့် Result
xrLocateViews() က Valid Eye Poses နှစ်ခု ပြန်ပေးခြင်း


Left/Right Swapchain Texture Copy နှစ်ခုလုံး အောင်မြင်ခြင်း

Submitted layers: 1 Log ထွက်ခြင်း


Quest 3S ထဲတွင် Stereo Image ပေါ်လာခြင်း

Output:
```
[XRRuntime] Submitted layers: 1
[XRRuntime] Stereo Views Located
[XRRuntime] View 0 | Position: -2.26372, -0.407369, 1.56407
[XRRuntime] View 1 | Position: -2.32602, -0.399751, 1.57138
[XRRuntime] Eye 0 | Source Texture: 38 | XR Texture: 51 | Copy SUCCESS
[XRRuntime] Eye 1 | Source Texture: 41 | XR Texture: 54 | Copy SUCCESS
```

8. လက်ရှိ Implementation ရဲ့ Limitation
Step 30 မှာ OpenXR View Pose ကို Projection Layer အတွက်သာ သုံးထားပါတယ်။ Gazebo Camera ကို Headset Pose နဲ့ မချိတ်ရသေးပါဘူး။

ဒါကြောင့် ပုံပေါ်လာရင်တောင် -
- Scene က Static ဖြစ်နိုင်ပါတယ်။
- Headset လှည့်လျှင် Camera View မှန်ကန်စွာ မပြောင်းသေးပါဘူး။
- လက်ရှိ ±0.032 m Camera Offset က Headset ရဲ့ Actual Eye Poses ကို မလိုက်သေးပါဘူး။
- Gazebo Camera ရဲ့ Projection Matrix/FOV နဲ့ OpenXR Projection Layer FOV ကို မကိုက်ညီသေးလို့ Stereo Depth နဲ့ Distortion ပြဿနာ ရှိနိုင်ပါတယ်။
ဒီအဆင့်ရဲ့ အဓိကရည်ရွယ်ချက်က Gazebo က ထုတ်ပေးတဲ့ Stereo Texture ကို WiVRn Composer ထဲ အမှန်တကယ် Submit လုပ်နိုင်ကြောင်း စစ်ဆေးခြင်း ဖြစ်ပါတယ်။
9. Development Roadmap
Step	Feature	Status
27	OpenGL Texture Validation	Completed
28	Framebuffer Blit	Completed
29	XR Swapchain Texture Copy	Assumed ready, verification needed
30	Stereo Projection Layer Submission	Ready to test
31	Continuous Stereo Camera Rendering	Pending
32	OpenXR Head Tracking → Gazebo Camera Pose	Pending
33	OpenXR FOV / Projection Matrix Matching	Pending
34	VR Frame Timing & GPU Optimization	Pending

Step 30 Result Analysis
Component	Result
xrLocateViews()	Success
Left Eye Pose	Received
Right Eye Pose	Received
Left Texture Copy	Success
Right Texture Copy	Success
Projection Layer Submission	1
Meta Quest Stereo Display	Working

OpenXR က Eye Position နှစ်ခုကိုလည်း ပြန်ပေးထားပါတယ်။
Left  : (-2.26372, -0.407369, 1.56407)
Right : (-2.32602, -0.399751, 1.57138)

Eye နှစ်ဖက်ကြား Distance က ခန့်မှန်းခြေ 0.0632 m (63.2 mm) ဖြစ်ပြီး Stereo Eye Separation နဲ့ ကိုက်ညီပါတယ်။


