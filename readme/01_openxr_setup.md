# $\color{#00c853}{\text{OpenXR setup မှတ်တမ်း}}$

ဤဖိုင်တွင် setup လုပ်စဉ် အသုံးပြုခဲ့သော command များ၊ output များနှင့် code နမူနာများကို မူရင်းအစဉ်အတိုင်း မှတ်တမ်းတင်ထားသည်။

## $\color{red}{\text{Terminal မှတ်တမ်း}}$

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
#############################################
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
#############################################
Command
```bash
lspci | grep -iE 'vga|3d|display'
```
Output
```text
00:17.0 SATA controller: Intel Corporation Device 43d2 (rev 11)
01:00.0 VGA compatible controller: NVIDIA Corporation GA106 [GeForce RTX 3060 Lite Hash Rate] (rev a1)
```
#############################################
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
#############################################
Command
```bash
# SetUP
sudo apt update
sudo apt install vulkan-tools mesa-utils -y
```
#############################################
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

#############################################
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
#############################################
Command
```bash
gz sim --versions
```
Output
```text
8.15.0
```
#############################################
Command
```bash
dpkg -l | grep -i openxr
```
Output
```text
ii  libopenxr-dev:amd64                                               1.0.20~dfsg1-1build1                             amd64        OpenXR software development kit -- development headers
ii  libopenxr-loader1:amd64                                           1.0.20~dfsg1-1build1                             amd64        OpenXR loader library
```
#############################################
Command
```bash
echo "$XR_RUNTIME_JSON"
```
Output
```text
NO output
```
#############################################
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
#############################################
Command
```bash
sudo apt install flatpak -y
```
#############################################
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
#############################################
Command
```bash
flatpak install flathub io.github.wivrn.wivrn -y
```
#############################################
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
#############################################
Command
```bash
flatpak run io.github.wivrn.wivrn
```
#############################################
Command
```bash
sudo apt install adb -y
adb devices -l
```
#############################################
Output
```text
adb reverse tcp:9757 tcp:9757
```
#############################################
Command
```bash
adb devices -l
```
#############################################
Output
```text
gsettings set org.gnome.mutter experimental-features "['kms-modifiers', 'headless-start']"
```
#############################################
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
#############################################

Command
```bash
ls -l ~/.config/openxr/1/
```
Output
```text
total 4
lrwxrwxrwx 1 mr_robot mr_robot 160 Nov 17 15:46 active_runtime.json -> /var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json
```

#############################################
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

############################################


Command
```bash
export XR_RUNTIME_JSON="$HOME/.config/openxr/1/active_runtime.json"
```
Output
```text
no output
```

############################################

Command
```bash
echo "$XR_RUNTIME_JSON"
```

Output
```text
/home/mr_robot/.config/openxr/1/active_runtime.json
```

############################################

Command
```bash
export XR_LOADER_DEBUG=all
```

###############################################
############################################

Command
```bash
export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

echo "$XR_RUNTIME_JSON"
```
Output
```text
/var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json
```

###############################################

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


###############################################
Command
ldd "$XR_LIB" | grep "not found"
```

###############################################

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


###############################################

Command
```bash
mkdir -p ~/openxr_workspace
cd ~/openxr_workspace
```

###############################################

Command
```bash
cd ~/openxr_workspace/OpenXR-SDK-Source

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON
```

###############################################

Command
```bash
cmake --build build --target hello_xr -j8
```

###############################################

Command
```bash
ls -l build/src/tests/hello_xr/hello_xr
```
Output
```text
-rwxrwxr-x 1 mr_robot mr_robot 303640 Nov 17 16:18 build/src/tests/hello_xr/hello_xr
```

###############################################

Command
```bash
 export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"
export XR_LOADER_DEBUG=all
```

###############################################

Command
```bash
cd ~/openxr_workspace/OpenXR-SDK-Source

./build/src/tests/hello_xr/hello_xr -g Vulkan
```

###############################################
###############################################

Command
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

###############################################

Command
```bash
pkg-config --list-all | grep -E 'gz-|openxr|sdformat'
```

###############################################

Command
```bash
pkg-config --modversion gz-sim8
pkg-config --modversion gz-gui8
pkg-config --modversion gz-rendering8
pkg-config --modversion gz-transport13
pkg-config --modversion gz-msgs10
pkg-config --modversion openxr
```
Output
```text
8.15.0
8.4.0
8.3.0
13.6.0
10.4.0
1.0
```

###############################################

Command
```bash
cd ~/openxr_workspace

mkdir -p gazebo_openxr/{src,include,qml,worlds,config,build}

cd gazebo_openxr
```

###############################################

10.1. Project Structure
အောက်က Structure က တည်ဆောက်သွားမယ့် Project Design ဖြစ်ပါတယ်။
Output
```text
 openxr_workspace/
 OpenXR-SDK-Source/

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
 
 
###############################################

Command
```bash
mr_robot@desktop:~/openxr_workspace/gazebo_openxr$ vim include/GazeboOpenXR.hh
```

```cpp
#pragma once

#include <QString>
#include <gz/gui/Plugin.hh>

class GazeboOpenXR : public gz::gui::Plugin
{
    Q_OBJECT

    Q_PROPERTY(QString status
               READ Status
               NOTIFY StatusChanged)

public:
    GazeboOpenXR();
    ~GazeboOpenXR() override;

    void LoadConfig(
        const tinyxml2::XMLElement *_pluginElem
    ) override;

    QString Status() const;

    Q_INVOKABLE void CheckRuntime();

signals:
    void StatusChanged();

private:
    QString status_ = "OpenXR not initialized";

    void SetStatus(const QString &status);
};
```

###############################################

Command
```bash
vim src/GazeboOpenXR.cc
```

```cpp
#include "GazeboOpenXR.hh"

#include <cstring>
#include <iostream>
#include <openxr/openxr.h>
#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";
}

QString GazeboOpenXR::Status() const
{
    return status_;
}

void GazeboOpenXR::SetStatus(
    const QString &status)
{
    status_ = status;
    emit StatusChanged();

    std::cout << "[GazeboOpenXR] "
              << status.toStdString()
              << std::endl;
}

void GazeboOpenXR::CheckRuntime()
{
    XrInstance instance = XR_NULL_HANDLE;

    XrInstanceCreateInfo createInfo{
        XR_TYPE_INSTANCE_CREATE_INFO
    };

    std::strncpy(
        createInfo.applicationInfo.applicationName,
        "GazeboOpenXR",
        XR_MAX_APPLICATION_NAME_SIZE - 1
    );

    createInfo.applicationInfo.applicationVersion = 1;
    createInfo.applicationInfo.apiVersion =
        XR_MAKE_VERSION(1, 0, 0);

    XrResult result =
        xrCreateInstance(&createInfo, &instance);

    if (XR_FAILED(result))
    {
        SetStatus(
            QString("xrCreateInstance failed: %1")
            .arg(static_cast<int>(result))
        );
        return;
    }

    XrInstanceProperties properties{
        XR_TYPE_INSTANCE_PROPERTIES
    };

    result = xrGetInstanceProperties(
        instance, &properties
    );

    if (XR_FAILED(result))
    {
        SetStatus("Failed to read runtime properties");
        xrDestroyInstance(instance);
        return;
    }

    XrSystemGetInfo systemInfo{
        XR_TYPE_SYSTEM_GET_INFO
    };

    systemInfo.formFactor =
        XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrSystemId systemId = XR_NULL_SYSTEM_ID;

    result = xrGetSystem(
        instance, &systemInfo, &systemId
    );

    if (XR_FAILED(result))
    {
        SetStatus(
            QString("Headset unavailable: %1")
            .arg(static_cast<int>(result))
        );

        xrDestroyInstance(instance);
        return;
    }

    XrSystemProperties systemProperties{
        XR_TYPE_SYSTEM_PROPERTIES
    };

    result = xrGetSystemProperties(
        instance,
        systemId,
        &systemProperties
    );

    if (XR_SUCCEEDED(result))
    {
        SetStatus(
            QString("Runtime: %1\nSystem: %2\nOpenXR Ready")
            .arg(QString::fromUtf8(
                properties.runtimeName))
            .arg(QString::fromUtf8(
                systemProperties.systemName))
        );
    }
    else
    {
        SetStatus("Failed to read system properties");
    }

    xrDestroyInstance(instance);
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
```


