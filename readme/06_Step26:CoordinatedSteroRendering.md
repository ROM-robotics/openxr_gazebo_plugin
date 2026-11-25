Q&A : 
```
Gazebo GUI Rendering Integration နဲ့ Dedicated XR Render Worker နှစ်ခုစလုံးဟာ Gazebo simulation scene ကို XR (VR/AR) headset ပေါ်မှာ render လုပ်ဖို့ အသုံးပြုနိုင်တဲ့ architecture တွေ ဖြစ်ပေမယ့် thread model, render loop ownership, နဲ့ OGRE 2 rendering resources ကို ဘယ်လိုစီမံခန့်ခွဲလဲ ဆိုတဲ့ အချက်တွေမှာ အခြေခံကျကျ ကွာခြားပါတယ်။၁။ Gazebo GUI Rendering Integrationဒီချဉ်းကပ်နည်းမှာ Gazebo ရဲ့ လက်ရှိ GUI / rendering plug-in infrastructure ထဲကနေ scene data နဲ့ camera/view mechanism တွေကို ချိတ်ဆက်ပြီး XR output အဖြစ် ထုတ်ပေးတာ ဖြစ်ပါတယ်။အလုပ်လုပ်ပုံ: Gazebo GUI ရဲ့ rendering pipeline နဲ့ MinimalScene ကို အခြေခံပြီး XR view/camera render pass တွေကို ထည့်သွင်းခြင်း သို့မဟုတ် GUI thread/render cycle နဲ့ ချိတ်ဆက် run ခြင်း ဖြစ်ပါတယ်။အားသာချက်များ:Existing Scene & GUI Reuse: Gazebo ထဲမှာ ရှိပြီးသား MinimalScene၊ lighting၊ visual entities၊ GUI plugins နဲ့ tooling တွေကို အစကနေ ပြန်မတည်ဆောက်ဘဲ ပြန်အသုံးပြုနိုင်ပါတယ်။Development Speed: Prototype လုပ်တာနဲ့ Gazebo ရဲ့ native GUI tools တွေနဲ့ အတူတူ တွဲသုံးရတာ ပိုမြန်ပြီး ရိုးရှင်းပါတယ်။သတိထားရန် / စိန်ခေါ်မှုများ:MinimalScene Lifecycle Synchronization: MinimalScene ထဲမှာ entities/visuals တွေ ဖန်တီးတာ၊ update လုပ်တာ၊ ဖျက်တာ (add/update/remove) တွေနဲ့ XR render pass တွေ lifecycle မကိုက်ညီရင် dangling pointer၊ race condition၊ သို့မဟုတ် crash တွေ ဖြစ်နိုင်ပါတယ်။GUI Frame Rate & Thread Coupling: GUI render loop နဲ့ ချိတ်ဆက်ထားတဲ့အတွက် GUI frame drop ဖြစ်တာ၊ physics/simulation RTF (Real-Time Factor) ကျတာ သို့မဟုတ် main/GUI thread မှာ heavy workload တက်တာတွေက XR frame smooth ဖြစ်မှုကို ထိခိုက်စေနိုင်ပါတယ်။၂။ Dedicated XR Render Workerဒီချဉ်းကပ်နည်းမှာ XR rendering အတွက် သီးခြား render worker thread / context တစ်ခု ဖန်တီးပြီး OpenXR/XR runtime ရဲ့ frame timing နဲ့ တိုက်ရိုက်ညှိနှိုင်းပြီး render လုပ်တာ ဖြစ်ပါတယ်။အလုပ်လုပ်ပုံ: XR worker thread က XR runtime (ဥပမာ OpenXR) ကနေ predicted display pose နဲ့ frame timing ကို တောင်းယူပြီး၊ stereo/eye views တွေအတွက် scene state ကို ဖတ်ရှုကာ သီးခြား render queue / context နဲ့ swapchain ကို submit လုပ်ပါတယ်။အားသာချက်များ:Independent XR Frame Timing: XR မှာ motion-to-photon latency နဲ့ frame pacing (ဥပမာ 72/90/120 Hz) အလွန်အရေးကြီးပါတယ်။ Dedicated worker ကြောင့် Gazebo GUI framerate နဲ့ မသက်ဆိုင်ဘဲ XR frame timing ကို ပိုပြီး ထိန်းချုပ်နိုင်ပါတယ်။Isolation from GUI Bottlenecks: GUI widget updates သို့မဟုတ် desktop rendering spikes တွေကြောင့် XR headset view ကို တိုက်ရိုက် ထစ်ငေါ့စေတာကို လျှော့ချနိုင်ပါတယ်။သတိထားရန် / စိန်ခေါ်မှုများ:OGRE 2 Context & Thread Safety: OGRE 2 (OgreNext) render resources၊ GPU buffers၊ textures၊ materials နဲ့ GL/Vulkan context တွေဟာ thread-affinity သို့မဟုတ် specific context rules တွေ ရှိတတ်ပါတယ်။ Thread ခွဲသုံးတဲ့အခါ context sharing နဲ့ synchronization ကို မှန်ကန်စွာ မစီမံရင် driver error သို့မဟုတ် memory corruption ဖြစ်နိုင်ပါတယ်။Scene Ownership & State Transfer: Simulation/scene state ကို ဘယ် thread ကပိုင်ဆိုင်လဲ (ownership) နဲ့ worker thread က snapshot/double-buffer သို့မဟုတ် mutex/command queue ဘယ်လိုသုံးပြီး safely ဖတ်မလဲ ဆိုတာကို ရှင်းရှင်းလင်းလင်း ဒီဇိုင်းဆွဲရပါတယ်။နှိုင်းယှဉ်ချက် အကျဉ်းနှိုင်းယှဉ်ချက်Gazebo GUI Rendering IntegrationDedicated XR Render WorkerRender Loop / TimingGazebo GUI / scene render cycle နဲ့ ပိုနီးစပ်XR runtime frame timing နဲ့ သီးခြားစီမံReuse of Existing Stackလက်ရှိ GUI နဲ့ MinimalScene ကို တိုက်ရိုက်ပြန်သုံးရန် လွယ်Scene sync / bridge layer ကို ပိုပြီး သီးခြားတည်ဆောက်ရLatency & Frame PacingGUI/main thread load ပေါ် မူတည်ပြီး ထိခိုက်လွယ်နိုင်XR frame pacing ကို ပိုမိုသီးခြား ထိန်းချုပ်နိုင်အဓိက နည်းပညာ RiskMinimalScene lifecycle & entity sync timingOGRE 2 context management & cross-thread scene ownershipဘယ်ဟာ ရွေးသင့်လဲ:လက်ရှိ Gazebo GUI/scene workflow နဲ့ အမြန်ချိတ်ဆက်ပြီး visualize လုပ်ချင်ရင် Gazebo GUI Rendering Integration က စတင်ရန် ပိုလွယ်ပါတယ်။XR frame timing၊ low-latency head tracking၊ stereo rendering stability နဲ့ thread isolation ကို ဦးစားပေးမယ်ဆိုရင် Dedicated XR Render Worker က ပိုသင့်တော်ပြီး၊ cross-thread state transfer နဲ့ OGRE 2 resource lifecycle ကို သေချာတည်ဆောက်ရပါမယ်။
```

