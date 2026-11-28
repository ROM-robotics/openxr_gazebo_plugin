# $\color{#00c853}{\text{OpenXR setup }}$



## $\color{red}{\text{၁။ လိုအပ်ချက် စစ်ဆေးခြင်း}}$
openxr ဆိုတာဘာလဲ?

OpenXR သည် VR/AR application များကို headset နှင့် runtime မျိုးစုံပေါ်တွင် အလုပ်လုပ်စေရန် သတ်မှတ်ထားသော cross-platform API စံနှုန်းတစ်ခုဖြစ်သည်။ Application နှင့် headset/runtime ကြား ဆက်သွယ်ပေးပြီး OpenXR ကိုယ်တိုင်က runtime သို့မဟုတ် application မဟုတ်ပါ။

```bash
cat /etc/os-release
uname -m
lscpu | grep "Model name"

free -h

lspci | grep -iE 'vga|3d|display'

nvidia-smi

# SetUP
sudo apt update
sudo apt install vulkan-tools mesa-utils -y

vulkaninfo --summary

glxinfo -B

gz sim --versions

dpkg -l | grep -i openxr

echo "$XR_RUNTIME_JSON"

ls -l ~/.config/openxr/1/
ls -l /etc/xdg/openxr/1/
```


## $\color{red}{\text{၂။ OpenXR runtime WiVRn တပ်ဆင် ပြင်ဆင်ခြင်း}}$

WiVRn ဆိုတာဘာလဲ?

WiVRn သည် Linux PC မှ VR ပုံရိပ်နှင့် အသံကို Wi-Fi ကွန်ရက်မှတစ်ဆင့် standalone headset သို့ ကြိုးမဲ့ပို့ပေးသော open-source PCVR စနစ်ဖြစ်သည်။ PC ဘက်တွင် OpenXR runtime အဖြစ် အလုပ်လုပ်ပြီး headset ဘက်ရှိ WiVRn app နှင့် ချိတ်ဆက်သည်။

```bash
sudo apt install flatpak -y

flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

flatpak install flathub io.github.wivrn.wivrn -y

flatpak update -y

flatpak run io.github.wivrn.wivrn
```

```bash
sudo apt install adb -y
adb devices -l

adb devices -l

adb shell am start \
  -a android.intent.action.VIEW \
  -d "wivrn+tcp://localhost" \
  org.meumeu.wivrn
  
ls -l ~/.config/openxr/1/

cat ~/.config/openxr/1/active_runtime.json
```
```json
{
    "file_format_version": "1.0.0",
    "runtime": {
        "name": "WiVRn",
        "library_path": "../../../lib/wivrn/libopenxr_wivrn.so",
        "MND_libmonado_path": "../../../lib/wivrn/libmonado_wivrn.so"
    }
}
```

```bash
export XR_RUNTIME_JSON="$HOME/.config/openxr/1/active_runtime.json"

echo "$XR_RUNTIME_JSON"

export XR_LOADER_DEBUG=all

export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

echo "$XR_RUNTIME_JSON"

XR_LIB="$(dirname "$XR_RUNTIME_JSON")/../../../lib/wivrn/libopenxr_wivrn.so"

realpath "$XR_LIB"
ldd "$XR_LIB"
```


```cpp
	libvulkan.so.1 => /lib/x86_64-linux-gnu/libvulkan.so.1 (0x000079befdd3a000)
	libGLX.so.0 => /lib/x86_64-linux-gnu/libGLX.so.0 (0x000079befdd06000)
	libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x000079befda00000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x000079befd919000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x000079befdce4000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x000079befd600000)
	libGL.so.1 => /lib/x86_64-linux-gnu/libGL.so.1 (0x000079befdc5d000)
	libGLdispatch.so.0 => /lib/x86_64-linux-gnu/libGLdispatch.so.0 (0x000079befd861000)
	libX11.so.6 => /lib/x86_64-linux-gnu/libX11.so.6 (0x000079befd4c0000)
	/lib64/ld-linux-x86-64.so.2 (0x000079befdff5000)
	libxcb.so.1 => /lib/x86_64-linux-gnu/libxcb.so.1 (0x000079befdc31000)
	libXau.so.6 => /lib/x86_64-linux-gnu/libXau.so.6 (0x000079befd85b000)
	libXdmcp.so.6 => /lib/x86_64-linux-gnu/libXdmcp.so.6 (0x000079befd853000)
	libbsd.so.0 => /lib/x86_64-linux-gnu/libbsd.so.0 (0x000079befd83b000)
	libmd.so.0 => /lib/x86_64-linux-gnu/libmd.so.0 (0x000079befd82e000)
```

 
```bash
ldd "$XR_LIB" | grep "not found"
```

## $\color{red}{\text{၃။ OpenXR SDK တပ်ဆင် build စမ်းသပ်ခြင်း}}$
open xr sdk က ဘာအတွက်လဲ?

OpenXR SDK တွင် OpenXR application များရေးသား၊ build လုပ်၊ စမ်းသပ်ရန် လိုအပ်သော header၊ library၊ tool နှင့် sample များ ပါဝင်သည်။ SDK သည် runtime ကို အစားမထိုးပါ။ ဤနေရာတွင် `hello_xr` sample ကို run လုပ်ခြင်းဖြင့် တပ်ဆင်ထားသော runtime နှင့် ချိတ်ဆက်မှုကို စမ်းသပ်နိုင်သည်။

```bash
sudo apt install -y \
  git \
  cmake \
  build-essential \
  python3 \
  python3-jinja2 \
  libgl1-mesa-dev \
  libegl1-mesa-dev \
  libvulkan-dev \
  libx11-dev \
  libx11-xcb-dev \
  libxcb-glx0-dev \
  libxcb-dri2-0-dev \
  libxcb-icccm4-dev \
  libxcb-keysyms1-dev \
  libxcb-randr0-dev \
  libxxf86vm-dev \
  libxrandr-dev \
  glslang-tools
```

 
```bash
cd $HOME/Desktop/Git/openxr_gazebo_plugin/openxr_sdk_source
mkdir -p build/linux_release
```

Linux Release
```bash
cmake -S . -B build/linux -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make
```

Linux Debug
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
```

Linux Release (with debug symbols)
```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../..
make
```
 
```bash
cmake --build build/linux --target hello_xr -j8
```

 
```bash
ls -l build/linux/src/tests/hello_xr/hello_xr
```

```bash
export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"
export XR_LOADER_DEBUG=all
```

 
```bash
cd $HOME/Desktop/Git/openxr_gazebo_plugin/openxr_sdk_source
./build/linux/src/tests/hello_xr/hello_xr -g Vulkan
```
