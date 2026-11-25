24.2. Update include/XRRuntime.hh
public: အောက်မှာ ထည့်ပါ။
```
bool TestSwapchainImageCycle();
```

24.3. Update src/XRRuntime.cc
File အောက်ဆုံးမှာ ဒီ Function ကို ထည့်ပါ။
```
bool XRRuntime::TestSwapchainImageCycle()
{
    if (!swapchainsReady_ || eyeSwapchains_.size() != 2)
        return false;

    for (size_t eye = 0; eye < eyeSwapchains_.size(); ++eye)
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
            status_ = "Acquire failed for eye " +
                std::to_string(eye);
            return false;
        }

        XrSwapchainImageWaitInfo waitInfo{
            XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO
        };

        waitInfo.timeout = XR_INFINITE_DURATION;

        result = xrWaitSwapchainImage(
            swapchain.handle, &waitInfo);

        if (XR_FAILED(result))
        {
            status_ = "Wait failed for eye " +
                std::to_string(eye);

            // A failed wait requires recovery before
            // the acquired image can be safely reused.
            return false;
        }

        const auto texture =
            swapchain.images[imageIndex].image;

        // Rendering into this texture comes later.

        if (frameCount_ % 120 == 0)
        {
            std::cout
                << "[XRRuntime] Eye " << eye
                << " acquired image " << imageIndex
                << " | Texture ID: " << texture
                << std::endl;
        }

        XrSwapchainImageReleaseInfo releaseInfo{
            XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO
        };

        result = xrReleaseSwapchainImage(
            swapchain.handle, &releaseInfo);

        if (XR_FAILED(result))
        {
            status_ = "Release failed for eye " +
                std::to_string(eye);
            return false;
        }
    }

    return true;
}

```

24.4. Update RenderFrame()
XRRuntime::RenderFrame() Function ထဲမှာ xrBeginFrame() အောင်မြင်ပြီးနောက်၊ 
XrFrameEndInfo မတည်ဆောက်ခင် အောက်ပါ Code ကို ထည့်ပါ။
```
if (frameState.shouldRender && swapchainsReady_)
{
    if (!TestSwapchainImageCycle())
    {
        std::cerr
            << "[XRRuntime] Swapchain image cycle failed: "
            << status_
            << std::endl;
    }
}
```
ဒီနေရာမှာ Diagnostic Failure ဖြစ်ရင် Log ထုတ်ပြီး xrEndFrame() ကို Layer မပါဘဲ ဆက်ခေါ်နိုင်အောင် ရေးထားပါတယ်။ Failure ဖြစ်နေတဲ့အခြေအနေကို အကြိမ်ကြိမ် Retry မလုပ်ဘဲ Session Restart လုပ်ပြီး Diagnose လုပ်သင့်ပါတယ်။

သတိ: RenderFrame() ထဲမှာရှိတဲ့ endInfo.layerCount = 0 ကို လောလောဆယ် မပြောင်းပါနဲ့။ Texture Data မထည့်ရသေးပါဘူး။

24.5. Rebuild
cd ~/openxr_workspace/gazebo_openxr

```
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

24.6. Run Gazebo
export GZ_GUI_PLUGIN_PATH="$PWD/build"

```
export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```

expected output

```
[XRRuntime] Creating stereo swapchains
[XRRuntime] Eye 0 | Resolution: 2198x2302 | Image count: 3 | Format: 0x8c43
  Image[0] GL Texture ID: 34
  Image[1] GL Texture ID: 35
  Image[2] GL Texture ID: 36
[XRRuntime] Eye 1 | Resolution: 2198x2302 | Image count: 3 | Format: 0x8c43
  Image[0] GL Texture ID: 37
  Image[1] GL Texture ID: 38
  Image[2] GL Texture ID: 39
[XRRuntime] Stereo Swapchains Ready
[GazeboOpenXR] OpenXR Session Created
[GazeboOpenXR] VR Frame Loop Running
[XRRuntime] Session state: 3
[XRRuntime] Session state: 4
[XRRuntime] Session state: 5

XRRuntime] Frame: 25440 | shouldRender: 1 | predictedDisplayTime: 3271613260762
[XRRuntime] Eye 0 acquired image 0 | Texture ID: 34
[XRRuntime] Eye 1 acquired image 0 | Texture ID: 37
[XRRuntime] Frame: 25560 | shouldRender: 1 | predictedDisplayTime: 3273613192215
[XRRuntime] Eye 0 acquired image 0 | Texture ID: 34
[XRRuntime] Eye 1 acquired image 0 | Texture ID: 37
```