###############################################

Command
```bash
nano qml/GazeboOpenXR.qml
```

```qml
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Rectangle {
    anchors.fill: parent
    color: "#202631"

    implicitWidth: 360
    implicitHeight: 280

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: "Gazebo OpenXR"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        Label {
            text: "Meta Quest | WiVRn Runtime"
            color: "#a0aec0"
            font.pixelSize: 13
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#475569"
        }

        Label {
            text: GazeboOpenXR.status
            color: "#4ade80"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            text: "Check OpenXR Runtime"
            Layout.fillWidth: true

            onClicked: {
                GazeboOpenXR.CheckRuntime()
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
```

###############################################

Command
```bash
vim GazeboOpenXR.qrc
```

```xml
<!DOCTYPE RCC>
<RCC version="1.0">
    <qresource prefix="/GazeboOpenXR">
        <file alias="GazeboOpenXR.qml">
            qml/GazeboOpenXR.qml
        </file>
    </qresource>
</RCC>
```

###############################################

Command
```bash
vim CMakeLists.txt
```

```cmake
cmake_minimum_required(VERSION 3.16)

project(GazeboOpenXR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
)

find_package(gz-gui8 REQUIRED)

find_package(OpenXR REQUIRED)

add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    include/GazeboOpenXR.hh
    GazeboOpenXR.qrc
)

target_include_directories(GazeboOpenXR PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GazeboOpenXR PRIVATE
    gz-gui8::gz-gui8
    OpenXR::openxr_loader
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

set_target_properties(GazeboOpenXR PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}"
)
```


###############################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release
```

Output
```text
Build files have been written to: /home/mr_robot/openxr_workspace/gazebo_openxr/build
```

###############################################

Command
```bash
cmake --build build -j8

cmake --build build -j8
```
Output
```text
[ 16%] Automatic MOC for target GazeboOpenXR
[ 16%] Built target GazeboOpenXR_autogen
gmake[2]: *** No rule to make target '../            qml/GazeboOpenXR.qml', needed by 'GazeboOpenXR_autogen/EWIEGA46WW/qrc_GazeboOpenXR.cpp'.  Stop.
gmake[1]: *** [CMakeFiles/Makefile2:83: CMakeFiles/GazeboOpenXR.dir/all] Error 2
gmake: *** [Makefile:91: all] Error 2
```

###############################################

Command
```bash
ls -lh build/libGazeboOpenXR.so
```
Output
```text
ls: cannot access 'build/libGazeboOpenXR.so': No such file or directory
```

###############################################

Command
```bash
ldd build/libGazeboOpenXR.so | grep "not found"
```
Output
```text
ldd: build/libGazeboOpenXR.so: No such file or directory
```

###############################################

Error Analysis
Output
```text
No rule to make target '../            qml/GazeboOpenXR.qml'
```


qml/ ရဲ့ အရှေ့မှာ Space တွေ ပါနေပါတယ်။ Qt Resource Compiler (RCC) က အဲဒီ Space တွေကို File Path ရဲ့ အစိတ်အပိုင်းအဖြစ် ယူလိုက်တဲ့အတွက် QML File ကို ရှာမတွေ့တာပါ။

အရင်ပေးခဲ့တဲ့ QRC Code ထဲမှာ File Path ကို စာကြောင်းခွဲရေးထားတာကြောင့် ဖြစ်ပါတယ်။
2. GazeboOpenXR.qrc ကို ပြင်ပါ
Command
```bash
cd ~/openxr_workspace/gazebo_openxr

vim GazeboOpenXR.qrc
```


အထဲက Code အားလုံးကို ဖျက်ပြီး အောက်က Code နဲ့ အစားထိုးပါ။
```xml
<!DOCTYPE RCC>
<RCC version="1.0">
    <qresource prefix="/GazeboOpenXR">
        <file alias="GazeboOpenXR.qml">qml/GazeboOpenXR.qml</file>
    </qresource>
</RCC>
```

###############################################

Command
```bash
ls -lh qml/GazeboOpenXR.qml
```
Output
```text
-rw-rw-r-- 1 mr_robot mr_robot 1.1K Nov 17 17:19 qml/GazeboOpenXR.qml
```

###############################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

rm -rf build

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

###############################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

export GZ_GUI_PLUGIN_PATH="$PWD/build${GZ_GUI_PLUGIN_PATH:+:$GZ_GUI_PLUGIN_PATH}"

export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=all
```

###############################################

Command
```bash
echo "$GZ_GUI_PLUGIN_PATH"
echo "$XR_RUNTIME_JSON"
```
Output
```text
/home/mr_robot/openxr_workspace/gazebo_openxr/build
/var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json
```


###############################################

Command
```bash
gz sim --force-version 8 -v 4 shapes.sdf
```

Output
```text
[GUI] [Err] [Application.cc:556] Failed to load plugin [GazeboOpenXR] : couldn't load library on path [/home/mr_robot/openxr_workspace/gazebo_openxr/build/libGazeboOpenXR.so].
```

###############################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found|error'
```
Output
```text
undefined symbol: _ZTI12GazeboOpenXR	(build/libGazeboOpenXR.so)
undefined symbol: _ZTV12GazeboOpenXR	(build/libGazeboOpenXR.so)
undefined symbol: _ZN12GazeboOpenXR13StatusChangedEv	(build/libGazeboOpenXR.so)
```
Command
```bash
mr_robot@desktop:~/openxr_workspace/gazebo_openxr$ nm -D build/libGazeboOpenXR.so | \
grep -E 'GzPluginHook|IgnitionPluginHook'
```
Output
```text
0000000000005b10 T GzPluginHook
```
Command
```bash
mr_robot@desktop:~/openxr_workspace/gazebo_openxr$ vim CMakeLists.txt
```

###############################################

```cmake
cmake_minimum_required(VERSION 3.16)

project(GazeboOpenXR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
)

find_package(gz-gui8 REQUIRED)
find_package(gz-plugin2 REQUIRED COMPONENTS register)
find_package(OpenXR REQUIRED)

add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    include/GazeboOpenXR.hh
    GazeboOpenXR.qrc
)

