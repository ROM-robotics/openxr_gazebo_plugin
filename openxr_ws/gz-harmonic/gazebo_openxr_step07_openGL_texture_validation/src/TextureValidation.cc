#include "TextureValidation.hh"

#include <GL/gl.h>

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
    std::cerr << "[XRCamera] OpenGL error during " << operation << ": 0x"
              << std::hex << error << std::dec << std::endl;
    return false;
}
}

bool texture_validation::ValidateStereoTextures(
    uint32_t leftTexture, uint32_t rightTexture)
{
    if (!glGetString(GL_VERSION))
    {
        std::cerr << "[XRCamera] No current OpenGL context" << std::endl;
        return false;
    }
    if (!CheckGlError("before texture validation"))
    {
        return false;
    }

    GLint previousBinding = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBinding);
    if (!CheckGlError("reading texture binding"))
    {
        return false;
    }

    const GLuint textures[] = {leftTexture, rightTexture};
    const char *names[] = {"Left Eye", "Right Eye"};
    bool valid = true;
    for (int eye = 0; eye < 2; ++eye)
    {
        const GLboolean isTexture = glIsTexture(textures[eye]);
        if (!CheckGlError("checking texture") || !textures[eye] ||
            isTexture != GL_TRUE)
        {
            std::cerr << "[XRCamera] Invalid texture: " << names[eye]
                      << " (" << textures[eye] << ")" << std::endl;
            valid = false;
            break;
        }

        glBindTexture(GL_TEXTURE_2D, textures[eye]);
        if (!CheckGlError("binding texture as GL_TEXTURE_2D"))
        {
            valid = false;
            break;
        }

        GLint width = 0;
        GLint height = 0;
        GLint format = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
        if (!CheckGlError("querying texture dimensions and format"))
        {
            valid = false;
            break;
        }
        std::cout << "[XRCamera] " << names[eye] << " | Texture: "
                  << textures[eye] << " | Size: " << width << "x" << height
                  << " | Format: 0x" << std::hex << format << std::dec
                  << std::endl;
        if (width <= 0 || height <= 0 || format <= 0)
        {
            std::cerr << "[XRCamera] Invalid texture dimensions or format: "
                      << names[eye] << std::endl;
            valid = false;
            break;
        }
    }

    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousBinding));
    if (!CheckGlError("restoring texture binding"))
    {
        valid = false;
    }
    if (valid)
    {
        std::cout << "[XRCamera] Stereo Textures Validated" << std::endl;
    }
    return valid;
}
