# $\color{#00c853}{\text{OpenXR setup မှတ်တမ်း}}$

ဤဖိုင်တွင် setup လုပ်စဉ် အသုံးပြုခဲ့သော command များ၊ output များနှင့် code နမူနာများကို မူရင်းအစဉ်အတိုင်း မှတ်တမ်းတင်ထားသည်။

## $\color{red}{\text{စက်ပတ်ဝန်းကျင်နှင့် လိုအပ်ချက် စစ်ဆေးခြင်း}}$

Command
```bash
cat /etc/os-release
```
Output
```text
PRETTY_NAME="Ubuntu 22.04.5 LTS"
NAME="Ubuntu"
VERSION_ID="22.04"
VERSION="22.04.5 LTS (Jammy Jellyfish)"
VERSION_CODENAME=jammy
ID=ubuntu
ID_LIKE=debian
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
UBUNTU_CODENAME=jammy
```
Command
```bash
uname -m
```
Output
```text
x86_64
mr_robot
```

Command
```bash
lscpu | grep "Model name"
```
Output
```text
Model name:                              11th Gen Intel(R) Core(TM) i7-11700 @ 2.50GHz
```
Command
```bash
free -h
```
Output
```text
               total        used        free      shared  buff/cache   available
Mem:            62Gi       5.9Gi        48Gi       109Mi       8.5Gi        55Gi
Swap:           46Gi          0B        46Gi
```

Command
```bash
lspci | grep -iE 'vga|3d|display'
```
Output
```text
00:17.0 SATA controller: Intel Corporation Device 43d2 (rev 11)
01:00.0 VGA compatible controller: NVIDIA Corporation GA106 [GeForce RTX 3060 Lite Hash Rate] (rev a1)
```

Command
```bash
nvidia-smi
```
Output
```text
Tue Nov 17 14:29:22 2026       
+-----------------------------------------------------------------------------------------+
| NVIDIA-SMI 580.178.04             Driver Version: 580.178.04     CUDA Version: 13.0     |
+-----------------------------------------+------------------------+----------------------+
| GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
| Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
|                                         |                        |               MIG M. |
|=========================================+========================+======================|
|   0  NVIDIA GeForce RTX 3060        On  |   00000000:01:00.0  On |                  N/A |
|  0%   35C    P5             25W /  170W |     640MiB /  12288MiB |     48%      Default |
|                                         |                        |                  N/A |
+-----------------------------------------+------------------------+----------------------+

+-----------------------------------------------------------------------------------------+
| Processes:                                                                              |
|  GPU   GI   CI              PID   Type   Process name                        GPU Memory |
|        ID   ID                                                               Usage      |
|=========================================================================================|
|    0   N/A  N/A            5001      G   /usr/lib/xorg/Xorg                      255MiB |
|    0   N/A  N/A            5161      G   /usr/bin/gnome-shell                     40MiB |
|    0   N/A  N/A            5894      G   ...rack-uuid=3190708988185955192        121MiB |
|    0   N/A  N/A            6556      G   ...pareRendererForSitePerProcess         17MiB |
|    0   N/A  N/A           20731      G   ...rack-uuid=3190708988185955192         79MiB |
|    0   N/A  N/A           75173      G   ...7190/usr/bin/telegram-desktop         60MiB |
+-----------------------------------------------------------------------------------------+
```

Command
```bash
# SetUP
sudo apt update
sudo apt install vulkan-tools mesa-utils -y
```