Workflow
```
Gazebo MinimalScene
Owns OGRE 2 rendering lifecycle
Coordinated Stereo Rendering
Left/Right Camera Render Passes
Gazebo OpenGL Textures
Camera output + GPU synchronization
OpenXR Swapchain Images
Acquire → Wait → Copy → Release
OpenXR Projection Layer
xrEndFrame() → WiVRn → Meta Quest 3S
```

Step 26.2: XRCamera.cc ပြင်ဆင်ခြင်း
လက်ရှိ Function ဖြစ်တဲ့
```
bool XRCamera::RenderStereoDiagnostic()
```

ကို အောက်ပါ Code နဲ့ တစ်ခုလုံး အစားထိုးပါ။

```
bool XRCamera::RenderStereoDiagnostic()
{
    if (stereoRenderReady_)
        return true;

    if (renderAttempted_)
        return false;

    if (!IsReady() || !leftEye_ || !rightEye_)
        return false;

    // Only one diagnostic attempt per scene.
    renderAttempted_ = true;

    std::cout
        << "[XRCamera] Starting coordinated stereo render"
        << std::endl;

    // This method must run in the Gazebo rendering
    // thread after MinimalScene's main camera Update().
    //
    // It starts a NEW, complete scene rendering cycle.

    scene_->PreRender();

    // Render both stereo cameras inside the same
    // scene rendering cycle.
    leftEye_->Render();
    rightEye_->Render();

    // Finalize both camera render passes.
    leftEye_->PostRender();
    rightEye_->PostRender();

    // CRITICAL:
    // Complete the scene rendering cycle.
    // The previous implementation missed this call.
    scene_->PostRender();

    // Obtain Gazebo-owned OpenGL texture handles.
    leftTextureId_ =
        leftEye_->RenderTextureGLId();

    rightTextureId_ =
        rightEye_->RenderTextureGLId();

    std::cout
        << "[XRCamera] Left texture ID: "
        << leftTextureId_
        << "\n[XRCamera] Right texture ID: "
        << rightTextureId_
        << std::endl;

    if (leftTextureId_ == 0 ||
        rightTextureId_ == 0)
    {
        std::cerr
            << "[XRCamera] Invalid stereo render texture"
            << std::endl;

        return false;
    }

    stereoRenderReady_ = true;

    std::cout
        << "[XRCamera] Coordinated Stereo Render SUCCESS"
        << std::endl;

    return true;
}
```

အခုက Camera နှစ်လုံးအတွက် Scene Render Cycle တစ်ခုတည်းကို အသုံးပြုထားပါတယ်။

5. Step 26.4: Rebuild
Gazebo GUI ကို အရင်ပိတ်ထားပါ။

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

6. Step 26.5: Runtime Test

```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```

Output:

```[GazeboOpenXR]   0x8cad
VR Frame Loop Running
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
[XRCamera] Left texture ID: 40
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
[XRRuntime] Session state: 3
[XRRuntime] Session state: 4
[XRRuntime] Session state: 5
[XRRuntime] Frame: 120 | shouldRender: 1 | predictedDisplayTime: 3282429978728
[XRRuntime] Eye 0 acquired image 2 | Texture ID: 51
[XRRuntime] Eye 1 acquired image 2 | Texture ID: 54
[XRRuntime] Frame: 240 | shouldRender: 1 | predictedDisplayTime: 3284430020686
[XRRuntime] Eye 0 acquired image 2 | Texture ID: 51
[XRRuntime] Eye 1 acquired image 2 | Texture ID: 54
[XRRuntime] Frame: 360 | shouldRender: 1 | predictedDisplayTime: 3286430387966
[XRRuntime] Eye 0 acquired image 2 | Texture ID: 51
[XRRuntime] Eye 1 acquired image 2 | Texture ID: 54
[XRRuntime] Frame: 480 | shouldRender: 1 | predictedDisplayTime: 3288441108568
[XRRuntime] Eye 0 acquired image 2 | Texture ID: 51
[XRRuntime] Eye 1 acquired image 2 | Texture ID: 54
```