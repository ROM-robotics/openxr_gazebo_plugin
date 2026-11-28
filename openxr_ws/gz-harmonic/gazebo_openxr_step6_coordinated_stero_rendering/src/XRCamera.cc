#include "XRCamera.hh"

#include <iostream>

#include <gz/math/Angle.hh>
#include <gz/math/Pose3.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/rendering/Visual.hh>

bool XRCamera::FindScene()
{
    if (scene_ && scene_->IsInitialized() && scene_->RootVisual())
    {
        return true;
    }

    if (scene_)
    {
        Reset();
    }

    const auto engines = gz::rendering::loadedEngines();
    if (engines.size() == 1)
    {
        auto *engine = gz::rendering::engine(engines[0]);
        if (engine && engine->SceneCount() == 1)
        {
            auto candidate = engine->SceneByIndex(0);
            if (candidate && candidate->IsInitialized() &&
                candidate->RootVisual())
            {
                scene_ = candidate;
                sceneWaitReported_ = false;
                std::cout << "[XRCamera] Gazebo Scene Ready\n"
                          << "Rendering Engine: " << engines[0]
                          << "\nScene Name: " << scene_->Name()
                          << "\nRoot Visual: Available" << std::endl;
                return true;
            }
        }
    }

    if (!sceneWaitReported_)
    {
        std::cerr << "[XRCamera] Waiting for exactly one initialized "
                     "render engine and scene" << std::endl;
        sceneWaitReported_ = true;
    }
    return false;
}

bool XRCamera::CreateStereoCameras()
{
    if (!scene_ || !scene_->IsInitialized() || !scene_->RootVisual())
    {
        std::cerr << "[XRCamera] Scene is not ready" << std::endl;
        return false;
    }
    if (leftEye_ && rightEye_)
    {
        return true;
    }

    constexpr unsigned int width = 1024;
    constexpr unsigned int height = 1024;
    constexpr double halfIPD = 0.032;

    const auto createEye = [&](const std::string &name, double y)
    {
        auto camera = scene_->CreateCamera(name);
        if (camera)
        {
            camera->SetImageWidth(width);
            camera->SetImageHeight(height);
            camera->SetAspectRatio(static_cast<double>(width) / height);
            camera->SetHFOV(gz::math::Angle(1.57));
            camera->SetNearClipPlane(0.05);
            camera->SetFarClipPlane(1000.0);
            camera->SetLocalPose(gz::math::Pose3d(0, y, 1.6, 0, 0, 0));
            scene_->RootVisual()->AddChild(camera);
        }
        return camera;
    };

    leftEye_ = createEye("xr_left_eye", halfIPD);
    rightEye_ = createEye("xr_right_eye", -halfIPD);
    if (!leftEye_ || !rightEye_)
    {
        std::cerr << "[XRCamera] Stereo camera creation failed" << std::endl;
        Reset();
        return false;
    }

    std::cout << "[XRCamera] Stereo Cameras Created\n"
              << "Left: " << leftEye_->Name()
              << "\nRight: " << rightEye_->Name()
              << "\nResolution: " << width << " x " << height
              << " per eye\nDiagnostic IPD: 0.064 m" << std::endl;
    return true;
}

bool XRCamera::RenderStereoDiagnostic()
{
    if (stereoRenderReady_)
    {
        return true;
    }
    if (stereoRenderAttempted_)
    {
        return false;
    }
    if (!scene_ || !scene_->IsInitialized() || !leftEye_ || !rightEye_)
    {
        std::cerr << "[XRCamera] Scene or stereo cameras are not ready" << std::endl;
        return false;
    }

    stereoRenderAttempted_ = true;
    std::cout << "[XRCamera] Starting coordinated stereo render" << std::endl;
    scene_->PreRender();
    leftEye_->Render();
    rightEye_->Render();
    leftEye_->PostRender();
    rightEye_->PostRender();
    scene_->PostRender();

    const auto leftTexture = leftEye_->RenderTextureGLId();
    const auto rightTexture = rightEye_->RenderTextureGLId();
    std::cout << "[XRCamera] Left texture ID: " << leftTexture
              << "\n[XRCamera] Right texture ID: " << rightTexture
              << std::endl;
    if (!leftTexture || !rightTexture || leftTexture == rightTexture)
    {
        std::cerr << "[XRCamera] Invalid stereo render textures" << std::endl;
        return false;
    }
    stereoRenderReady_ = true;
    std::cout << "[XRCamera] Coordinated Stereo Render SUCCESS" << std::endl;
    return true;
}

bool XRCamera::StereoRenderAttempted() const
{
    return stereoRenderAttempted_;
}

void XRCamera::Reset()
{
    if (scene_ && scene_->IsInitialized())
    {
        if (leftEye_)
        {
            scene_->DestroySensor(leftEye_);
        }
        if (rightEye_)
        {
            scene_->DestroySensor(rightEye_);
        }
    }

    leftEye_.reset();
    rightEye_.reset();
    scene_.reset();
    sceneWaitReported_ = false;
    stereoRenderAttempted_ = false;
    stereoRenderReady_ = false;
}
