#pragma once

class XRRenderer
{
public:
    // Copy a Gazebo OpenGL texture into an
    // acquired OpenXR swapchain texture.
    //
    // Must be called on the rendering thread
    // with a valid, current OpenGL context.

    static bool BlitTexture(
        unsigned int sourceTexture,
        unsigned int destinationTexture,
        int sourceWidth,
        int sourceHeight,
        int destinationWidth,
        int destinationHeight);
};