Command
```bash
vulkaninfo --summary
```
Output
```text
WARNING: [Loader Message] Code 0 : terminator_CreateInstance: Failed to CreateInstance in ICD 2.  Skipping ICD.
==========
VULKANINFO
==========

Vulkan Instance Version: 1.3.204

Instance Extensions: count = 20
-------------------------------
VK_EXT_acquire_drm_display             : extension revision 1
VK_EXT_acquire_xlib_display            : extension revision 1
VK_EXT_debug_report                    : extension revision 10
VK_EXT_debug_utils                     : extension revision 2
VK_EXT_direct_mode_display             : extension revision 1
VK_EXT_display_surface_counter         : extension revision 1
VK_EXT_swapchain_colorspace            : extension revision 5
VK_KHR_device_group_creation           : extension revision 1
VK_KHR_display                         : extension revision 23
VK_KHR_external_fence_capabilities     : extension revision 1
VK_KHR_external_memory_capabilities    : extension revision 1
VK_KHR_external_semaphore_capabilities : extension revision 1
VK_KHR_get_display_properties2         : extension revision 1
VK_KHR_get_physical_device_properties2 : extension revision 2
VK_KHR_get_surface_capabilities2       : extension revision 1
VK_KHR_surface                         : extension revision 25
VK_KHR_surface_protected_capabilities  : extension revision 1
VK_KHR_wayland_surface                 : extension revision 6
VK_KHR_xcb_surface                     : extension revision 6
VK_KHR_xlib_surface                    : extension revision 6

Instance Layers: count = 5
--------------------------
VK_LAYER_INTEL_nullhw       INTEL NULL HW                1.1.73   version 1
VK_LAYER_MESA_device_select Linux device selection layer 1.3.211  version 1
VK_LAYER_MESA_overlay       Mesa Overlay layer           1.3.211  version 1
VK_LAYER_NV_optimus         NVIDIA Optimus layer         1.4.312  version 1
VK_LAYER_NV_present         NVIDIA GR2608 layer          1.4.312  version 1

Devices:
========
GPU0:
	apiVersion         = 4211000 (1.4.312)
	driverVersion      = 2435612928 (0x912c8100)
	vendorID           = 0x10de
	deviceID           = 0x2504
	deviceType         = PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
	deviceName         = NVIDIA GeForce RTX 3060
	driverID           = DRIVER_ID_NVIDIA_PROPRIETARY
	driverName         = NVIDIA
	driverInfo         = 580.178.04
	conformanceVersion = 1.4.1.3
	deviceUUID         = a36d16df-52d6-a8e8-6794-a0f5edf13239
	driverUUID         = f0e773c0-8ed4-56b7-842b-3368d137712a
GPU1:
	apiVersion         = 4206847 (1.3.255)
	driverVersion      = 1 (0x0001)
	vendorID           = 0x10005
	deviceID           = 0x0000
	deviceType         = PHYSICAL_DEVICE_TYPE_CPU
	deviceName         = llvmpipe (LLVM 15.0.7, 256 bits)
	driverID           = DRIVER_ID_MESA_LLVMPIPE
	driverName         = llvmpipe
	driverInfo         = Mesa 23.2.1-1ubuntu3.1~22.04.4 (LLVM 15.0.7)
	conformanceVersion = 1.3.1.1
	deviceUUID         = 6d657361-3233-2e32-2e31-2d3175627500
	driverUUID         = 6c6c766d-7069-7065-5555-494400000000
```

Command
```bash
glxinfo -B
```
Output
```text
name of display: :1
display: :1  screen: 0
direct rendering: Yes
Memory info (GL_NVX_gpu_memory_info):
    Dedicated video memory: 12288 MB
    Total available memory: 12288 MB
    Currently available dedicated video memory: 11271 MB
OpenGL vendor string: NVIDIA Corporation
OpenGL renderer string: NVIDIA GeForce RTX 3060/PCIe/SSE2
OpenGL core profile version string: 4.6.0 NVIDIA 580.178.04
OpenGL core profile shading language version string: 4.60 NVIDIA
OpenGL core profile context flags: (none)
OpenGL core profile profile mask: core profile

OpenGL version string: 4.6.0 NVIDIA 580.178.04
OpenGL shading language version string: 4.60 NVIDIA
OpenGL context flags: (none)
OpenGL profile mask: (none)

OpenGL ES profile version string: OpenGL ES 3.2 NVIDIA 580.178.04
OpenGL ES profile shading language version string: OpenGL ES GLSL ES 3.20
```

Command
```bash
gz sim --versions
```
Output
```text
8.15.0
```

Command
```bash
dpkg -l | grep -i openxr
```
Output
```text
ii  libopenxr-dev:amd64                                               1.0.20~dfsg1-1build1                             amd64        OpenXR software development kit -- development headers
ii  libopenxr-loader1:amd64                                           1.0.20~dfsg1-1build1                             amd64        OpenXR loader library
```

Command
```bash
echo "$XR_RUNTIME_JSON"
```
Output
```text
NO output
```

