#include "XRCamera.hh"

#include <iostream>
#include <memory>
#include <thread>

#include <X11/Xlib.h>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/Image.hh>
#include <gz/rendering/Material.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/rendering/Visual.hh>

namespace
{
bool CheckImage(const gz::rendering::CameraPtr &camera, unsigned int channel)
{
    auto image = camera->CreateImage();
    camera->Copy(image);
    if (!image.Data() || image.Width() == 0 || image.Height() == 0 ||
        image.Depth() != 3 || image.MemorySize() < 3)
    {
        std::cerr << "[TextureDiagnostic] Invalid captured image" << std::endl;
        return false;
    }

    const auto *pixels = image.Data<unsigned char>();
    const auto offset = (image.Width() * (image.Height() / 10) +
        image.Width() / 10) * image.Depth();
    if (pixels[offset + channel] < 128 ||
        pixels[offset + (channel == 0 ? 1 : 0)] > 64)
    {
        std::cerr << "[TextureDiagnostic] Unexpected pixel for "
                  << camera->Name() << std::endl;
        return false;
    }
    const auto center = (image.Width() * (image.Height() / 2) +
        image.Width() / 2) * image.Depth();
    if (pixels[center + 2] < 128)
    {
        std::cerr << "[TextureDiagnostic] Test box not visible in "
                  << camera->Name() << std::endl;
        return false;
    }
    return true;
}

bool RenderDiagnostic(const gz::rendering::ScenePtr &scene)
{
    auto visual = scene->CreateVisual("diagnostic_box");
    auto box = scene->CreateBox();
    auto material = scene->CreateMaterial("diagnostic_blue");
    if (!visual || !box || !material)
    {
        std::cerr << "[TextureDiagnostic] Unable to create test geometry"
                  << std::endl;
        return false;
    }
    material->SetDiffuse(0.0, 0.0, 1.0);
    material->SetEmissive(0.0, 0.0, 1.0);
    visual->AddGeometry(box);
    visual->SetMaterial(material);
    visual->SetLocalPosition(3.0, 0.0, 1.6);
    scene->RootVisual()->AddChild(visual);

    XRCamera cameras;
    if (!cameras.FindScene() || !cameras.CreateStereoCameras())
    {
        std::cerr << "[TextureDiagnostic] Stereo cameras unavailable" << std::endl;
        return false;
    }

    auto left = std::dynamic_pointer_cast<gz::rendering::Camera>(
        scene->SensorByName("xr_left_eye"));
    auto right = std::dynamic_pointer_cast<gz::rendering::Camera>(
        scene->SensorByName("xr_right_eye"));
    if (!left || !right)
    {
        std::cerr << "[TextureDiagnostic] Stereo camera lookup failed" << std::endl;
        cameras.Reset();
        return false;
    }
    left->SetImageFormat(gz::rendering::PF_R8G8B8);
    right->SetImageFormat(gz::rendering::PF_R8G8B8);
    scene->SetCameraPassCountPerGpuFlush(1);

    scene->SetBackgroundColor(1.0, 0.0, 0.0);
    bool success = cameras.RenderStereoDiagnostic() &&
        cameras.ValidateStereoTextures() &&
        CheckImage(left, 0) && CheckImage(right, 0);

    scene->SetBackgroundColor(0.0, 1.0, 0.0);
    const auto start = std::chrono::steady_clock::now();
    while (success && cameras.StereoFrameCount() < 15 &&
           std::chrono::steady_clock::now() - start < std::chrono::seconds(10))
    {
        success = cameras.UpdateStereoFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    const auto elapsed = std::chrono::steady_clock::now() - start;
    const auto maxUpdates = 1 + std::chrono::duration_cast<std::chrono::milliseconds>(
        elapsed).count() / 100;
    if (cameras.StereoFrameCount() < 15 ||
        cameras.StereoFrameCount() > static_cast<std::uint64_t>(maxUpdates))
    {
        std::cerr << "[TextureDiagnostic] Continuous update count outside 10 FPS limit"
                  << std::endl;
        success = false;
    }
    success = success && CheckImage(left, 1) && CheckImage(right, 1);

    left.reset();
    right.reset();
    cameras.Reset();
    if (cameras.StereoFrameCount() != 0 || cameras.GetLeftTextureId() ||
        cameras.GetRightTextureId() || cameras.UpdateStereoFrame())
    {
        std::cerr << "[TextureDiagnostic] Camera reset failed" << std::endl;
        success = false;
    }
    return success;
}
}

int main()
{
    Display *display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "[TextureDiagnostic] An accessible X11 display is required"
                  << std::endl;
        return 77;
    }
    XCloseDisplay(display);

    auto *engine = gz::rendering::engine("ogre2");
    if (!engine)
    {
        std::cerr << "[TextureDiagnostic] Unable to load OGRE2" << std::endl;
        return 1;
    }
    auto scene = engine->CreateScene("texture_diagnostic");
    if (!scene || !scene->IsInitialized() || !scene->RootVisual())
    {
        std::cerr << "[TextureDiagnostic] Unable to create scene" << std::endl;
        if (scene)
        {
            engine->DestroyScene(scene);
        }
        gz::rendering::unloadEngine("ogre2");
        return 1;
    }

    const bool success = RenderDiagnostic(scene);
    engine->DestroyScene(scene);
    scene.reset();
    if (!gz::rendering::unloadEngine("ogre2"))
    {
        std::cerr << "[TextureDiagnostic] Unable to unload OGRE2" << std::endl;
        return 1;
    }
    if (!success)
    {
        return 1;
    }
    std::cout << "[TextureDiagnostic] Stereo texture rendering verified"
              << std::endl;
    return 0;
}
