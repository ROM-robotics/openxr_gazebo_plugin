#pragma once

#include <chrono>
#include <cstdint>
#include <gz/rendering/Scene.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderTypes.hh>


class XRCamera
{
public:
    bool FindScene();
    bool IsReady() const;

    bool CreateStereoCameras();

    bool RenderStereoDiagnostic();

    bool IsStereoRenderReady() const;

    bool ValidateStereoTextures();

    bool TestFramebufferBlit();

    unsigned int GetLeftTextureId() const;
    
    unsigned int GetRightTextureId() const;

    bool UpdateStereoFrame();

private:
    gz::rendering::ScenePtr scene_;

    gz::rendering::CameraPtr leftEye_;
    gz::rendering::CameraPtr rightEye_;

    bool reported_ = false;
    bool stereoReported_ = false;

    bool stereoRenderReady_ = false;

    bool renderAttempted_ = false;

    unsigned int leftTextureId_ = 0;
    
    unsigned int rightTextureId_ = 0;

    bool textureValidationReported_ = false;

    bool blitTestAttempted_ = false;

    std::chrono::steady_clock::time_point lastStereoRender_{};

    std::uint64_t stereoFrameCount_ = 0;
};
