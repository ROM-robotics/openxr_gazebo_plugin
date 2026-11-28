#include "XRCamera.hh"
#include "TextureBlit.hh"
#include "TextureValidation.hh"

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

    leftTextureId_ = leftEye_->RenderTextureGLId();
    rightTextureId_ = rightEye_->RenderTextureGLId();
    std::cout << "[XRCamera] Left texture ID: " << leftTextureId_
              << "\n[XRCamera] Right texture ID: " << rightTextureId_
              << std::endl;
    if (!leftTextureId_ || !rightTextureId_ || leftTextureId_ == rightTextureId_)
    {
        std::cerr << "[XRCamera] Invalid stereo render textures" << std::endl;
        return false;
    }
    stereoRenderReady_ = true;
    lastStereoRender_ = std::chrono::steady_clock::now();
    std::cout << "[XRCamera] Coordinated Stereo Render SUCCESS" << std::endl;
    return true;
}

bool XRCamera::UpdateStereoFrame()
{
    if (!stereoRenderReady_ || !scene_ || !scene_->IsInitialized() ||
        !leftEye_ || !rightEye_)
    {
        std::cerr << "[XRCamera] Stereo frame update is not ready" << std::endl;
        return false;
    }

    using Clock = std::chrono::steady_clock;
    constexpr auto interval = std::chrono::milliseconds(100);
    if (Clock::now() - lastStereoRender_ < interval)
    {
        return true;
    }

    scene_->PreRender();
    leftEye_->Render();
    rightEye_->Render();
    leftEye_->PostRender();
    rightEye_->PostRender();
    scene_->PostRender();
    lastStereoRender_ = Clock::now();

    const uint32_t left = leftEye_->RenderTextureGLId();
    const uint32_t right = rightEye_->RenderTextureGLId();
    if (!left || !right || left == right)
    {
        textureValidationReady_ = false;
        std::cerr << "[XRCamera] Invalid updated stereo textures" << std::endl;
        return false;
    }
    if (left != leftTextureId_ || right != rightTextureId_)
    {
        leftTextureId_ = left;
        rightTextureId_ = right;
        textureValidationAttempted_ = false;
        textureValidationReady_ = false;
        if (!ValidateStereoTextures())
        {
            std::cerr << "[XRCamera] Updated stereo texture validation failed"
                      << std::endl;
            return false;
        }
    }
    ++stereoFrameCount_;
    if (stereoFrameCount_ % 60 == 0)
    {
        std::cout << "[XRCamera] Continuous Stereo Frame: " << stereoFrameCount_
                  << " | Left: " << leftTextureId_
                  << " | Right: " << rightTextureId_ << std::endl;
    }
    return true;
}

bool XRCamera::StereoRenderAttempted() const
{
    return stereoRenderAttempted_;
}

uint32_t XRCamera::GetLeftTextureId() const
{
    return textureValidationReady_ ? leftTextureId_ : 0;
}

uint32_t XRCamera::GetRightTextureId() const
{
    return textureValidationReady_ ? rightTextureId_ : 0;
}

std::uint64_t XRCamera::StereoFrameCount() const
{
    return stereoFrameCount_;
}

bool XRCamera::ValidateStereoTextures()
{
    if (!stereoRenderReady_ || !leftEye_ || !rightEye_)
    {
        std::cerr << "[XRCamera] Stereo textures are not ready" << std::endl;
        return false;
    }
    if (!textureValidationAttempted_)
    {
        textureValidationAttempted_ = true;
        textureValidationReady_ = texture_validation::ValidateStereoTextures(
            leftTextureId_, rightTextureId_);
    }
    return textureValidationReady_;
}

bool XRCamera::TestFramebufferBlit()
{
    if (!textureValidationReady_ || !leftEye_)
    {
        std::cerr << "[XRCamera] Stereo texture validation is not ready" << std::endl;
        return false;
    }
    if (!blitTestAttempted_)
    {
        blitTestAttempted_ = true;
        blitTestReady_ =
            texture_blit::TestFramebufferBlit(leftTextureId_);
    }
    return blitTestReady_;
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
    textureValidationAttempted_ = false;
    textureValidationReady_ = false;
    blitTestAttempted_ = false;
    blitTestReady_ = false;
    leftTextureId_ = 0;
    rightTextureId_ = 0;
    lastStereoRender_ = {};
    stereoFrameCount_ = 0;
}
