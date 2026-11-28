# $\color{#00c853}{\text{Gazebo Harmonic project စတင်ခြင်း}}$

Gazebo Harmonic project တည်ဆောက်ခြင်း၊ OpenXR plugin ရေးသားခြင်းနှင့် session စမ်းသပ်ခြင်း <br>
ဒီ project က Gazebo Harmonic ရဲ့ GUI ထဲမှာ OpenXR plugin တစ်ခုထည့်ပြီး OpenXR runtime နဲ့ VR session ချိတ်ဆက်မှု စမ်းသပ်ဖို့ ရည်ရွယ်ထားတာပါ။

Check OpenXR Runtime — runtime ကို initialize လုပ်ပြီး runtime နဲ့ headset/system အချက်အလက်ကို စစ်တယ်။
Start VR Session — Gazebo ရဲ့ လက်ရှိ OpenGL context ကိုယူပြီး OpenXR session စတင်တယ်။ Session event တွေကိုစစ်ကာ frame loop လည်ပတ်စေတယ်။
Stop VR Session — OpenXR session ကို ရပ်ဖို့ တောင်းဆိုတယ်။
အရေးကြီးတာက ဒီအဆင့်မှာ Gazebo scene ကို headset ထဲမှာ မပြသေးပါဘူး။ XRRuntime.cc က frame များကို OpenXR ထံတင်ပို့ပေမယ့် composition layer မပို့သေးတာကြောင့် stereo image, head tracking, controller input တို့ မပါသေးပါ။ အခုအခြေအနေကို “VR rendering” ထက် OpenXR session/frame-loop အခြေခံ စမ်းသပ်မှု လို့ နားလည်နိုင်ပါတယ်။

## $\color{red}{\text{၁။ Gazebo dependency}}$


```bash
sudo apt update

sudo apt install -y \
    libgz-sim8-dev \
    libgz-gui8-dev \
    libgz-rendering8-dev \
    libgz-transport13-dev \
    libgz-msgs10-dev \
    libgz-common5-dev \
    libgz-math7-dev \
    libgz-plugin2-dev \
    libsdformat14-dev \
    libopenxr-dev \
    libopenxr-loader1 \
    libglew-dev \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtquickcontrols2-5-dev
```


```bash
pkg-config --list-all | grep -E 'gz-|openxr|sdformat'
```


```bash
pkg-config --modversion gz-sim8
pkg-config --modversion gz-gui8
pkg-config --modversion gz-rendering8
pkg-config --modversion gz-transport13
pkg-config --modversion gz-msgs10
pkg-config --modversion openxr
```
Desktop Output
```text
8.15.0
8.4.0
8.3.0
13.6.0
10.4.0
1.0
```
Laptop Output
```bash
8.15.0
8.5.0
8.3.0
13.7.0
10.4.0
1.0
```

## $\color{red}{\text{၂။ Plugin code ရေးခြင်း}}$


```bash
mkdir -p openxr_ws/gz-harmonic/gazebo_openxr_step1
cd openxr_ws/gz-harmonic/gazebo_openxr_step1

mkdir -p gazebo_openxr_step1/{src,include,qml,worlds,config,build}
cd gazebo_openxr_step1
```

10.1. Project Structure
အောက်က Structure က တည်ဆောက်သွားမယ့် Project Design ဖြစ်ပါတယ်။

```text

 gazebo_openxr/
    CMakeLists.txt

    src/
        GazeboOpenXR.cc
        XRRuntime.cc
        XRCamera.cc
        XRRenderer.cc
        XRInput.cc

    include/
        GazeboOpenXR.hh
        XRRuntime.hh
        XRCamera.hh
        XRRenderer.hh
        XRInput.hh

    qml/
        GazeboOpenXR.qml

    worlds/
        xr_test_world.sdf

    config/

    build/
```

```bash
cd openxr_ws/gz-harmonic/gazebo_openxr_step1
cmake -S . -B build-panel -DCMAKE_BUILD_TYPE=Release
```

```bash
cmake --build build-panel -j8
```

ဤ workspace ရှိ `build` cache သည် project path အဟောင်းကို ညွှန်းနေသောကြောင့် `build-panel` ကို အသုံးပြုထားသည်။

## $\color{red}{\text{Q1. VR headset မပါဘဲ project ကို run လို့ရသလား}}$

ရပါတယ်။ `openxr_ws/gz-harmonic/gazebo_openxr_step1` ကို build လုပ်ပြီး Gazebo GUI ထဲမှာ plugin ဖွင့်ကြည့်ဖို့ VR headset မလိုပါဘူး။ Plugin စဖွင့်ချိန်မှာ OpenXR ကို အလိုအလျောက် initialize မလုပ်ပါ။

`Check OpenXR Runtime` သို့မဟုတ် `Start VR Session` ကိုနှိပ်မှ OpenXR runtime နဲ့ headset/system ကို စစ်ဆေးပါတယ်။ Runtime သို့မဟုတ် headset မရှိလျှင် status မှာ အမှားပြပြီး session မစနိုင်ပါ။

## $\color{red}{\text{Q2. Build ပြီးသား project ကို ဘယ်လို run မလဲ}}$

`openxr_ws/gz-harmonic/gazebo_openxr_step1` directory မှာ အောက်ပါ command ကို run ပါ။

```bash
# GUI
export GZ_GUI_PLUGIN_PATH="$PWD/build-panel"
gz sim --force-version 8 -v 4 shapes.sdf
```

Gazebo ပွင့်လာရင် ညာဘက်အပေါ်ရှိ **Plugins** menu ကနေ **GazeboOpenXR** ကိုရွေးပါ။ Plugin panel ကို VR headset မရှိဘဲ ဖွင့်ကြည့်နိုင်ပါတယ်။ **Start VR Session** အတွက်တော့ အလုပ်လုပ်နေတဲ့ OpenXR runtime နဲ့ headset လိုပါတယ်။

Panel ကို အပြည့်မမြင်ရလျှင် title bar ရှိ **□** ဖြင့် floating ပြောင်း၍ ရွှေ့နိုင်သည်။ Panel အတွင်း နေရာမလုံလောက်လျှင် scroll လုပ်နိုင်သည်။ Docked panel ကို မြင်ကွင်းအောက်တွင် ရှာမတွေ့ပါက ညာဘက်အစွန်ရှိ scrollbar ကို ဆွဲပါ။