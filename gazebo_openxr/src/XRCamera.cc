#include "XRCamera.hh"

#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/Visual.hh>
#include <gz/math/Pose3.hh>
#include <gz/math/Angle.hh> 

#include <iostream>

bool XRCamera::FindScene()
{
    if (scene_ && scene_->IsInitialized() &&
        scene_->RootVisual())
    {
        return true;
    }

    scene_.reset();

    // Reuse the existing Gazebo rendering scene.
    // Do not create a separate rendering engine.
    auto engines = gz::rendering::loadedEngines();

    if (engines.empty())
        return false;

    if (engines.size() != 1)
        return false;

    auto *engine = gz::rendering::engine(engines[0]);

    if (!engine || engine->SceneCount() != 1)
        return false;

    auto candidate = engine->SceneByIndex(0);

    if (!candidate || !candidate->IsInitialized() ||
        !candidate->RootVisual())
    {
        return false;
    }

    scene_ = candidate;

    if (!reported_)
    {
        std::cout
            << "[XRCamera] Gazebo Scene Ready\n"
            << "Rendering Engine: "
            << engines[0]
            << "\nScene Name: "
            << scene_->Name()
            << "\nRoot Visual: Available"
            << std::endl;

        reported_ = true;
    }

    return true;
}

bool XRCamera::IsReady() const
{
    return scene_ &&
           scene_->IsInitialized() &&
           scene_->RootVisual() != nullptr;
}

bool XRCamera::CreateStereoCameras()
{
    if (!IsReady())
        return false;

    if (leftEye_ && rightEye_)
        return true;

    // Diagnostic resolution only.
    // Replace with OpenXR recommended sizes later.
    constexpr unsigned int width = 1024;
    constexpr unsigned int height = 1024;

    // Temporary diagnostic IPD (64 mm).
    constexpr double halfIPD = 0.032;

    auto createEye =
        [&](const std::string &name, double y)
        -> gz::rendering::CameraPtr
    {
        auto camera = scene_->CreateCamera(name);

        if (!camera)
            return nullptr;

        camera->SetImageWidth(width);
        camera->SetImageHeight(height);
        camera->SetAspectRatio(
            static_cast<double>(width) / height);

        camera->SetHFOV(gz::math::Angle(1.57));
        camera->SetNearClipPlane(0.05);
        camera->SetFarClipPlane(1000.0);

        camera->SetLocalPose(
            gz::math::Pose3d(0, y, 1.6, 0, 0, 0));

        scene_->RootVisual()->AddChild(camera);

        return camera;
    };

    if (!leftEye_)
        leftEye_ = createEye("xr_left_eye", halfIPD);

    if (!rightEye_)
        rightEye_ = createEye("xr_right_eye", -halfIPD);

    if (!leftEye_ || !rightEye_)
    {
        std::cerr
            << "[XRCamera] Stereo camera creation failed"
            << std::endl;

        return false;
    }

    if (!stereoReported_)
    {
        std::cout
            << "[XRCamera] Stereo Cameras Created\n"
            << "Left: " << leftEye_->Name() << "\n"
            << "Right: " << rightEye_->Name() << "\n"
            << "Resolution: "
            << width << " x " << height
            << " per eye\n"
            << "Diagnostic IPD: 0.064 m"
            << std::endl;

        stereoReported_ = true;
    }

    return true;
}

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

bool XRCamera::IsStereoRenderReady() const
{
    return stereoRenderReady_;
}