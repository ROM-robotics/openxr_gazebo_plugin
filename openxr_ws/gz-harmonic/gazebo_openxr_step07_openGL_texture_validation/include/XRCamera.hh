#pragma once

#include <gz/rendering/RenderTypes.hh>

class XRCamera
{
public:
    bool FindScene();
    bool CreateStereoCameras();
    bool RenderStereoDiagnostic();
    bool ValidateStereoTextures();
    bool StereoRenderAttempted() const;
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
};
