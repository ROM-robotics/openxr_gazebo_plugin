#pragma once

#include <gz/rendering/Scene.hh>
#include <gz/rendering/RenderTypes.hh>

class XRCamera
{
public:
    bool FindScene();

    bool IsReady() const;

private:
    gz::rendering::ScenePtr scene_;

    bool reported_ = false;
};
