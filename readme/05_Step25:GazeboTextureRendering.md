## Gazebo Camera Texture Rendering


Gazebo OGRE 2 Scene
Scene Access: Ready
Left Camera
Gazebo Render Texture

Right Camera
Gazebo Render Texture


OpenGL Texture Transfer
Render / Copy into acquired XR images
OpenXR Projection Layer
Submit stereoscopic frames to WiVRn

Step 25.1: Camera Texture API စစ်ဆေးခြင်း

နောက်တစ်ဆင့်မှာ Code မပြင်ခင် Gazebo Rendering 8 ရဲ့ Camera Texture API ကို အရင်စစ်မယ်။
```
grep -R -nE \
  'RenderTextureGLId|RenderTexture|CreateRenderTexture' \
  /usr/include/gz/rendering8/gz/rendering \
  2>/dev/null | head -40
```

Rendering Engine မှာ OpenGL Texture API ဘယ်လိုဖော်ပြထားလဲကို ဒီ Command နဲ့ စစ်ပါ။

Gazebo Rendering 8 မှာ Camera Rendering အတွက် Render(), PostRender() နဲ့ RenderTextureGLId() API တွေ ရှိကြောင်း Official Documentation မှာ အတည်ပြုနိုင်ပါတယ်။ Render() မခေါ်ခင် Scene ရဲ့ PreRender() ပြီးထားဖို့ လိုပါတယ်။

25.2. include/XRCamera.hh ပြင်ခြင်း
public: အောက်မှာ Functions နှစ်ခု ထပ်ထည့်ပါ။

```
bool RenderStereoDiagnostic();

bool IsStereoRenderReady() const;
```


private: အောက်မှာ Variables ထပ်ထည့်ပါ။

```
bool stereoRenderReady_ = false;

bool renderAttempted_ = false;

unsigned int leftTextureId_ = 0;
unsigned int rightTextureId_ = 0;
```


25.3. src/XRCamera.cc ပြင်ခြင်း
အောက်ပါ Function နှစ်ခုကို File အဆုံးမှာ ထည့်ပါ။

```
bool XRCamera::RenderStereoDiagnostic()
{
    if (stereoRenderReady_)
        return true;

    if (renderAttempted_)
        return false;

    if (!IsReady() || !leftEye_ || !rightEye_)
        return false;

    // Run the diagnostic only once.
    renderAttempted_ = true;

    std::cout
        << "[XRCamera] Starting stereo render diagnostic"
        << std::endl;

    // Prepare the Gazebo scene before camera rendering.
    scene_->PreRender();

    // Render left eye.
    leftEye_->Render();
    leftEye_->PostRender();

    // Render right eye.
    rightEye_->Render();
    rightEye_->PostRender();

    // Retrieve Gazebo-owned OpenGL textures.
    leftTextureId_ = leftEye_->RenderTextureGLId();
    rightTextureId_ = rightEye_->RenderTextureGLId();

    std::cout
        << "[XRCamera] Left texture ID: "
        << leftTextureId_
        << "\n[XRCamera] Right texture ID: "
        << rightTextureId_
        << std::endl;

    if (leftTextureId_ == 0 || rightTextureId_ == 0)
    {
        std::cerr
            << "[XRCamera] Invalid camera texture"
            << std::endl;

        return false;
    }

    stereoRenderReady_ = true;

    std::cout
        << "[XRCamera] Stereo Render Textures Ready"
        << std::endl;

    return true;
}

bool XRCamera::IsStereoRenderReady() const
{
    return stereoRenderReady_;
}
```

ဒီ Code က Camera::Update() ကို မသုံးထားပါဘူး။ Gazebo Documentation မှာ Update() ကို Single-camera Scene အတွက် Convenience Function အဖြစ် သတ်မှတ်ထားပြီး Multiple Cameras အတွက် မသုံးသင့်ကြောင်း ဖော်ပြထားပါတယ်။ ဒါကြောင့် PreRender → Render → PostRender ကို အသုံးပြုထားတာပါ။ 
Gazebo+1


အရေးကြီးတဲ့ သတိချက်: လက်ရှိ Gazebo GUI Render Event ထဲမှာ scene_->PreRender() ကို ထပ်ခေါ်တာက Gazebo ရဲ့ ရှိပြီးသား Rendering Pipeline နဲ့ ထပ်နေခြင်း ဖြစ်နိုင်ပါတယ်။ ဒါကြောင့် ဒီ Code ဟာ Production-safe လို့ အတည်ပြုထားတဲ့ Integration မဟုတ်ဘဲ One-shot Diagnostic စမ်းသပ်ချက်သာ ဖြစ်ပါတယ်။ အရင် Backup ထားဖို့ အရေးကြီးပါတယ်။