Command
```bash
ls -l ~/.config/openxr/1/
ls -l /etc/xdg/openxr/1/
```
Output
```text
ls: cannot access '/home/mr_robot/.config/openxr/1/': No such file or directory
ls: cannot access '/etc/xdg/openxr/1/': No such file or directory
```

## $\color{red}{\text{OpenXR runtime WiVRn တပ်ဆင် ပြင်ဆင်ခြင်း}}$

Command
```bash
sudo apt install flatpak -y
```

Command
```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
```
Output
```text
Note that the directories 

'/var/lib/flatpak/exports/share'
'/home/mr_robot/.local/share/flatpak/exports/share'

are not in the search path set by the XDG_DATA_DIRS environment
variable, so applications installed by Flatpak may not appear on
your desktop until the session is restarted.
```

Command
```bash
flatpak install flathub io.github.wivrn.wivrn -y
```

Command
```bash
flatpak update -y
```
Output
```text
Note that the directories 

'/var/lib/flatpak/exports/share'
'/home/mr_robot/.local/share/flatpak/exports/share'

are not in the search path set by the XDG_DATA_DIRS environment variable, so
applications installed by Flatpak may not appear on your desktop until the
session is restarted.

Looking for updates…
Nothing to do.
```

Command
```bash
flatpak run io.github.wivrn.wivrn
```

Command
```bash
sudo apt install adb -y
adb devices -l
```

Output
```text
adb reverse tcp:9757 tcp:9757
```

Command
```bash
adb devices -l
```

Output
```text
gsettings set org.gnome.mutter experimental-features "['kms-modifiers', 'headless-start']"
```

Command
```bash
adb shell am start \
  -a android.intent.action.VIEW \
  -d "wivrn+tcp://localhost" \
  org.meumeu.wivrn
```
Output
```text
Starting: Intent { act=android.intent.action.VIEW dat=wivrn+tcp://localhost/... pkg=org.meumeu.wivrn }
```

Command
```bash
ls -l ~/.config/openxr/1/
```
Output
```text
total 4
lrwxrwxrwx 1 mr_robot mr_robot 160 Nov 17 15:46 active_runtime.json -> /var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json
```

Command
```bash
cat ~/.config/openxr/1/active_runtime.json
```
```cpp
{
    "file_format_version": "1.0.0",
    "runtime": {
        "name": "WiVRn",
        "library_path": "../../../lib/wivrn/libopenxr_wivrn.so",
        "MND_libmonado_path": "../../../lib/wivrn/libmonado_wivrn.so"
    }
}
```

Command
```bash
export XR_RUNTIME_JSON="$HOME/.config/openxr/1/active_runtime.json"
```
Output
```text
no output
```

Command
```bash
echo "$XR_RUNTIME_JSON"
```

Output
```text
/home/mr_robot/.config/openxr/1/active_runtime.json
```

Command
```bash
export XR_LOADER_DEBUG=all
```

Command
```bash
export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

echo "$XR_RUNTIME_JSON"
```
Output
```text
/var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json
```

Command
```bash
XR_LIB="$(dirname "$XR_RUNTIME_JSON")/../../../lib/wivrn/libopenxr_wivrn.so"

realpath "$XR_LIB"
ldd "$XR_LIB"
```
Output
```text
/var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/lib/wivrn/libopenxr_wivrn.so
	linux-vdso.so.1 (0x00007ffccc3ad000)
	/usr/lib/x86_64-linux-gnu/libGLEW.so (0x000079befdde0000)
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

Command
ldd "$XR_LIB" | grep "not found"
```

## $\color{red}{\text{OpenXR SDK တပ်ဆင် build စမ်းသပ်ခြင်း}}$

Command
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

Command
```bash
mkdir -p ~/openxr_workspace
cd ~/openxr_workspace
```

Command
```bash
cd ~/openxr_workspace/OpenXR-SDK-Source

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON
```

Command
```bash
cmake --build build --target hello_xr -j8
```

Command
```bash
ls -l build/src/tests/hello_xr/hello_xr
```
Output
```text
-rwxrwxr-x 1 mr_robot mr_robot 303640 Nov 17 16:18 build/src/tests/hello_xr/hello_xr
```

Command
```bash
 export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"
export XR_LOADER_DEBUG=all
```

Command
```bash
cd ~/openxr_workspace/OpenXR-SDK-Source

./build/src/tests/hello_xr/hello_xr -g Vulkan
```
```