target_include_directories(GazeboOpenXR PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GazeboOpenXR PRIVATE
    gz-gui8::gz-gui8
    gz-plugin2::register
    OpenXR::openxr_loader
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

target_link_options(GazeboOpenXR PRIVATE
    "-Wl,--no-undefined"
)

set_target_properties(GazeboOpenXR PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
```

###############################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

rm -rf build

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

###############################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

cat build/GazeboOpenXR_autogen/mocs_compilation.cpp
```

###############################################

Command
```bash
find build -name "moc_GazeboOpenXR.cpp"
```

###############################################

Command
```bash
vim CMakeLists.txt
```

###############################################

```cmake
cmake_minimum_required(VERSION 3.16)

project(GazeboOpenXR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
)

find_package(gz-gui8 REQUIRED)
find_package(gz-plugin2 REQUIRED COMPONENTS register)
find_package(OpenXR REQUIRED)

# Explicit Qt MOC generation
qt5_wrap_cpp(MOC_SOURCES
    include/GazeboOpenXR.hh
)

# Explicit Qt resource generation
qt5_add_resources(QRC_SOURCES
    GazeboOpenXR.qrc
)

add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    ${MOC_SOURCES}
    ${QRC_SOURCES}
)

target_include_directories(GazeboOpenXR PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GazeboOpenXR PRIVATE
    gz-gui8::gz-gui8
    gz-plugin2::register
    OpenXR::openxr_loader
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

target_compile_options(GazeboOpenXR PRIVATE
    -frtti
)

target_link_options(GazeboOpenXR PRIVATE
    "-Wl,--no-undefined"
)

set_target_properties(GazeboOpenXR PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
```

###############################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

rm -rf build

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8 2>&1 | tee build.log
```

###############################################

Command
```bash
ls -lh build/libGazeboOpenXR.so

ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'

nm -DC build/libGazeboOpenXR.so | \
grep -E 'typeinfo for GazeboOpenXR|vtable for GazeboOpenXR|GazeboOpenXR::staticMetaObject'
```

Output
```text
D GazeboOpenXR::staticMetaObject
V typeinfo for GazeboOpenXR
V vtable for GazeboOpenXR
```

###############################################

Command
```bash
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=all

gz gui -v 4 -s GazeboOpenXR
```

###############################################
###############################################

OpenXR Session Creation

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

echo "$XDG_SESSION_TYPE"
echo "$DISPLAY"

pkg-config --modversion gl
pkg-config --modversion glew
pkg-config --modversion x11
```
Output
```text
x11
:1
1.2
2.2.0
1.7.5
```

###############################################

Command
```bash
mr_robot@desktop:~/openxr_workspace/gazebo_openxr$ cd ~/openxr_workspace/gazebo_openxr

git init
git add CMakeLists.txt GazeboOpenXR.qrc src include qml
git commit -m "Phase 1: Working Gazebo OpenXR plugin"
```
Output
```text
hint: Using 'master' as the name for the initial branch. This default branch name
hint: is subject to change. To configure the initial branch name to use in all
hint: of your new repositories, which will suppress this warning, call:
hint: 
hint: 	git config --global init.defaultBranch <name>
hint: 
hint: Names commonly chosen instead of 'master' are 'main', 'trunk' and
hint: 'development'. The just-created branch can be renamed via this command:
hint: 
hint: 	git branch -m <name>
Initialized empty Git repository in /home/mr_robot/openxr_workspace/gazebo_openxr/.git/
[master (root-commit) 70f9638] Phase 1: Working Gazebo OpenXR plugin
 5 files changed, 300 insertions(+)
 create mode 100644 CMakeLists.txt
 create mode 100644 GazeboOpenXR.qrc
 create mode 100644 include/GazeboOpenXR.hh
 create mode 100644 qml/GazeboOpenXR.qml
 create mode 100644 src/GazeboOpenXR.cc
```
 
 ###############################################
 
Command
```bash
 mr_robot@desktop:~/openxr_workspace/gazebo_openxr$ touch include/XRRuntime.hh
touch include/XRRenderer.hh
touch include/XRCamera.hh
touch include/XRInput.hh

touch src/XRRuntime.cc
touch src/XRRenderer.cc
touch src/XRCamera.cc
touch src/XRInput.cc
```

 ###############################################
 
 Final Project Structure
Output
```text
gazebo_openxr/
├── CMakeLists.txt
├── GazeboOpenXR.qrc
├── include/
│   ├── GazeboOpenXR.hh
│   ├── XRRuntime.hh
│   ├── XRRenderer.hh
│   ├── XRCamera.hh
│   └── XRInput.hh
├── src/
│   ├── GazeboOpenXR.cc
│   ├── XRRuntime.cc
│   ├── XRRenderer.cc
│   ├── XRCamera.cc
│   └── XRInput.cc
├── qml/
│   └── GazeboOpenXR.qml
├── worlds/
├── config/
└── build/
```

 ###############################################
 
Command
```bash
 cd ~/openxr_workspace

cp -a gazebo_openxr gazebo_openxr_phase1_backup

cd gazebo_openxr
```

 ###############################################
 
Command
```bash
 vim include/XRRuntime.hh
```
 
```cpp
 #pragma once

#include <openxr/openxr.h>
#include <string>

class XRRuntime
{
public:
    XRRuntime() = default;
    ~XRRuntime();

    XRRuntime(const XRRuntime&) = delete;
    XRRuntime& operator=(const XRRuntime&) = delete;

    bool Initialize();
    void Shutdown();

    bool IsInitialized() const;

    XrInstance GetInstance() const;
    XrSystemId GetSystemId() const;

    std::string GetStatus() const;

private:
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;

    bool initialized_ = false;

    std::string status_ = "Not initialized";

    bool Fail(const std::string& message,
              XrResult result);
};
```

  ###############################################
  
Command
```bash
  vim src/XRRuntime.cc
```
 
```cpp
 #include "XRRuntime.hh"

#define XR_USE_GRAPHICS_API_OPENGL
#include <openxr/openxr_platform.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

XRRuntime::~XRRuntime()
{
    Shutdown();
}

bool XRRuntime::Initialize()
{
    if (initialized_)
        return true;

    uint32_t count = 0;

    XrResult result =
        xrEnumerateInstanceExtensionProperties(
            nullptr, 0, &count, nullptr);

    if (XR_FAILED(result))
        return Fail("Extension enumeration", result);

    std::vector<XrExtensionProperties> extensions(
        count, {XR_TYPE_EXTENSION_PROPERTIES});

    result = xrEnumerateInstanceExtensionProperties(
        nullptr, count, &count, extensions.data());

    if (XR_FAILED(result))
        return Fail("Extension properties", result);

    bool openGLSupported = false;

    for (const auto& extension : extensions)
    {
        if (std::strcmp(
                extension.extensionName,
                XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) == 0)
        {
            openGLSupported = true;
        }
    }

    if (!openGLSupported)
    {
        status_ = "OpenGL extension not supported";
        return false;
    }

    const char* enabledExtensions[] = {
        XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
    };

    XrInstanceCreateInfo createInfo{
        XR_TYPE_INSTANCE_CREATE_INFO
    };

    std::strncpy(
        createInfo.applicationInfo.applicationName,
        "GazeboOpenXR",
        XR_MAX_APPLICATION_NAME_SIZE - 1);

    createInfo.applicationInfo.applicationVersion = 1;
    createInfo.applicationInfo.apiVersion =
        XR_MAKE_VERSION(1, 0, 0);

    createInfo.enabledExtensionCount = 1;
    createInfo.enabledExtensionNames = enabledExtensions;

    result = xrCreateInstance(&createInfo, &instance_);

    if (XR_FAILED(result))
        return Fail("xrCreateInstance", result);

    XrInstanceProperties instanceProperties{
        XR_TYPE_INSTANCE_PROPERTIES
    };

    result = xrGetInstanceProperties(
        instance_, &instanceProperties);

    if (XR_FAILED(result))
        return Fail("xrGetInstanceProperties", result);

    XrSystemGetInfo systemInfo{
        XR_TYPE_SYSTEM_GET_INFO
    };

    systemInfo.formFactor =
        XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(
        instance_, &systemInfo, &systemId_);

    if (XR_FAILED(result))
        return Fail("xrGetSystem", result);

    XrSystemProperties systemProperties{
        XR_TYPE_SYSTEM_PROPERTIES
    };

    result = xrGetSystemProperties(
        instance_, systemId_, &systemProperties);

    if (XR_FAILED(result))
        return Fail("xrGetSystemProperties", result);

    PFN_xrGetOpenGLGraphicsRequirementsKHR
        getGraphicsRequirements = nullptr;

    result = xrGetInstanceProcAddr(
        instance_,
        "xrGetOpenGLGraphicsRequirementsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(
            &getGraphicsRequirements));

    if (XR_FAILED(result) || !getGraphicsRequirements)
        return Fail("OpenGL function lookup",
                    XR_FAILED(result)
                    ? result : XR_ERROR_FUNCTION_UNSUPPORTED);

    XrGraphicsRequirementsOpenGLKHR requirements{
        XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR
    };

    result = getGraphicsRequirements(
        instance_, systemId_, &requirements);

    if (XR_FAILED(result))
        return Fail("OpenGL graphics requirements", result);

    std::ostringstream output;

    output << "Runtime: "
           << instanceProperties.runtimeName
           << "\nSystem: "
           << systemProperties.systemName
           << "\nOpenGL Extension: Supported"
           << "\nMinimum OpenGL: "
           << XR_VERSION_MAJOR(requirements.minApiVersionSupported)
           << "."
           << XR_VERSION_MINOR(requirements.minApiVersionSupported)
           << "\nMaximum Tested OpenGL: "
           << XR_VERSION_MAJOR(requirements.maxApiVersionSupported)
           << "."
           << XR_VERSION_MINOR(requirements.maxApiVersionSupported)
           << "\nOpenXR Runtime Ready";

    status_ = output.str();
    initialized_ = true;

    return true;
}

void XRRuntime::Shutdown()
{
    if (instance_ != XR_NULL_HANDLE)
    {
        xrDestroyInstance(instance_);
        instance_ = XR_NULL_HANDLE;
    }

    systemId_ = XR_NULL_SYSTEM_ID;
    initialized_ = false;
}

bool XRRuntime::Fail(
    const std::string& message, XrResult result)
{
    status_ = message + " failed: " +
        std::to_string(static_cast<int>(result));

    Shutdown();
    return false;
}

bool XRRuntime::IsInitialized() const
{
    return initialized_;
}

XrInstance XRRuntime::GetInstance() const
{
    return instance_;
}

XrSystemId XRRuntime::GetSystemId() const
{
    return systemId_;
}

std::string XRRuntime::GetStatus() const
{
    return status_;
}
```

#####################################################

Command
```bash
vim include/GazeboOpenXR.hh
```

```cpp
#pragma once

#include <QString>
#include <gz/gui/Plugin.hh>

#include "XRRuntime.hh"

class GazeboOpenXR : public gz::gui::Plugin
{
    Q_OBJECT

    Q_PROPERTY(QString status
               READ Status
               NOTIFY StatusChanged)

public:
    GazeboOpenXR();
    ~GazeboOpenXR() override;

    void LoadConfig(
        const tinyxml2::XMLElement *_pluginElem
    ) override;

    QString Status() const;

    Q_INVOKABLE void CheckRuntime();

signals:
    void StatusChanged();

private:
    XRRuntime runtime_;

    QString status_ = "OpenXR not initialized";

    void SetStatus(const QString &status);
};
```

#####################################################

Command
```bash
vim src/GazeboOpenXR.cc
```

```cpp
#include "GazeboOpenXR.hh"

#include <iostream>
#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";
}

QString GazeboOpenXR::Status() const
{
    return status_;
}

void GazeboOpenXR::SetStatus(
    const QString &status)
{
    status_ = status;

    emit StatusChanged();

    std::cout << "[GazeboOpenXR] "
              << status.toStdString()
              << std::endl;
}

void GazeboOpenXR::CheckRuntime()
{
    runtime_.Initialize();

    SetStatus(
        QString::fromStdString(
            runtime_.GetStatus()));
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
```

#####################################################

လက်ရှိ အလုပ်လုပ်နေတဲ့ CMakeLists.txt အားလုံးကို ပြန်ပြင်စရာမလိုပါဘူး။

add_library() အပိုင်းကိုပဲ အောက်ပါအတိုင်း ပြောင်းပါ။
```cmake
add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    src/XRRuntime.cc
    ${MOC_SOURCES}
    ${QRC_SOURCES}
)
```

#####################################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

#####################################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8

ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```

#####################################################

Command
```bash
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=all

gz sim --force-version 8 -v 4 shapes.sdf
```

Output
```text
UI: Gazebo Open XR 
Meta Quest | WiVRn Runtime
OpenXR not initialized
xrCreateInstanced failed: 51

[GUI] [Dbg] [Application.cc:528] Loading plugin [GazeboOpenXR]
[GUI] [Msg] Added plugin [Gazebo OpenXR] to main window
[GUI] [Msg] Loaded plugin [GazeboOpenXR] from path [/home/mr_robot/openxr_workspace/gazebo_openxr/build/libGazeboOpenXR.so]
Verbose [GENERAL | xrEnumerateInstanceExtensionProperties | OpenXR-Loader] : Entering loader trampoline
Info [GENERAL |  | OpenXR-Loader] : RuntimeManifestFile::FindManifestFiles - using environment variable override runtime file /var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json
Info [GENERAL |  | OpenXR-Loader] : RuntimeManifestFile::CreateIfValid - attempting to load /var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json
Info [GENERAL | xrEnumerateInstanceExtensionProperties | OpenXR-Loader] : RuntimeInterface::LoadRuntime succeeded loading runtime defined in manifest file /var/lib/flatpak/app/io.github.wivrn.wivrn/x86_64/stable/61d58e18b177b19a34f4a0f3063c571bfc7585ef8a3145327a5beb8b5e268332/files/share/openxr/1/openxr_wivrn.json using interface version 1 and OpenXR API version 1.1
Verbose [GENERAL | xrEnumerateInstanceExtensionProperties | OpenXR-Loader] : Completed loader trampoline
Verbose [GENERAL | xrEnumerateInstanceExtensionProperties | OpenXR-Loader] : Entering loader trampoline
Verbose [GENERAL | xrEnumerateInstanceExtensionProperties | OpenXR-Loader] : Completed loader trampoline
Verbose [GENERAL | xrCreateInstance | OpenXR-Loader] : Entering loader trampoline
Verbose [GENERAL | xrCreateInstance | OpenXR-Loader] : Entering LoaderInstance::CreateInstance
Verbose [GENERAL | xrCreateInstance | OpenXR-Loader] : Entering loader terminator
ERROR [ipc_client_socket_connect] Failed to connect to socket /run/user/1000/wivrn/comp_ipc: No such file or directory!
ERROR [ipc_client_connection_init] Failed to connect to monado service process

###
#
# Please make sure that the service process is running
#
# It is called "monado-service"
# In build trees, it is located "build-dir/src/xrt/targets/service/monado-service"
#
###
XR_ERROR_RUNTIME_UNAVAILABLE in xrCreateInstance: Failed to create instance '-1'
Verbose [GENERAL | xrCreateInstance | OpenXR-Loader] : Completed loader terminator
Error [GENERAL | xrCreateInstance | OpenXR-Loader] : LoaderInstance::CreateInstance chained CreateInstance call failed
Error [GENERAL | xrCreateInstance | OpenXR-Loader] : LoaderInstance::CreateInstance chained CreateInstance call failed
Info [GENERAL | xrCreateInstance | OpenXR-Loader] : RuntimeInterface::UnloadRuntime - Unloading RuntimeInterface
Info [GENERAL |  | OpenXR-Loader] : RuntimeInterface being destroyed.
Error [GENERAL | xrCreateInstance | OpenXR-Loader] : xrCreateInstance failed
Error [GENERAL | xrCreateInstance | OpenXR-Loader] : xrCreateInstance failed
[GazeboOpenXR] xrCreateInstance failed: -51
```


WivRn ဖွင့်ထားပြီး VR နဲ့  connect ဖြစ်နေဖို့လိုအပ်တယ်။
####################################


Command
```bash
echo "$XDG_SESSION_TYPE"
echo "$DISPLAY"

pkg-config --modversion gl
pkg-config --modversion glew
pkg-config --modversion x11
```
Output
```text
x11
:1
1.2
2.2.0
1.7.5
```

####################################

Command
```bash
cd ~/openxr_workspace

cp -a gazebo_openxr gazebo_openxr_phase2_1_backup

cd gazebo_openxr
```

####################################

Command
```bash
vim include/XRRuntime.hh
```

public: အောက်မှာ အောက်ပါ Functions တွေ ထပ်ထည့်ပါ။
```cpp
bool ProbeGraphicsContext();

bool CreateSessionFromCurrentContext();

void DestroySession();

XrSession GetSession() const;
```


private: အောက်မှာ အောက်ပါ Variables တွေ ထပ်ထည့်ပါ။
```cpp
XrSession session_ = XR_NULL_HANDLE;

XrSpace localSpace_ = XR_NULL_HANDLE;
```

####################################

src/XRRuntime.cc ရဲ့ အပေါ်ဆုံး Include အပိုင်းကို အောက်ပါအတိုင်း ပြောင်းပါ။

```cpp
#include "XRRuntime.hh"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#define XR_USE_PLATFORM_XLIB
#define XR_USE_GRAPHICS_API_OPENGL

#include <openxr/openxr_platform.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
```

src/XRRuntime.cc ရဲ့ အောက်ဆုံးမှာ ဒီ Function ကို ထည့်ပါ။
```cpp
bool XRRuntime::ProbeGraphicsContext()
{
    Display* display = glXGetCurrentDisplay();

    GLXContext context = glXGetCurrentContext();

    GLXDrawable drawable = glXGetCurrentDrawable();

    if (!display || !context || !drawable)
    {
        std::cout
            << "[XRRuntime] No current GLX context"
            << std::endl;

        return false;
    }

    std::cout
        << "[XRRuntime] GLX Context Ready\n"
        << "OpenGL Vendor: "
        << glGetString(GL_VENDOR)
        << "\nOpenGL Renderer: "
        << glGetString(GL_RENDERER)
        << "\nOpenGL Version: "
        << glGetString(GL_VERSION)
        << std::endl;

    return true;
}
```

အောက်ပါ Function ကိုလည်း ဆက်ထည့်ပါ။
```cpp
bool XRRuntime::CreateSessionFromCurrentContext()
{
    if (session_ != XR_NULL_HANDLE)
        return true;

    if (!initialized_)
        return false;

    if (!ProbeGraphicsContext())
        return false;

    Display* display = glXGetCurrentDisplay();
    GLXContext context = glXGetCurrentContext();
    GLXDrawable drawable = glXGetCurrentDrawable();

    int configId = 0;
    int screen = 0;

    if (glXQueryContext(display, context,
                        GLX_FBCONFIG_ID, &configId) != 0 ||
        glXQueryContext(display, context,
                        GLX_SCREEN, &screen) != 0)
    {
        status_ = "GLX context query failed";
        return false;
    }

    int attributes[] = {
        GLX_FBCONFIG_ID, configId,
        None
    };

    int count = 0;

    GLXFBConfig* configs = glXChooseFBConfig(
        display, screen, attributes, &count);

    if (!configs || count == 0)
    {
        if (configs)
            XFree(configs);

        status_ = "GLX FBConfig not found";
        return false;
    }

    GLXFBConfig config = configs[0];
    XFree(configs);

    int visualId = 0;

    if (glXGetFBConfigAttrib(
            display, config,
            GLX_VISUAL_ID, &visualId) != 0 ||
        visualId == 0)
    {
        status_ = "GLX Visual ID unavailable";
        return false;
    }

    XrGraphicsBindingOpenGLXlibKHR binding{
        XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR
    };

    binding.xDisplay = display;
    binding.visualid = visualId;
    binding.glxFBConfig = config;
    binding.glxDrawable = drawable;
    binding.glxContext = context;

    XrSessionCreateInfo createInfo{
        XR_TYPE_SESSION_CREATE_INFO
    };

    createInfo.next = &binding;
    createInfo.systemId = systemId_;

    XrResult result = xrCreateSession(
        instance_, &createInfo, &session_);

    if (XR_FAILED(result))
    {
        status_ = "xrCreateSession failed: " +
            std::to_string(static_cast<int>(result));

        session_ = XR_NULL_HANDLE;
        return false;
    }

    XrReferenceSpaceCreateInfo spaceInfo{
        XR_TYPE_REFERENCE_SPACE_CREATE_INFO
    };

    spaceInfo.referenceSpaceType =
        XR_REFERENCE_SPACE_TYPE_LOCAL;

    spaceInfo.poseInReferenceSpace.orientation.w = 1.0f;

    result = xrCreateReferenceSpace(
        session_, &spaceInfo, &localSpace_);

    if (XR_FAILED(result))
    {
        status_ = "Reference space creation failed: " +
            std::to_string(static_cast<int>(result));

        DestroySession();
        return false;
    }

    status_ = "OpenXR Session Created Successfully";

    std::cout << "[XRRuntime] "
              << status_ << std::endl;

    return true;
}

void XRRuntime::DestroySession()
{
    if (localSpace_ != XR_NULL_HANDLE)
    {
        xrDestroySpace(localSpace_);
        localSpace_ = XR_NULL_HANDLE;
    }

    if (session_ != XR_NULL_HANDLE)
    {
        xrDestroySession(session_);
        session_ = XR_NULL_HANDLE;
    }
}

XrSession XRRuntime::GetSession() const
{
    return session_;
}
```

ရှိပြီးသား Shutdown() Function ကိုလည်း Update လုပ်ပါ။

```cpp
void XRRuntime::Shutdown()
{
    DestroySession();

    if (instance_ != XR_NULL_HANDLE)
    {
        xrDestroyInstance(instance_);
        instance_ = XR_NULL_HANDLE;
    }

    systemId_ = XR_NULL_SYSTEM_ID;
    initialized_ = false;
}
```

####################################

Command
```bash
cd ~/openxr_workspace

cp -a gazebo_openxr gazebo_openxr_phase2_2_backup

cd gazebo_openxr
```


####################################

Command
```bash
vim include/GazeboOpenXR.hh
```

```cpp
#pragma once

#include <QString>
#include <QEvent>
#include <atomic>

#include <gz/gui/Plugin.hh>

#include "XRRuntime.hh"

class GazeboOpenXR : public gz::gui::Plugin
{
    Q_OBJECT

    Q_PROPERTY(QString status
               READ Status
               NOTIFY StatusChanged)

public:
    GazeboOpenXR();
    ~GazeboOpenXR() override;

    void LoadConfig(
        const tinyxml2::XMLElement *_pluginElem
    ) override;

    QString Status() const;

    Q_INVOKABLE void CheckRuntime();
    Q_INVOKABLE void StartVR();
    Q_INVOKABLE void StopVR();

signals:
    void StatusChanged();

protected:
    bool eventFilter(
        QObject *_obj,
        QEvent *_event
    ) override;

private:
    XRRuntime runtime_;

    QString status_ = "OpenXR not initialized";

    std::atomic<bool> checkRequested_{false};
    std::atomic<bool> startRequested_{false};
    std::atomic<bool> stopRequested_{false};
    std::atomic<bool> sessionCreated_{false};

    void SetStatus(const QString &status);
    void PostStatus(const QString &status);
    void OnRender();
};
```


####################################

Command
```bash
vim src/GazeboOpenXR.cc
```

```cpp
#include "GazeboOpenXR.hh"

#include <iostream>
#include <QMetaObject>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/GuiEvents.hh>

#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";

    auto *mainWindow =
        gz::gui::App()->findChild<
            gz::gui::MainWindow *>();

    if (mainWindow)
    {
        mainWindow->installEventFilter(this);

        std::cout
            << "[GazeboOpenXR] Render event filter installed"
            << std::endl;
    }
}

QString GazeboOpenXR::Status() const
{
    return status_;
}

void GazeboOpenXR::SetStatus(
    const QString &status)
{
    status_ = status;

    emit StatusChanged();

    std::cout << "[GazeboOpenXR] "
              << status.toStdString()
              << std::endl;
}

void GazeboOpenXR::PostStatus(
    const QString &status)
{
    QMetaObject::invokeMethod(
        this,
        [this, status]()
        {
            SetStatus(status);
        },
        Qt::QueuedConnection
    );
}

void GazeboOpenXR::CheckRuntime()
{
    checkRequested_.store(true);
}

void GazeboOpenXR::StartVR()
{
    SetStatus("Requesting OpenXR session...");
    startRequested_.store(true);
}

void GazeboOpenXR::StopVR()
{
    stopRequested_.store(true);
}

bool GazeboOpenXR::eventFilter(
    QObject *_obj,
    QEvent *_event)
{
    if (_event->type() ==
        gz::gui::events::Render::kType)
    {
        OnRender();
    }

    return QObject::eventFilter(_obj, _event);
}

void GazeboOpenXR::OnRender()
{
    if (stopRequested_.exchange(false))
    {
        startRequested_.store(false);

        runtime_.DestroySession();

        sessionCreated_.store(false);

        PostStatus("OpenXR Session Stopped");

        return;
    }

    if (checkRequested_.exchange(false))
    {
        runtime_.Initialize();

        PostStatus(
            QString::fromStdString(
                runtime_.GetStatus()));
    }

    if (startRequested_.exchange(false))
    {
        if (sessionCreated_.load())
            return;

        std::cout
            << "[GazeboOpenXR] Start VR request received"
            << std::endl;

        if (!runtime_.Initialize())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        if (!runtime_.ProbeGraphicsContext())
        {
            PostStatus(
                "No current GLX context in render event");

            return;
        }

        if (!runtime_.CreateSessionFromCurrentContext())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        sessionCreated_.store(true);

        PostStatus(
            "OpenXR Session Created Successfully");
    }
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
```

####################################

Command
```bash
vim qml/GazeboOpenXR.qml
```

```qml
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Rectangle {
    anchors.fill: parent
    color: "#202631"

    implicitWidth: 380
    implicitHeight: 420

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: "Gazebo OpenXR"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        Label {
            text: "Meta Quest | WiVRn Runtime"
            color: "#a0aec0"
            font.pixelSize: 13
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#475569"
        }

        Label {
            text: GazeboOpenXR.status
            color: "#4ade80"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            text: "Check OpenXR Runtime"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.CheckRuntime()
        }

        Button {
            text: "Start VR Session"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.StartVR()
        }

        Button {
            text: "Stop VR Session"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.StopVR()
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
```

####################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

####################################

Command
```bash
ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```

####################################

Command
```bash
if [ -S "$XDG_RUNTIME_DIR/wivrn/comp_ipc" ]; then (17.8)
    echo "WiVRn IPC: READY"
```
```cpp
else
```
Command
```bash
    echo "WiVRn IPC: NOT READY"
```
Output
```text
fi
```

####################################

Command
```bash
cd ~/openxr_workspace/gazebo_openxr (17.9)

export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=all

gz sim --force-version 8 -v 4 shapes.sdf
```

####################################

18.3 
Command
```bash
vim include/XRRuntime.hh
```

public: အောက်မှာ ထပ်ထည့်ပါ။

```cpp
bool PollEvents();

bool RequestStop();

bool IsSessionRunning() const;

bool RenderFrame();

XrSpace GetLocalSpace() const;
```

private: အောက်မှာ ထပ်ထည့်ပါ။
```cpp
XrSessionState sessionState_ =
    XR_SESSION_STATE_UNKNOWN;

bool sessionRunning_ = false;

bool exitRequested_ = false;

uint64_t frameCount_ = 0;
```

Header File အပေါ်ဆုံးမှာလည်း ထည့်ပါ။
```cpp
#include <cstdint>
```

####################################

18.4

Command
```bash
vim src/XRRuntime.cc
```

```cpp
bool XRRuntime::PollEvents()
{
    if (instance_ == XR_NULL_HANDLE)
        return false;

    XrEventDataBuffer event{
        XR_TYPE_EVENT_DATA_BUFFER
    };

    while (true)
    {
        event = {};
        event.type = XR_TYPE_EVENT_DATA_BUFFER;

        XrResult result =
            xrPollEvent(instance_, &event);

        if (result == XR_EVENT_UNAVAILABLE)
            break;

        if (XR_FAILED(result))
        {
            status_ = "xrPollEvent failed: " +
                std::to_string(static_cast<int>(result));

            return false;
        }

        if (event.type ==
            XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING)
        {
            status_ = "OpenXR instance loss pending";
            Shutdown();
            return false;
        }

        if (event.type !=
            XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
        {
            continue;
        }

        const auto* stateEvent =
            reinterpret_cast<
                const XrEventDataSessionStateChanged*
            >(&event);

        if (stateEvent->session != session_)
            continue;

        sessionState_ = stateEvent->state;

        char stateName[XR_MAX_STRUCTURE_NAME_SIZE]{};

        xrStructureTypeToString(
            instance_,
            event.type,
            stateName);

        std::cout
            << "[XRRuntime] Session state: "
            << static_cast<int>(sessionState_)
            << std::endl;

        switch (sessionState_)
        {
            case XR_SESSION_STATE_READY:
            {
                XrSessionBeginInfo beginInfo{
                    XR_TYPE_SESSION_BEGIN_INFO
                };

                beginInfo.primaryViewConfigurationType =
                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

                result = xrBeginSession(
                    session_, &beginInfo);

                if (XR_FAILED(result))
                {
                    status_ = "xrBeginSession failed: " +
                        std::to_string(static_cast<int>(result));

                    return false;
                }

                sessionRunning_ = true;
                frameCount_ = 0;

                std::cout
                    << "[XRRuntime] Session started"
                    << std::endl;

                break;
            }

            case XR_SESSION_STATE_STOPPING:
            {
                if (sessionRunning_)
                {
                    result = xrEndSession(session_);

                    if (XR_FAILED(result))
                    {
                        status_ = "xrEndSession failed: " +
                            std::to_string(static_cast<int>(result));

                        return false;
                    }
                }

                sessionRunning_ = false;

                std::cout
                    << "[XRRuntime] Session ended"
                    << std::endl;

                break;
            }

            case XR_SESSION_STATE_EXITING:
            case XR_SESSION_STATE_LOSS_PENDING:
            {
                sessionRunning_ = false;
                DestroySession();

                status_ = "OpenXR session closed";
                return true;
            }

            default:
                break;
        }
    }

    if (exitRequested_ &&
        sessionState_ == XR_SESSION_STATE_IDLE &&
        !sessionRunning_)
    {
        DestroySession();
        status_ = "OpenXR Session Stopped";
    }

    return true;
}
```


##################################
18.5
```cpp
bool XRRuntime::RequestStop()
{
    if (session_ == XR_NULL_HANDLE)
        return true;

    if (exitRequested_)
        return true;

    if (!sessionRunning_)
    {
        DestroySession();
        return true;
    }

    XrResult result =
        xrRequestExitSession(session_);

    if (XR_FAILED(result))
    {
        status_ = "xrRequestExitSession failed: " +
            std::to_string(static_cast<int>(result));

        return false;
    }

    exitRequested_ = true;

    return true;
}

bool XRRuntime::IsSessionRunning() const
{
    return sessionRunning_;
}

XrSpace XRRuntime::GetLocalSpace() const
{
    return localSpace_;
}
```

####################################################

ရှိပြီးသား DestroySession() Function ကိုလည်း အောက်ပါအတိုင်း အစားထိုးပါ။
```cpp
void XRRuntime::DestroySession()
{
    if (localSpace_ != XR_NULL_HANDLE)
    {
        xrDestroySpace(localSpace_);
        localSpace_ = XR_NULL_HANDLE;
    }

    if (session_ != XR_NULL_HANDLE)
    {
        xrDestroySession(session_);
        session_ = XR_NULL_HANDLE;
    }

    sessionRunning_ = false;
    sessionState_ = XR_SESSION_STATE_UNKNOWN;

    exitRequested_ = false;
    frameCount_ = 0;
}
```

#######################################################

Output
```text
19.1
```
19.1. Frame Loop Implementation
src/XRRuntime.cc ထဲမှာ အောက်ပါ Function ကို ဆက်ထည့်ပါ။
```cpp
bool XRRuntime::RenderFrame()
{
    if (!sessionRunning_ ||
        session_ == XR_NULL_HANDLE)
    {
        return false;
    }

    XrFrameWaitInfo waitInfo{
        XR_TYPE_FRAME_WAIT_INFO
    };

    XrFrameState frameState{
        XR_TYPE_FRAME_STATE
    };

    XrResult result = xrWaitFrame(
        session_,
        &waitInfo,
        &frameState);

    if (XR_FAILED(result))
    {
        status_ = "xrWaitFrame failed: " +
            std::to_string(static_cast<int>(result));

        return false;
    }

    XrFrameBeginInfo beginInfo{
        XR_TYPE_FRAME_BEGIN_INFO
    };

    result = xrBeginFrame(
        session_, &beginInfo);

    if (XR_FAILED(result))
    {
        status_ = "xrBeginFrame failed: " +
            std::to_string(static_cast<int>(result));

        return false;
    }

    XrFrameEndInfo endInfo{
        XR_TYPE_FRAME_END_INFO
    };

    endInfo.displayTime =
        frameState.predictedDisplayTime;

    endInfo.environmentBlendMode =
        XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

    // Diagnostic frame without composition layers
    endInfo.layerCount = 0;
    endInfo.layers = nullptr;

    result = xrEndFrame(
        session_, &endInfo);

    if (XR_FAILED(result))
    {
        status_ = "xrEndFrame failed: " +
            std::to_string(static_cast<int>(result));

        return false;
    }

    ++frameCount_;

    if (frameCount_ % 120 == 0)
    {
        std::cout
            << "[XRRuntime] Frame: "
            << frameCount_
            << " | shouldRender: "
            << frameState.shouldRender
            << " | predictedDisplayTime: "
            << frameState.predictedDisplayTime
            << std::endl;
    }

    return true;
}

#########################################33

19.2
```

Command
```bash
vim src/GazeboOpenXR.cc
```

```cpp
void GazeboOpenXR::OnRender()
{
    if (stopRequested_.exchange(false))
    {
        startRequested_.store(false);

        if (runtime_.RequestStop())
        {
            PostStatus("Stopping OpenXR Session...");
        }
        else
        {
            PostStatus(QString::fromStdString(
                runtime_.GetStatus()));
        }
    }

    if (checkRequested_.exchange(false))
    {
        runtime_.Initialize();

        PostStatus(
            QString::fromStdString(
                runtime_.GetStatus()));
    }

    if (startRequested_.exchange(false))
    {
        if (!sessionCreated_.load())
        {
            if (!runtime_.Initialize())
            {
                PostStatus(QString::fromStdString(
                    runtime_.GetStatus()));

                return;
            }

            if (!runtime_.CreateSessionFromCurrentContext())
            {
                PostStatus(QString::fromStdString(
                    runtime_.GetStatus()));

                return;
            }

            sessionCreated_.store(true);

            PostStatus(
                "OpenXR Session Created");
        }
    }

    if (!sessionCreated_.load())
        return;

    if (runtime_.GetSession() == XR_NULL_HANDLE)
    {
        sessionCreated_.store(false);
        PostStatus("OpenXR Session Stopped");
        return;
    }

    bool wasRunning =
        runtime_.IsSessionRunning();

    if (!runtime_.PollEvents())
    {
        PostStatus(QString::fromStdString(
            runtime_.GetStatus()));

        runtime_.DestroySession();
        sessionCreated_.store(false);
        return;
    }

    if (runtime_.GetSession() == XR_NULL_HANDLE)
    {
        sessionCreated_.store(false);
        PostStatus("OpenXR Session Stopped");
        return;
    }

    if (runtime_.IsSessionRunning())
    {
        if (!wasRunning)
        {
            PostStatus("VR Frame Loop Running");
        }

        if (!runtime_.RenderFrame())
        {
            PostStatus(QString::fromStdString(
                runtime_.GetStatus()));

            runtime_.DestroySession();
            sessionCreated_.store(false);
        }
    }
}
```

###########################################################

19.4. Rebuild Plugin

Command
```bash
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

###########################################################

Command
```bash
ldd -r build/libGazeboOpenXR.so 2>&1 | \
grep -iE 'undefined|not found'
```

#############################################################

19.5. Launch Gazebo
Output
```text
mr_robot@desktop:~/openxr_workspace/openxr_gazebo_plugin/gazebo_openxr
```


Command
```bash
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f "$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```


#####################################################



Command
```bash
vim include/GazeboOpenXR.hh
```

```cpp
#pragma once

#include <QString>
#include <QEvent>
#include <atomic>

#include <gz/gui/Plugin.hh>

#include "XRRuntime.hh"

class GazeboOpenXR : public gz::gui::Plugin
{
    Q_OBJECT

    Q_PROPERTY(QString status
               READ Status
               NOTIFY StatusChanged)

public:
    GazeboOpenXR();
    ~GazeboOpenXR() override;

    void LoadConfig(
        const tinyxml2::XMLElement *_pluginElem
    ) override;

    QString Status() const;

    Q_INVOKABLE void CheckRuntime();
    Q_INVOKABLE void StartVR();
    Q_INVOKABLE void StopVR();

signals:
    void StatusChanged();

protected:
    bool eventFilter(
        QObject *_obj,
        QEvent *_event
    ) override;

private:
    XRRuntime runtime_;

    QString status_ = "OpenXR not initialized";

    std::atomic<bool> checkRequested_{false};
    std::atomic<bool> startRequested_{false};
    std::atomic<bool> stopRequested_{false};
    std::atomic<bool> sessionCreated_{false};

    void SetStatus(const QString &status);
    void PostStatus(const QString &status);
    void OnRender();
};
```

####################################

Command
```bash
vim src/GazeboOpenXR.cc
```

```cpp
#include "GazeboOpenXR.hh"

#include <iostream>
#include <QMetaObject>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/GuiEvents.hh>

#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";

    auto *mainWindow =
        gz::gui::App()->findChild<
            gz::gui::MainWindow *>();

    if (mainWindow)
    {
        mainWindow->installEventFilter(this);

        std::cout
            << "[GazeboOpenXR] Render event filter installed"
            << std::endl;
    }
}

QString GazeboOpenXR::Status() const
{
    return status_;
}

void GazeboOpenXR::SetStatus(
    const QString &status)
{
    status_ = status;

    emit StatusChanged();

    std::cout << "[GazeboOpenXR] "
              << status.toStdString()
              << std::endl;
}

void GazeboOpenXR::PostStatus(
    const QString &status)
{
    QMetaObject::invokeMethod(
        this,
        [this, status]()
        {
            SetStatus(status);
        },
        Qt::QueuedConnection
    );
}

void GazeboOpenXR::CheckRuntime()
{
    checkRequested_.store(true);
}

void GazeboOpenXR::StartVR()
{
    SetStatus("Requesting OpenXR session...");
    startRequested_.store(true);
}

void GazeboOpenXR::StopVR()
{
    stopRequested_.store(true);
}

bool GazeboOpenXR::eventFilter(
    QObject *_obj,
    QEvent *_event)
{
    if (_event->type() ==
        gz::gui::events::Render::kType)
    {
        OnRender();
    }

    return QObject::eventFilter(_obj, _event);
}

void GazeboOpenXR::OnRender()
{
    if (stopRequested_.exchange(false))
    {
        startRequested_.store(false);

        runtime_.DestroySession();

        sessionCreated_.store(false);

        PostStatus("OpenXR Session Stopped");

        return;
    }

    if (checkRequested_.exchange(false))
    {
        runtime_.Initialize();

        PostStatus(
            QString::fromStdString(
                runtime_.GetStatus()));
    }

    if (startRequested_.exchange(false))
    {
        if (sessionCreated_.load())
            return;

        std::cout
            << "[GazeboOpenXR] Start VR request received"
            << std::endl;

        if (!runtime_.Initialize())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        if (!runtime_.ProbeGraphicsContext())
        {
            PostStatus(
                "No current GLX context in render event");

            return;
        }

        if (!runtime_.CreateSessionFromCurrentContext())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        sessionCreated_.store(true);

        PostStatus(
            "OpenXR Session Created Successfully");
    }
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
```

####################################

Command
```bash
vim src/GazeboOpenXR.cc
```

```cpp
#include "GazeboOpenXR.hh"

#include <iostream>
#include <QMetaObject>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/GuiEvents.hh>

#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";

    auto *mainWindow =
        gz::gui::App()->findChild<
            gz::gui::MainWindow *>();

    if (mainWindow)
    {
        mainWindow->installEventFilter(this);

        std::cout
            << "[GazeboOpenXR] Render event filter installed"
            << std::endl;
    }
}

QString GazeboOpenXR::Status() const
{
    return status_;
}

void GazeboOpenXR::SetStatus(
    const QString &status)
{
    status_ = status;

    emit StatusChanged();

    std::cout << "[GazeboOpenXR] "
              << status.toStdString()
              << std::endl;
}

void GazeboOpenXR::PostStatus(
    const QString &status)
{
    QMetaObject::invokeMethod(
        this,
        [this, status]()
        {
            SetStatus(status);
        },
        Qt::QueuedConnection
    );
}

void GazeboOpenXR::CheckRuntime()
{
    checkRequested_.store(true);
}

void GazeboOpenXR::StartVR()
{
    SetStatus("Requesting OpenXR session...");
    startRequested_.store(true);
}

void GazeboOpenXR::StopVR()
{
    stopRequested_.store(true);
}

bool GazeboOpenXR::eventFilter(
    QObject *_obj,
    QEvent *_event)
{
    if (_event->type() ==
        gz::gui::events::Render::kType)
    {
        OnRender();
    }

    return QObject::eventFilter(_obj, _event);
}

void GazeboOpenXR::OnRender()
{
    if (stopRequested_.exchange(false))
    {
        startRequested_.store(false);

        runtime_.DestroySession();

        sessionCreated_.store(false);

        PostStatus("OpenXR Session Stopped");

        return;
    }

    if (checkRequested_.exchange(false))
    {
        runtime_.Initialize();

        PostStatus(
            QString::fromStdString(
                runtime_.GetStatus()));
    }

    if (startRequested_.exchange(false))
    {
        if (sessionCreated_.load())
            return;

        std::cout
            << "[GazeboOpenXR] Start VR request received"
            << std::endl;

        if (!runtime_.Initialize())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        if (!runtime_.ProbeGraphicsContext())
        {
            PostStatus(
                "No current GLX context in render event");

            return;
        }

        if (!runtime_.CreateSessionFromCurrentContext())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        sessionCreated_.store(true);

        PostStatus(
            "OpenXR Session Created Successfully");
    }
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
```

####################################

Command
```bash
vim src/GazeboOpenXR.cc
```

```cpp
#include "GazeboOpenXR.hh"

#include <iostream>
#include <QMetaObject>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/GuiEvents.hh>

#include <gz/plugin/Register.hh>

GazeboOpenXR::GazeboOpenXR() = default;

GazeboOpenXR::~GazeboOpenXR() = default;

void GazeboOpenXR::LoadConfig(
    const tinyxml2::XMLElement *)
{
    this->title = "Gazebo OpenXR";

    auto *mainWindow =
        gz::gui::App()->findChild<
            gz::gui::MainWindow *>();

    if (mainWindow)
    {
        mainWindow->installEventFilter(this);

        std::cout
            << "[GazeboOpenXR] Render event filter installed"
            << std::endl;
    }
}

QString GazeboOpenXR::Status() const
{
    return status_;
}

void GazeboOpenXR::SetStatus(
    const QString &status)
{
    status_ = status;

    emit StatusChanged();

    std::cout << "[GazeboOpenXR] "
              << status.toStdString()
              << std::endl;
}

void GazeboOpenXR::PostStatus(
    const QString &status)
{
    QMetaObject::invokeMethod(
        this,
        [this, status]()
        {
            SetStatus(status);
        },
        Qt::QueuedConnection
    );
}

void GazeboOpenXR::CheckRuntime()
{
    checkRequested_.store(true);
}

void GazeboOpenXR::StartVR()
{
    SetStatus("Requesting OpenXR session...");
    startRequested_.store(true);
}

void GazeboOpenXR::StopVR()
{
    stopRequested_.store(true);
}

bool GazeboOpenXR::eventFilter(
    QObject *_obj,
    QEvent *_event)
{
    if (_event->type() ==
        gz::gui::events::Render::kType)
    {
        OnRender();
    }

    return QObject::eventFilter(_obj, _event);
}

void GazeboOpenXR::OnRender()
{
    if (stopRequested_.exchange(false))
    {
        startRequested_.store(false);

        runtime_.DestroySession();

        sessionCreated_.store(false);

        PostStatus("OpenXR Session Stopped");

        return;
    }

    if (checkRequested_.exchange(false))
    {
        runtime_.Initialize();

        PostStatus(
            QString::fromStdString(
                runtime_.GetStatus()));
    }

    if (startRequested_.exchange(false))
    {
        if (sessionCreated_.load())
            return;

        std::cout
            << "[GazeboOpenXR] Start VR request received"
            << std::endl;

        if (!runtime_.Initialize())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        if (!runtime_.ProbeGraphicsContext())
        {
            PostStatus(
                "No current GLX context in render event");

            return;
        }

        if (!runtime_.CreateSessionFromCurrentContext())
        {
            PostStatus(
                QString::fromStdString(
                    runtime_.GetStatus()));

            return;
        }

        sessionCreated_.store(true);

        PostStatus(
            "OpenXR Session Created Successfully");
    }
}

GZ_ADD_PLUGIN(
    GazeboOpenXR,
    gz::gui::Plugin
)
```

####################################

Command
```bash
vim qml/GazeboOpenXR.qml
```

```qml
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Rectangle {
    anchors.fill: parent
    color: "#202631"

    implicitWidth: 380
    implicitHeight: 420

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: "Gazebo OpenXR"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        Label {
            text: "Meta Quest | WiVRn Runtime"
            color: "#a0aec0"
            font.pixelSize: 13
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#475569"
        }

        Label {
            text: GazeboOpenXR.status
            color: "#4ade80"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            text: "Check OpenXR Runtime"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.CheckRuntime()
        }

        Button {
            text: "Start VR Session"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.StartVR()
        }

        Button {
            text: "Stop VR Session"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.StopVR()
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
```

####################################