25.4. GazeboOpenXR.cc ပြင်ခြင်း
လက်ရှိ OnRender() ထဲမှာ ဒီ Code ကို ထည့်ပါ။ function မၕှိပါက အောက်ပါအတိုင်းထည့်ပါ။

```
if (xrCamera_.FindScene())
{
    xrCamera_.CreateStereoCameras();
}
```

ဒီ Block ကို အောက်ပါအတိုင်း ပြောင်းပါ။

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

RenderStereoDiagnostic() ကို OnRender() ထဲမှာခေါ်ထားတာကြောင့် Render Event တက်လာမှ Execute လုပ်ပါမယ်။ Function က တစ်ကြိမ်ပဲ Rendering ကြိုးစားအောင် Guard ထားပါတယ်။

Build & Test
```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

ပြီးရင် Gazebo ကို ပြန်ဖွင့်ပါ။
```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

gz sim --force-version 8 -v 4 shapes.sdf
```

Output: 
```
[XRCamera] Gazebo Scene Ready
Rendering Engine: ogre2
Scene Name: scene
Root Visual: Available
[GazeboOpenXR] OpenXR Session Created
[GazeboOpenXR] VR Frame Loop Running
[XRCamera] Stereo Cameras Created
Left: xr_left_eye
Right: xr_right_eye
Resolution: 1024 x 1024 per eye
Diagnostic IPD: 0.064 m
[XRCamera] Starting stereo render diagnostic
[XRCamera] Left texture ID: 32
[XRCamera] Right texture ID: 43
[XRCamera] Stereo Render Textures Ready
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
gz sim gui: ./ogre2/src/Ogre2Scene.cc:211: virtual void gz::rendering::v8::Ogre2Scene::PreRender(): Assertion `("Scene::PreRender called again before calling Scene::PostRender. " "See Scene::SetCameraPassCountPerGpuFlush for details", (this->LegacyAutoGpuFlush() || this->dataPtr->frameUpdateStarted == false))' failed.
Stack trace (most recent call last) in thread 97675:
#24   Object "[0xffffffffffffffff]", at 0xffffffffffffffff, in 
#23   Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x7cd77392685f, in 
#22   Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x7cd7738949e2, in 
#21   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d4ccca0, in 
#20   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d4cbaf1, in QThread::exec()
#19   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d6b875a, in QEventLoop::exec(QFlags<QEventLoop::ProcessEventsFlag>)
#18   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d7130b7, in QEventDispatcherGlib::processEvents(QFlags<QEventLoop::ProcessEventsFlag>)
#17   Object "/lib/x86_64-linux-gnu/libglib-2.0.so.0", at 0x7cd76dd18432, in g_main_context_iteration
#16   Object "/lib/x86_64-linux-gnu/libglib-2.0.so.0", at 0x7cd76dd705f7, in 
#15   Object "/lib/x86_64-linux-gnu/libglib-2.0.so.0", at 0x7cd76dd1ad8a, in g_main_context_dispatch
#14   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d713a66, in 
#13   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d6bcf26, in QCoreApplicationPrivate::sendPostedEvents(QObject*, int, QThreadData*)
#12   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d6b9e39, in QCoreApplication::notifyInternal2(QObject*, QEvent*)
#11   Object "/lib/x86_64-linux-gnu/libQt5Widgets.so.5", at 0x7cd76cd6c712, in QApplicationPrivate::notify_helper(QObject*, QEvent*)
#10   Object "/lib/x86_64-linux-gnu/libQt5Core.so.5", at 0x7cd76d6e741d, in QObject::event(QEvent*)
#9    Object "/usr/lib/x86_64-linux-gnu/gz-gui-8/plugins/libMinimalScene.so", at 0x7cd74597d9f8, in gz::gui::plugins::RenderThread::RenderNext(gz::gui::plugins::RenderSync*)
#8    Object "/usr/lib/x86_64-linux-gnu/gz-gui-8/plugins/libMinimalScene.so", at 0x7cd745990408, in gz::gui::plugins::RenderThreadRhiOpenGL::RenderNext(gz::gui::plugins::RenderSync*)
#7    Object "/usr/lib/x86_64-linux-gnu/gz-gui-8/plugins/libMinimalScene.so", at 0x7cd745981583, in gz::gui::plugins::GzRenderer::Render(gz::gui::plugins::RenderSync*, gz::gui::plugins::RenderThreadRhi&)
#6    Object "/usr/lib/x86_64-linux-gnu/gz-rendering-8/engine-plugins/libgz-rendering-ogre2.so", at 0x7cd70f567333, in 
#5    Object "/usr/lib/x86_64-linux-gnu/gz-rendering-8/engine-plugins/libgz-rendering-ogre2.so", at 0x7cd70f60d8c0, in gz::rendering::v8::Ogre2Scene::PreRender()
#4    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x7cd773839e95, in __assert_fail
#3    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x7cd77382871a, in 
#2    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x7cd7738287f2, in abort
#1    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x7cd773842405, in raise
#0    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x7cd77389691c, in pthread_kill
Aborted (Signal sent by tkill() 97561 1000)
[Dbg] [SignalHandler.cc:142] Received signal[2].
[Dbg] [ServerPrivate.cc:122] Server received signal[2]
[Dbg] [gz.cc:434] Shutting down gz-sim-server
[Dbg] [SimulationRunner.cc:654] Exiting postupdate worker thread (0)
```

