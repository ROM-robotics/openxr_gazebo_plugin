#include "TextureValidation.hh"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>
#include <sstream>

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

    REQUIRE(glXMakeCurrent(display, None, nullptr));
    REQUIRE(!texture_validation::ValidateStereoTextures(textures[0], textures[1]));
    REQUIRE(glXMakeCurrent(display, window, context));
    REQUIRE(bindingPreserved());
    glDeleteTextures(5, textures);
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);
    XDestroyWindow(display, window);
    XFreeColormap(display, colormap);
    XFree(visual);
    XCloseDisplay(display);
    std::cout << "Texture validation and binding restoration passed" << std::endl;
    return 0;
}
