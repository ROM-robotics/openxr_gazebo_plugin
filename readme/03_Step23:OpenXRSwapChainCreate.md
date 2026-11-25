### Step 23: OpenXR Swapchain Creation

ဒီအဆင့်မှာ အောက်ပါအလုပ်တွေ လုပ်မယ်။
- Left Eye နဲ့ Right Eye အတွက် Color Swapchain တစ်ခုစီဖန်တီးမယ်။
- WiVRn Recommended Resolution 2198 × 2302 ကို အသုံးပြုမယ်။
- Runtime ပေးတဲ့ OpenGL Texture IDs တွေကို ရယူမယ်။
- OpenXR Session ရပ်တဲ့အခါ Swapchain တွေကို စနစ်တကျ Destroy လုပ်မယ်။

```
Gazebo Harmonic + OpenXR Session
GLX / NVIDIA RTX 3060

Left Eye Swapchain

2198 × 2302
OpenGL Images

Right Eye Swapchain

2198 × 2302
OpenGL Images



WiVRn Runtime
Swapchain Management
```

ဒီအဆင့်မှာ Texture တွေကို Gazebo က Render မလုပ်သေးပါဘူး။ Swapchain Creation နဲ့ Image Enumeration ကိုသာ စမ်းသပ်မှာပါ။

### 23.2. Update include/XRRuntime.hh
အပေါ်ဆုံး Include အပိုင်းမှာ ထပ်ထည့်ပါ။

```
#include <vector>
#include <openxr/openxr.h>

#define XR_USE_GRAPHICS_API_OPENGL
#include <openxr/openxr_platform.h>
```

public: အောက်မှာ ထပ်ထည့်ပါ။

```
bool CreateColorSwapchains();

void DestroyColorSwapchains();

bool AreColorSwapchainsReady() const;
```
private: အောက်မှာ ထပ်ထည့်ပါ။
```
struct EyeSwapchain
{
    XrSwapchain handle = XR_NULL_HANDLE;

    uint32_t width = 0;
    uint32_t height = 0;

    std::vector<XrSwapchainImageOpenGLKHR> images;
};

std::vector<EyeSwapchain> eyeSwapchains_;

bool swapchainsReady_ = false;
```
### 23.3. Update src/XRRuntime.cc

File အပေါ်ဆုံး Include တွေထဲမှာ အောက်ပါ Header တွေရှိကြောင်း စစ်ပါ။

```
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>
```

ပြီးရင် File အောက်ဆုံးမှာ ဒီ Function ကို ထည့်ပါ။
```
bool XRRuntime::CreateColorSwapchains()
{
    if (swapchainsReady_)
        return true;

    if (session_ == XR_NULL_HANDLE ||
        instance_ == XR_NULL_HANDLE)
        return false;

    constexpr auto viewType =
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

    uint32_t viewCount = 0;

    XrResult result =
        xrEnumerateViewConfigurationViews(
            instance_,
            systemId_,
            viewType,
            0,
            &viewCount,
            nullptr);

    if (XR_FAILED(result) || viewCount != 2)
    {
        status_ = "Expected two stereo views";
        return false;
    }

    std::vector<XrViewConfigurationView> views(viewCount);

    for (auto &view : views)
    {
        view.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        view.next = nullptr;
    }

    result = xrEnumerateViewConfigurationViews(
        instance_,
        systemId_,
        viewType,
        viewCount,
        &viewCount,
        views.data());

    if (XR_FAILED(result))
    {
        status_ = "View configuration failed";
        return false;
    }

    uint32_t formatCount = 0;

    result = xrEnumerateSwapchainFormats(
        session_, 0, &formatCount, nullptr);

    if (XR_FAILED(result) || formatCount == 0)
    {
        status_ = "No swapchain formats available";
        return false;
    }

    std::vector<int64_t> formats(formatCount);

    result = xrEnumerateSwapchainFormats(
        session_, formatCount,
        &formatCount, formats.data());

    if (XR_FAILED(result))
    {
        status_ = "Swapchain format query failed";
        return false;
    }

    // OpenGL internal format values.
    constexpr int64_t SRGB8_ALPHA8 = 0x8C43;
    constexpr int64_t RGBA8 = 0x8058;

    auto supported = [&](int64_t format)
    {
        return std::find(
            formats.begin(),
            formats.end(),
            format) != formats.end();
    };

    int64_t selectedFormat = 0;

    if (supported(SRGB8_ALPHA8))
        selectedFormat = SRGB8_ALPHA8;
    else if (supported(RGBA8))
        selectedFormat = RGBA8;
    else
    {
        status_ = "No compatible RGBA format";
        return false;
    }

    std::cout
        << "[XRRuntime] Creating stereo swapchains"
        << std::endl;

    for (uint32_t eye = 0; eye < viewCount; ++eye)
    {
        EyeSwapchain swapchain;

        swapchain.width =
            views[eye].recommendedImageRectWidth;

        swapchain.height =
            views[eye].recommendedImageRectHeight;

        XrSwapchainCreateInfo createInfo{
            XR_TYPE_SWAPCHAIN_CREATE_INFO
        };

        createInfo.createFlags = 0;

        createInfo.usageFlags =
            XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;

        createInfo.format = selectedFormat;

        createInfo.sampleCount =
            views[eye].recommendedSwapchainSampleCount;

        createInfo.width = swapchain.width;
        createInfo.height = swapchain.height;

        createInfo.faceCount = 1;
        createInfo.arraySize = 1;
        createInfo.mipCount = 1;

        result = xrCreateSwapchain(
            session_,
            &createInfo,
            &swapchain.handle);

        if (XR_FAILED(result))
        {
            status_ = "xrCreateSwapchain failed: " +
                std::to_string(static_cast<int>(result));

            DestroyColorSwapchains();
            return false;
        }

        // Store handle immediately so cleanup can
        // release it if enumeration fails.
        eyeSwapchains_.push_back(std::move(swapchain));

        auto &created = eyeSwapchains_.back();

        uint32_t imageCount = 0;

        result = xrEnumerateSwapchainImages(
            created.handle,
            0,
            &imageCount,
            nullptr);

        if (XR_FAILED(result) || imageCount == 0)
        {
            status_ = "Swapchain image count failed";
            DestroyColorSwapchains();
            return false;
        }

        created.images.resize(imageCount);

        for (auto &image : created.images)
        {
            image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR;
            image.next = nullptr;
        }

        result = xrEnumerateSwapchainImages(
            created.handle,
            imageCount,
            &imageCount,
            reinterpret_cast<
                XrSwapchainImageBaseHeader*>(
                    created.images.data()));

        if (XR_FAILED(result))
        {
            status_ = "Swapchain image enumeration failed";
            DestroyColorSwapchains();
            return false;
        }

        std::cout
            << "[XRRuntime] Eye " << eye
            << " | Resolution: "
            << created.width << "x"
            << created.height
            << " | Image count: "
            << imageCount
            << " | Format: 0x"
            << std::hex << selectedFormat
            << std::dec << std::endl;

        for (uint32_t i = 0; i < imageCount; ++i)
        {
            std::cout
                << "  Image[" << i << "] GL Texture ID: "
                << created.images[i].image
                << std::endl;
        }
    }

    swapchainsReady_ = true;

    std::cout
        << "[XRRuntime] Stereo Swapchains Ready"
        << std::endl;

    return true;
}
```
ဒီ Implementation က xrCreateSwapchain() နဲ့ xrEnumerateSwapchainImages() ကို အသုံးပြုပါတယ်။ OpenGL Runtime ကပေးတဲ့ Image တွေမှာ OpenGL Texture Handle ပါဝင်ပါတယ်။

