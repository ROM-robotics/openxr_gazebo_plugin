#pragma once

#include <chrono>
#include <cstdint>
#include <gz/rendering/RenderTypes.hh>

class XRCamera
{
public:
    bool FindScene();
    bool CreateStereoCameras();
    bool RenderStereoDiagnostic();
    bool UpdateStereoFrame();
    bool ValidateStereoTextures();
    bool TestFramebufferBlit();
    bool StereoRenderAttempted() const;
    uint32_t GetLeftTextureId() const;
    uint32_t GetRightTextureId() const;
    std::uint64_t StereoFrameCount() const;
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
    uint32_t leftTextureId_ = 0;
    uint32_t rightTextureId_ = 0;
    std::chrono::steady_clock::time_point lastStereoRender_{};
    std::uint64_t stereoFrameCount_ = 0;
};
