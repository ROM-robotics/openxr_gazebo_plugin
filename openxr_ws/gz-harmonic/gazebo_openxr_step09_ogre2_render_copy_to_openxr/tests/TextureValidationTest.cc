#include "TextureValidation.hh"
#include "TextureBlit.hh"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>
#include <sstream>
#include <vector>

#define REQUIRE(condition) \
    do { if (!(condition)) { \
        std::cerr << "Failed at line " << __LINE__ << ": " #condition << '\n'; \
        return 1; \
    } } while (false)

int main()
{
    Display *display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "An accessible X11 display is required" << std::endl;
        return 77;
    }

    int attributes[] = {GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8,
                        GLX_BLUE_SIZE, 8, None};
    XVisualInfo *visual = glXChooseVisual(display, DefaultScreen(display), attributes);
    REQUIRE(visual);
    Colormap colormap = XCreateColormap(
        display, RootWindow(display, visual->screen), visual->visual, AllocNone);
    XSetWindowAttributes windowAttributes{};
    windowAttributes.colormap = colormap;
    Window window = XCreateWindow(
        display, RootWindow(display, visual->screen), 0, 0, 16, 16, 0,
        visual->depth, InputOutput, visual->visual, CWColormap, &windowAttributes);
    REQUIRE(window);
    GLXContext context = glXCreateContext(display, visual, nullptr, True);
    REQUIRE(context && glXMakeCurrent(display, window, context));

    GLuint textures[5]{};
    glGenTextures(5, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 16, 8, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 32, 4, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures[4]);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    REQUIRE(glGetError() == GL_NO_ERROR);

    const auto bindingPreserved = [&]
    {
        GLint binding = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &binding);
        return binding == static_cast<GLint>(textures[2]) &&
            glGetError() == GL_NO_ERROR;
    };

    std::ostringstream logs;
    auto *oldBuffer = std::cout.rdbuf(logs.rdbuf());
    const bool success =
        texture_validation::ValidateStereoTextures(textures[0], textures[1]);
    std::cout.rdbuf(oldBuffer);
    REQUIRE(success);
    REQUIRE(logs.str().find("Size: 16x8 | Format: 0x8058") != std::string::npos);
    REQUIRE(logs.str().find("Size: 32x4 | Format: 0x8058") != std::string::npos);
    REQUIRE(bindingPreserved());

    REQUIRE(!texture_validation::ValidateStereoTextures(textures[0], 0));
    REQUIRE(bindingPreserved());
    GLuint deletedTexture = 0;
    glGenTextures(1, &deletedTexture);
    glBindTexture(GL_TEXTURE_2D, deletedTexture);
    glDeleteTextures(1, &deletedTexture);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    REQUIRE(!texture_validation::ValidateStereoTextures(textures[0], deletedTexture));
    REQUIRE(bindingPreserved());
    REQUIRE(!texture_validation::ValidateStereoTextures(textures[0], textures[3]));
    REQUIRE(bindingPreserved());
    REQUIRE(!texture_validation::ValidateStereoTextures(textures[0], textures[4]));
    REQUIRE(bindingPreserved());
    glBindTexture(0xffff, 0);
    REQUIRE(!texture_validation::ValidateStereoTextures(textures[0], textures[1]));
    REQUIRE(bindingPreserved());

    std::vector<unsigned char> source(16 * 8 * 4);
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 16; ++x)
        {
            const auto offset = (y * 16 + x) * 4;
            source[offset] = x < 8 ? 128 : 0;
            source[offset + 1] = x < 8 ? 0 : 255;
            source[offset + 3] = 255;
        }
    }
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 8,
                    GL_RGBA, GL_UNSIGNED_BYTE, source.data());
    std::vector<unsigned char> destination(32 * 4 * 4, 0);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 32, 4,
                    GL_RGBA, GL_UNSIGNED_BYTE, destination.data());
    GLuint depth = 0;
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 16, 8, 0,
                 GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    GLuint srgb = 0;
    glGenTextures(1, &srgb);
    glBindTexture(GL_TEXTURE_2D, srgb);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, 32, 4, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenTextures(1, &deletedTexture);
    glBindTexture(GL_TEXTURE_2D, deletedTexture);
    glDeleteTextures(1, &deletedTexture);
    glBindTexture(GL_TEXTURE_2D, textures[2]);

    GLuint framebuffers[2]{};
    glGenFramebuffers(2, framebuffers);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[0]);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, textures[0], 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[1]);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, textures[1], 0);
    const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_NONE};
    glDrawBuffers(2, drawBuffers);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glScissor(0, 0, 1, 1);
    REQUIRE(glGetError() == GL_NO_ERROR);

    const auto statePreserved = [&]
    {
        GLint read = 0, draw = 0, readBuffer = 0, drawBuffer0 = 0, drawBuffer1 = 0;
        GLint scissor[4]{};
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw);
        glGetIntegerv(GL_READ_BUFFER, &readBuffer);
        glGetIntegerv(GL_DRAW_BUFFER0, &drawBuffer0);
        glGetIntegerv(GL_DRAW_BUFFER1, &drawBuffer1);
        glGetIntegerv(GL_SCISSOR_BOX, scissor);
        return bindingPreserved() &&
            read == static_cast<GLint>(framebuffers[0]) &&
            draw == static_cast<GLint>(framebuffers[1]) &&
            readBuffer == GL_COLOR_ATTACHMENT0 &&
            drawBuffer0 == GL_COLOR_ATTACHMENT0 && drawBuffer1 == GL_NONE &&
            glIsEnabled(GL_SCISSOR_TEST) &&
            glIsEnabled(GL_FRAMEBUFFER_SRGB) &&
            scissor[0] == 0 && scissor[1] == 0 &&
            scissor[2] == 1 && scissor[3] == 1 &&
            glGetError() == GL_NO_ERROR;
    };

    REQUIRE(texture_blit::BlitTexture(textures[0], textures[1]));
    REQUIRE(statePreserved());
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, destination.data());
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    for (int y = 0; y < 4; ++y)
    {
        for (int x : {0, 8, 23, 31})
        {
            const auto offset = (y * 32 + x) * 4;
            REQUIRE(destination[offset] == (x < 16 ? 128 : 0));
            REQUIRE(destination[offset + 1] == (x < 16 ? 0 : 255));
            REQUIRE(destination[offset + 2] == 0 && destination[offset + 3] == 255);
        }
    }
    REQUIRE(statePreserved());
    REQUIRE(!texture_blit::BlitTexture(textures[0], 0));
    REQUIRE(!texture_blit::BlitTexture(textures[0], textures[0]));
    REQUIRE(!texture_blit::BlitTexture(textures[0], deletedTexture));
    REQUIRE(!texture_blit::BlitTexture(textures[0], textures[3]));
    REQUIRE(!texture_blit::BlitTexture(textures[0], textures[4]));
    REQUIRE(!texture_blit::BlitTexture(textures[0], depth));
    REQUIRE(!texture_blit::BlitTexture(depth, textures[1]));
    REQUIRE(statePreserved());

    REQUIRE(texture_blit::BlitTexture(textures[0], srgb));
    REQUIRE(statePreserved());
    glBindTexture(GL_TEXTURE_2D, srgb);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, destination.data());
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    REQUIRE(destination[0] == 128);
    REQUIRE(statePreserved());

    GLuint unpackBuffer = 0;
    glGenBuffers(1, &unpackBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, unpackBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, 1, nullptr, GL_STATIC_DRAW);
    REQUIRE(texture_blit::TestFramebufferBlit(textures[0]));
    GLint restoredUnpackBuffer = 0;
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &restoredUnpackBuffer);
    REQUIRE(restoredUnpackBuffer == static_cast<GLint>(unpackBuffer));
    REQUIRE(statePreserved());
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glDeleteBuffers(1, &unpackBuffer);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_FRAMEBUFFER_SRGB);
    REQUIRE(texture_blit::BlitTexture(textures[0], textures[1]));
    REQUIRE(!glIsEnabled(GL_SCISSOR_TEST));
    REQUIRE(!glIsEnabled(GL_FRAMEBUFFER_SRGB));
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    REQUIRE(statePreserved());
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(2, framebuffers);
    glDeleteTextures(1, &depth);
    glDeleteTextures(1, &srgb);

    REQUIRE(glXMakeCurrent(display, None, nullptr));
    REQUIRE(!texture_validation::ValidateStereoTextures(textures[0], textures[1]));
    REQUIRE(!texture_blit::BlitTexture(textures[0], textures[1]));
    REQUIRE(!texture_blit::TestFramebufferBlit(textures[0]));
    REQUIRE(glXMakeCurrent(display, window, context));
    REQUIRE(bindingPreserved());
    glDeleteTextures(5, textures);
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);
    XDestroyWindow(display, window);
    XFreeColormap(display, colormap);
    XFree(visual);
    XCloseDisplay(display);
    std::cout << "Texture validation and scaled blit passed" << std::endl;
    return 0;
}