23.4. Swapchain Cleanup ထည့်ပါ
src/XRRuntime.cc ထဲမှာ ဒီ Function ကို ထပ်ထည့်ပါ။

```
void XRRuntime::DestroyColorSwapchains()
{
    for (auto &eye : eyeSwapchains_)
    {
        if (eye.handle != XR_NULL_HANDLE)
        {
            xrDestroySwapchain(eye.handle);
            eye.handle = XR_NULL_HANDLE;
        }

        eye.images.clear();
    }

    eyeSwapchains_.clear();
    swapchainsReady_ = false;
}

bool XRRuntime::AreColorSwapchainsReady() const
{
    return swapchainsReady_;
}
```
ပြီးရင် ရှိပြီးသား DestroySession() Function ရဲ့ အစဆုံးမှာ ဒီစာကြောင်းကို ထည့်ပါ။

```
DestroyColorSwapchains();
```
```
Cleanup Order က အောက်ပါအတိုင်း ဖြစ်ရပါမယ်။
Destroy Swapchains
       ↓
Destroy Reference Space
       ↓
Destroy OpenXR Session
       ↓
Destroy OpenXR Instance (Shutdown only)
```
အရေးကြီးတာက Swapchain Texture တွေကို glDeleteTextures() နဲ့ ကိုယ်တိုင် မဖျက်ရပါဘူး။ Texture Ownership က OpenXR Runtime မှာ ရှိပါတယ်။

### 23.5. Update GazeboOpenXR.cc
OnRender() ထဲက if (runtime_.IsSessionRunning()) Block မှာ အောက်ပါအစီအစဉ်အတိုင်း ထားပါ။

```
if (runtime_.IsSessionRunning())
{
    if (!wasRunning)
    {
        PostStatus("VR Frame Loop Running");
    }

    runtime_.InspectViewConfiguration();

    // Swapchain creation happens before frame rendering.
    if (!runtime_.AreColorSwapchainsReady())
    {
        if (!runtime_.CreateColorSwapchains())
        {
            PostStatus(QString::fromStdString(
                runtime_.GetStatus()));

            // Stop cleanly rather than retrying
            // swapchain creation every render event.
            runtime_.RequestStop();
            return;
        }
    }

    if (!runtime_.RenderFrame())
    {
        PostStatus(QString::fromStdString(
            runtime_.GetStatus()));

        runtime_.DestroySession();
        sessionCreated_.store(false);
    }
}
```

ဒီမှာ xrWaitFrame() မခေါ်ခင် Swapchain Creation လုပ်ထားပါတယ်။

လက်ရှိ RenderFrame() မှာ Composition Layers မထည့်ရသေးလို့ Frame Loop အလုပ်လုပ်နေပေမဲ့ Quest ထဲ Gazebo Environment ပေါ်လာဦးမှာ မဟုတ်ပါဘူး။

Thread Safety: Swapchain Create/Destroy တွေကို လက်ရှိ Session ဖန်တီးခဲ့တဲ့ GLX Render Context နဲ့ သက်ဆိုင်တဲ့ Thread မှာပဲ လုပ်ပါ။ XR_KHR_opengl_enable က OpenGL Context Ownership အတွက် သတ်မှတ်ချက်တွေ ရှိပါတယ်။

23.6. Build & Test
CMakeLists.txt ကို ပြင်စရာ မလိုသေးပါဘူး။ Code က XRRuntime.cc ထဲမှာပဲ ရှိနေတာကြောင့်ပါ။

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

```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```
