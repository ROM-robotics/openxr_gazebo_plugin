#include <GL/glew.h>
#include <GL/glx.h>

#include "XRRenderer.hh"

#include <iostream>

namespace
{
bool EnsureGLEWInitialized()
{
    const GLXContext currentContext =
        glXGetCurrentContext();

    if (!currentContext)
    {
        std::cerr
            << "[XRRenderer] No current GLX context\n";
        return false;
    }

    // Diagnostic: track the last initialized GL context
    // on this rendering thread.
    static thread_local GLXContext initializedContext =
        nullptr;

    if (initializedContext == currentContext)
        return true;

    glewExperimental = GL_TRUE;

    const GLenum result = glewInit();

    if (result != GLEW_OK)
    {
        std::cerr
            << "[XRRenderer] GLEW initialization failed: "
            << reinterpret_cast<const char*>(
                   glewGetErrorString(result))
            << std::endl;

        return false;
    }

    // Clear a possible GLEW initialization error.
    (void)glGetError();

    if (!glGenFramebuffers ||
        !glBindFramebuffer ||
        !glFramebufferTexture2D ||
        !glCheckFramebufferStatus ||
        !glBlitFramebuffer ||
        !glDeleteFramebuffers)
    {
        std::cerr
            << "[XRRenderer] Required FBO functions missing\n";

        return false;
    }

    initializedContext = currentContext;

    std::cout
        << "[XRRenderer] GLEW Initialized: "
        << reinterpret_cast<const char*>(
               glewGetString(GLEW_VERSION))
        << std::endl;

    std::cout
        << "[XRRenderer] OpenGL FBO functions ready"
        << std::endl;

    return true;
}
}


bool XRRenderer::BlitTexture(
    unsigned int sourceTexture,
    unsigned int destinationTexture,
    int sourceWidth,
    int sourceHeight,
    int destinationWidth,
    int destinationHeight)
{   
    
    if (!EnsureGLEWInitialized())
    {
        std::cerr
            << "[XRRenderer] OpenGL initialization failed\n";
        return false;
    }

    if (!glXGetCurrentContext())
    {
        std::cerr
            << "[XRRenderer] No current GLX context\n";
        return false;
    }

    if (!sourceTexture || !destinationTexture ||
        sourceWidth <= 0 || sourceHeight <= 0 ||
        destinationWidth <= 0 || destinationHeight <= 0)
    {
        std::cerr
            << "[XRRenderer] Invalid texture or dimensions\n";
        return false;
    }

    if (glIsTexture(sourceTexture) != GL_TRUE ||
        glIsTexture(destinationTexture) != GL_TRUE)
    {
        std::cerr
            << "[XRRenderer] Texture is not valid in this GL context\n";
        return false;
    }

    // GLEW must already be initialized.
    if (!glGenFramebuffers ||
        !glBindFramebuffer ||
        !glFramebufferTexture2D ||
        !glCheckFramebufferStatus ||
        !glBlitFramebuffer ||
        !glDeleteFramebuffers)
    {
        std::cerr
            << "[XRRenderer] Missing framebuffer functions\n";
        return false;
    }

    GLint previousReadFbo = 0;
    GLint previousDrawFbo = 0;
    GLint previousReadBuffer = 0;
    GLint previousDrawBuffer = 0;

    glGetIntegerv(
        GL_READ_FRAMEBUFFER_BINDING,
        &previousReadFbo);

    glGetIntegerv(
        GL_DRAW_FRAMEBUFFER_BINDING,
        &previousDrawFbo);

    glGetIntegerv(GL_READ_BUFFER, &previousReadBuffer);
    glGetIntegerv(GL_DRAW_BUFFER, &previousDrawBuffer);

    const GLboolean scissorEnabled =
        glIsEnabled(GL_SCISSOR_TEST);

    const GLboolean srgbEnabled =
        glIsEnabled(GL_FRAMEBUFFER_SRGB);

    GLuint sourceFbo = 0;
    GLuint destinationFbo = 0;

    bool success = false;

    glGenFramebuffers(1, &sourceFbo);
    glGenFramebuffers(1, &destinationFbo);

    if (!sourceFbo || !destinationFbo)
        goto cleanup;

    // Gazebo camera texture -> READ FBO
    glBindFramebuffer(
        GL_READ_FRAMEBUFFER, sourceFbo);

    glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        sourceTexture,
        0);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(
            GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr
            << "[XRRenderer] Source FBO incomplete\n";
        goto cleanup;
    }

    // OpenXR swapchain texture -> DRAW FBO
    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER, destinationFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        destinationTexture,
        0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(
            GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr
            << "[XRRenderer] Destination FBO incomplete\n";
        goto cleanup;
    }

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_FRAMEBUFFER_SRGB);

    glBlitFramebuffer(
        0, 0, sourceWidth, sourceHeight,
        0, 0, destinationWidth, destinationHeight,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR);

    if (const GLenum error = glGetError();
        error != GL_NO_ERROR)
    {
        std::cerr
            << "[XRRenderer] glBlitFramebuffer failed: 0x"
            << std::hex << error << std::dec
            << std::endl;
        goto cleanup;
    }

    success = true;

cleanup:
    // Restore Gazebo's framebuffer bindings.
    glBindFramebuffer(
        GL_READ_FRAMEBUFFER, previousReadFbo);

    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER, previousDrawFbo);

    glReadBuffer(previousReadBuffer);
    glDrawBuffer(previousDrawBuffer);

    if (scissorEnabled)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

    if (srgbEnabled)
        glEnable(GL_FRAMEBUFFER_SRGB);
    else
        glDisable(GL_FRAMEBUFFER_SRGB);

    if (sourceFbo)
        glDeleteFramebuffers(1, &sourceFbo);

    if (destinationFbo)
        glDeleteFramebuffers(1, &destinationFbo);

    return success;
}