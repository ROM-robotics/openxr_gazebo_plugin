#pragma once

#include <cstdint>
#include <gz/rendering/RenderTypes.hh>

class XRCamera
{
public:
    bool FindScene();
    bool CreateStereoCameras();
    bool RenderStereoDiagnostic();
    bool ValidateStereoTextures();
    bool TestFramebufferBlit();
    bool StereoRenderAttempted() const;
    uint32_t GetLeftTextureId() const;
    uint32_t GetRightTextureId() const;
    void Reset();

private:
    gz::rendering::ScenePtr scene_;
    gz::rendering::CameraPtr leftEye_;
    gz::rendering::CameraPtr rightEye_;
    bool sceneWaitReported_ = false;
    bool stereoRenderAttempted_ = false;
    bool stereoRenderReady_ = false;
    bool textureValidationAttempted_ = false;
    bool textureValidationReady_ = false;
    bool blitTestAttempted_ = false;
    bool blitTestReady_ = false;
};
