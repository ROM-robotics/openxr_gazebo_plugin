#pragma once

#include <gz/rendering/Scene.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderTypes.hh>

class XRCamera
{
public:
    bool FindScene();
    bool IsReady() const;

    bool CreateStereoCameras();

private:
    gz::rendering::ScenePtr scene_;

    gz::rendering::CameraPtr leftEye_;
    gz::rendering::CameraPtr rightEye_;

    bool reported_ = false;
    bool stereoReported_ = false;
};