1. Error Analysis
အဓိက Error က

```
Ogre2Scene::PreRender():

Assertion:
"Scene::PreRender called again before calling
Scene::PostRender."
```

အကြောင်းရင်းက ကျွန်တော်တို့ Step 25 မှာ ရေးခဲ့တဲ့

```
scene_->PreRender();

leftEye_->Render();
leftEye_->PostRender();

rightEye_->Render();
rightEye_->PostRender();
```

ဆိုတဲ့ Code ပါ။

Gazebo ရဲ့ MinimalScene Plugin က Rendering Lifecycle ကို စီမံနေတဲ့အချိန်မှာ ကျွန်တော်တို့ GazeboOpenXR Plugin က scene_->PreRender() ကို သီးခြားထပ်ခေါ်ခဲ့ပါတယ်။

Camera::PostRender() နဲ့ Scene::PostRender() ကလည်း မတူပါဘူး။

အကျိုးဆက်အနေနဲ့ OGRE 2 ရဲ့ Internal Rendering State မကိုက်ညီတော့ဘဲ Assertion Failed ဖြစ်သွားတာပါ။

ကောင်းတဲ့အချက်က Camera Texture နှစ်ခုလုံးကို ရယူနိုင်ခဲ့ပါတယ်။

2. အခု Crash ကို ဘယ်လိုပြင်မလဲ?
Gazebo Rendering 8 ရဲ့ Official API Documentation အရ Camera Rendering ကို အောက်ပါ Lifecycle အတွင်းမှာ လုပ်ရပါတယ်။ 
Gazebo Simulation+1

```
// Correct lifecycle when the application
// owns the scene's rendering sequence.

scene->PreRender();

leftCamera->Render();
rightCamera->Render();

leftCamera->PostRender();
rightCamera->PostRender();

scene->PostRender();
```

ဒါပေမဲ့ ကျွန်တော်တို့ Plugin က Scene Rendering Lifecycle ကို ပိုင်ဆိုင်တာ မဟုတ်ပါဘူး။ Gazebo MinimalScene က ပိုင်ဆိုင်ထားတာပါ။ ဒါကြောင့် အထက်ပါ Pattern ကို OnRender() ထဲ တိုက်ရိုက်ကူးထည့်လို့ မရပါဘူး။


Step 25.8: Crash ဖြစ်စေတဲ့ Code ကို Disable လုပ်ပါ
src/GazeboOpenXR.cc ကို ဖွင့်ပါ။

```
cd ~/openxr_workspace/gazebo_openxr
vim src/GazeboOpenXR.cc
```

လက်ရှိ Camera Block ကို အောက်ပါအတိုင်း ပြောင်းပါ။

```
// STEP 25: Scene and camera discovery only.
// Do not invoke nested OGRE2 rendering here.
if (xrCamera_.FindScene())
{
    xrCamera_.CreateStereoCameras();
}
```

xrCamera_.RenderStereoDiagnostic() ကို လောလောဆယ် မခေါ်ပါနဲ့။

ဒီလိုပြောင်းလိုက်ရင် အရင်အောင်မြင်ထားတဲ့ Scene Access၊ Stereo Camera Creation၊ OpenXR Swapchain နဲ့ Frame Loop တွေကို ထိန်းထားနိုင်ပါမယ်။

```
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

gz sim --force-version 8 -v 4 shapes.sdf
```