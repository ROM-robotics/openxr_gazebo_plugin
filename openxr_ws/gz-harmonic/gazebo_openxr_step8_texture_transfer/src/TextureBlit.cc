#include "TextureBlit.hh"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>

#include <iostream>

namespace
{
bool CheckGlError(const char *operation)
{
    const GLenum error = glGetError();
    if (error == GL_NO_ERROR)
    {
        return true;
    }
    std::cerr << "[TextureBlit] OpenGL error during " << operation << ": 0x"
              << std::hex << error << std::dec << std::endl;
    return false;
}

bool CheckContext()
{
    if (!glXGetCurrentContext())
    {
        std::cerr << "[TextureBlit] No current GLX context" << std::endl;
        return false;
    }
    if (!CheckGlError("before blit"))
    {
        return false;
    }
    GLint major = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    if (!CheckGlError("querying OpenGL version") || major < 3)
    {
        std::cerr << "[TextureBlit] OpenGL 3.0 or later is required" << std::endl;
        return false;
    }
    return true;
}
}

bool texture_blit::BlitTexture(uint32_t source, uint32_t destination)
{
    if (!CheckContext())
    {
        return false;
    }
    if (!source || !destination || source == destination ||
        glIsTexture(source) != GL_TRUE || glIsTexture(destination) != GL_TRUE)
    {
        std::cerr << "[TextureBlit] Invalid source or destination texture" << std::endl;
        return false;
    }
    if (!CheckGlError("checking textures"))
    {
        return false;
    }

    GLint previousRead = 0;
    GLint previousDraw = 0;
    GLint previousTexture = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousRead);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousDraw);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
    const GLboolean scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
    if (!CheckGlError("saving OpenGL state"))
    {
        return false;
    }

    GLint widths[2]{};
    GLint heights[2]{};
    const GLuint textures[] = {source, destination};
    bool success = true;
    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        if (!CheckGlError("binding texture as GL_TEXTURE_2D"))
        {
            success = false;
            break;
        }
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &widths[i]);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &heights[i]);
        if (!CheckGlError("querying texture dimensions") ||
            widths[i] <= 0 || heights[i] <= 0)
        {
            std::cerr << "[TextureBlit] Invalid texture dimensions" << std::endl;
            success = false;
            break;
        }
    }

    GLuint readFbo = 0;
    GLuint drawFbo = 0;
    if (success)
    {
        glGenFramebuffers(1, &readFbo);
        glGenFramebuffers(1, &drawFbo);
        if (!CheckGlError("creating framebuffers") || !readFbo || !drawFbo)
        {
            std::cerr << "[TextureBlit] Could not create framebuffers" << std::endl;
            success = false;
        }
    }
    if (success)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, source, 0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        const GLenum status = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
        if (!CheckGlError("preparing read framebuffer") ||
            status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "[TextureBlit] Read framebuffer incomplete: 0x"
                      << std::hex << status << std::dec << std::endl;
            success = false;
        }
    }
    if (success)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, destination, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        const GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (!CheckGlError("preparing draw framebuffer") ||
            status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "[TextureBlit] Draw framebuffer incomplete: 0x"
                      << std::hex << status << std::dec << std::endl;
            success = false;
        }
    }
    if (success)
    {
        glDisable(GL_SCISSOR_TEST);
        glBlitFramebuffer(0, 0, widths[0], heights[0],
                          0, 0, widths[1], heights[1],
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);
        if (!CheckGlError("blitting framebuffer"))
        {
            success = false;
        }
        else
        {
            glFinish();
            success = CheckGlError("waiting for blit");
        }
    }

    if (scissorEnabled)
    {
        glEnable(GL_SCISSOR_TEST);
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(previousRead));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(previousDraw));
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));
    if (readFbo)
    {
        glDeleteFramebuffers(1, &readFbo);
    }
    if (drawFbo)
    {
        glDeleteFramebuffers(1, &drawFbo);
    }
    if (!CheckGlError("restoring state and deleting framebuffers"))
    {
        success = false;
    }
    return success;
}

bool texture_blit::TestFramebufferBlit(uint32_t source)
{
    if (!CheckContext())
    {
        return false;
    }
    GLint previousTexture = 0;
    GLint previousUnpackBuffer = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousUnpackBuffer);
    if (!CheckGlError("saving texture binding"))
    {
        return false;
    }

    GLuint destination = 0;
    glGenTextures(1, &destination);
    if (!CheckGlError("creating temporary texture") || !destination)
    {
        std::cerr << "[TextureBlit] Could not create temporary texture" << std::endl;
        if (destination)
        {
            glDeleteTextures(1, &destination);
            CheckGlError("deleting temporary texture");
        }
        return false;
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, destination);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    bool success = CheckGlError("allocating temporary texture");
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, static_cast<GLuint>(previousUnpackBuffer));
    if (!CheckGlError("restoring texture and unpack buffer bindings"))
    {
        success = false;
    }
    if (success)
    {
        success = BlitTexture(source, destination);
    }
    glDeleteTextures(1, &destination);
    if (!CheckGlError("deleting temporary texture"))
    {
        success = false;
    }
    if (success)
    {
        std::cout << "[XRCamera] Framebuffer Blit SUCCESS" << std::endl;
    }
    return success;
}
