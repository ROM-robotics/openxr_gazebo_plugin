
#include <GL/glew.h>
#include <GL/glx.h>
#include "XRCamera.hh"

#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/Visual.hh>
#include <gz/math/Pose3.hh>
#include <gz/math/Angle.hh> 
#include <iostream>

static bool InitializeGLFunctions();

bool XRCamera::FindScene()
{
    if (scene_ && scene_->IsInitialized() &&
        scene_->RootVisual())
    {
        return true;
    }

    scene_.reset();

    // Reuse the existing Gazebo rendering scene.
    // Do not create a separate rendering engine.
    auto engines = gz::rendering::loadedEngines();

    if (engines.empty())
        return false;

    if (engines.size() != 1)
        return false;

    auto *engine = gz::rendering::engine(engines[0]);

    if (!engine || engine->SceneCount() != 1)
        return false;

    auto candidate = engine->SceneByIndex(0);

    if (!candidate || !candidate->IsInitialized() ||
        !candidate->RootVisual())
    {
        return false;
    }

    scene_ = candidate;

    if (!reported_)
    {
        std::cout
            << "[XRCamera] Gazebo Scene Ready\n"
            << "Rendering Engine: "
            << engines[0]
            << "\nScene Name: "
            << scene_->Name()
            << "\nRoot Visual: Available"
            << std::endl;

        reported_ = true;
    }

    return true;
}

bool XRCamera::IsReady() const
{
    return scene_ &&
           scene_->IsInitialized() &&
           scene_->RootVisual() != nullptr;
}

bool XRCamera::CreateStereoCameras()
{
    if (!IsReady())
        return false;

    if (leftEye_ && rightEye_)
        return true;

    // Diagnostic resolution only.
    // Replace with OpenXR recommended sizes later.
    constexpr unsigned int width = 1024;
    constexpr unsigned int height = 1024;

    // Temporary diagnostic IPD (64 mm).
    constexpr double halfIPD = 0.032;

    auto createEye =
        [&](const std::string &name, double y)
        -> gz::rendering::CameraPtr
    {
        auto camera = scene_->CreateCamera(name);

        if (!camera)
            return nullptr;

        camera->SetImageWidth(width);
        camera->SetImageHeight(height);
        camera->SetAspectRatio(
            static_cast<double>(width) / height);

        camera->SetHFOV(gz::math::Angle(1.57));
        camera->SetNearClipPlane(0.05);
        camera->SetFarClipPlane(1000.0);

        camera->SetLocalPose(
            gz::math::Pose3d(0, y, 1.6, 0, 0, 0));

        scene_->RootVisual()->AddChild(camera);

        return camera;
    };

    if (!leftEye_)
        leftEye_ = createEye("xr_left_eye", halfIPD);

    if (!rightEye_)
        rightEye_ = createEye("xr_right_eye", -halfIPD);

    if (!leftEye_ || !rightEye_)
    {
        std::cerr
            << "[XRCamera] Stereo camera creation failed"
            << std::endl;

        return false;
    }

    if (!stereoReported_)
    {
        std::cout
            << "[XRCamera] Stereo Cameras Created\n"
            << "Left: " << leftEye_->Name() << "\n"
            << "Right: " << rightEye_->Name() << "\n"
            << "Resolution: "
            << width << " x " << height
            << " per eye\n"
            << "Diagnostic IPD: 0.064 m"
            << std::endl;

        stereoReported_ = true;
    }

    return true;
}

bool XRCamera::RenderStereoDiagnostic()
{
    if (stereoRenderReady_)
        return true;

    if (renderAttempted_)
        return false;

    if (!IsReady() || !leftEye_ || !rightEye_)
        return false;

    // Only one diagnostic attempt per scene.
    renderAttempted_ = true;

    std::cout
        << "[XRCamera] Starting coordinated stereo render"
        << std::endl;

    // This method must run in the Gazebo rendering
    // thread after MinimalScene's main camera Update().
    //
    // It starts a NEW, complete scene rendering cycle.

    scene_->PreRender();

    // Render both stereo cameras inside the same
    // scene rendering cycle.
    leftEye_->Render();
    rightEye_->Render();

    // Finalize both camera render passes.
    leftEye_->PostRender();
    rightEye_->PostRender();

    // CRITICAL:
    // Complete the scene rendering cycle.
    // The previous implementation missed this call.
    scene_->PostRender();

    // Obtain Gazebo-owned OpenGL texture handles.
    leftTextureId_ =
        leftEye_->RenderTextureGLId();

    rightTextureId_ =
        rightEye_->RenderTextureGLId();

    std::cout
        << "[XRCamera] Left texture ID: "
        << leftTextureId_
        << "\n[XRCamera] Right texture ID: "
        << rightTextureId_
        << std::endl;

    if (leftTextureId_ == 0 ||
        rightTextureId_ == 0)
    {
        std::cerr
            << "[XRCamera] Invalid stereo render texture"
            << std::endl;

        return false;
    }

    stereoRenderReady_ = true;

    std::cout
        << "[XRCamera] Coordinated Stereo Render SUCCESS"
        << std::endl;

    return true;
}

bool XRCamera::IsStereoRenderReady() const
{
    return stereoRenderReady_;
}

bool XRCamera::ValidateStereoTextures()
{
    if (!stereoRenderReady_)
        return false;

    if (textureValidationReported_)
        return true;

    const GLuint textures[2] = {
        leftTextureId_,
        rightTextureId_
    };

    const char* names[2] = {
        "Left Eye",
        "Right Eye"
    };

    GLint previousBinding = 0;

    glGetIntegerv(
        GL_TEXTURE_BINDING_2D,
        &previousBinding
    );

    for (int eye = 0; eye < 2; ++eye)
    {
        const GLuint texture = textures[eye];

        if (texture == 0 || glIsTexture(texture) != GL_TRUE)
        {
            std::cerr
                << "[XRCamera] Invalid texture: "
                << names[eye] << std::endl;

            glBindTexture(GL_TEXTURE_2D, previousBinding);
            return false;
        }

        glBindTexture(GL_TEXTURE_2D, texture);

        GLint width = 0;
        GLint height = 0;
        GLint internalFormat = 0;

        glGetTexLevelParameteriv(
            GL_TEXTURE_2D, 0,
            GL_TEXTURE_WIDTH, &width);

        glGetTexLevelParameteriv(
            GL_TEXTURE_2D, 0,
            GL_TEXTURE_HEIGHT, &height);

        glGetTexLevelParameteriv(
            GL_TEXTURE_2D, 0,
            GL_TEXTURE_INTERNAL_FORMAT,
            &internalFormat);

        std::cout
            << "[XRCamera] " << names[eye]
            << " | Texture: " << texture
            << " | Size: " << width << "x" << height
            << " | Format: 0x"
            << std::hex << internalFormat
            << std::dec << std::endl;

        if (width <= 0 || height <= 0)
        {
            glBindTexture(GL_TEXTURE_2D, previousBinding);
            return false;
        }
    }

    glBindTexture(GL_TEXTURE_2D, previousBinding);

    textureValidationReported_ = true;

    std::cout
        << "[XRCamera] Stereo Textures Validated"
        << std::endl;

    return true;
}

bool XRCamera::TestFramebufferBlit()
{
    if (!stereoRenderReady_ || !textureValidationReported_)
        return false;

    if (blitTestAttempted_)
        return false;

    blitTestAttempted_ = true;

    if (!InitializeGLFunctions())
    {
        std::cerr
            << "[XRCamera] OpenGL function loading failed"
            << std::endl;
        return false;
    }
    GLint oldReadFbo = 0;
    GLint oldDrawFbo = 0;
    GLint oldTexture = 0;
    GLint oldReadBuffer = 0;
    GLint oldDrawBuffer = 0;

    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &oldReadFbo);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldDrawFbo);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);
    glGetIntegerv(GL_READ_BUFFER, &oldReadBuffer);
    glGetIntegerv(GL_DRAW_BUFFER, &oldDrawBuffer);

    GLuint readFbo = 0;
    GLuint drawFbo = 0;
    GLuint testTexture = 0;

    bool success = true;

    glGenFramebuffers(1, &readFbo);
    glGenFramebuffers(1, &drawFbo);
    glGenTextures(1, &testTexture);

    glBindTexture(GL_TEXTURE_2D, testTexture);

    constexpr GLint dstWidth = 512;
    constexpr GLint dstHeight = 512;

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        dstWidth, dstHeight, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        testTexture, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)
        != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "[XRCamera] Draw FBO incomplete\n";
        success = false;
    }

    if (success)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);

        glFramebufferTexture2D(
            GL_READ_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            leftTextureId_, 0);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER)
            != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "[XRCamera] Read FBO incomplete\n";
            success = false;
        }
    }

    if (success)
    {
        // 1024x1024 diagnostic source
        // into a 512x512 temporary texture.
        glBlitFramebuffer(
            0, 0, 1024, 1024,
            0, 0, dstWidth, dstHeight,
            GL_COLOR_BUFFER_BIT, GL_LINEAR);

        GLenum error = glGetError();

        if (error != GL_NO_ERROR)
        {
            std::cerr << "[XRCamera] Blit GL error: 0x"
                      << std::hex << error << std::dec
                      << std::endl;
            success = false;
        }
        else
        {
            glFinish(); // Diagnostic only
            std::cout
                << "[XRCamera] Framebuffer Blit SUCCESS"
                << std::endl;
        }
    }

    // Unbind test FBOs before deleting.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, oldReadFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldDrawFbo);

    glReadBuffer(oldReadBuffer);
    glDrawBuffer(oldDrawBuffer);

    glBindTexture(GL_TEXTURE_2D, oldTexture);

    glDeleteFramebuffers(1, &readFbo);
    glDeleteFramebuffers(1, &drawFbo);
    glDeleteTextures(1, &testTexture);

    return success;
}

static bool InitializeGLFunctions()
{
    if (glXGetCurrentContext() == nullptr)
    {
        std::cerr
            << "[XRCamera] ERROR: No current GLX context\n";
        return false;
    }

    glewExperimental = GL_TRUE;

    GLenum result = glewInit();

    if (result != GLEW_OK)
    {
        std::cerr
            << "[XRCamera] GLEW initialization failed: "
            << glewGetErrorString(result)
            << std::endl;
        return false;
    }

    // GLEW may leave GL_INVALID_ENUM on core profiles.
    while (glGetError() != GL_NO_ERROR) {}

    if (!glGenFramebuffers ||
        !glBindFramebuffer ||
        !glFramebufferTexture2D ||
        !glCheckFramebufferStatus ||
        !glBlitFramebuffer ||
        !glDeleteFramebuffers)
    {
        std::cerr
            << "[XRCamera] Missing OpenGL framebuffer functions\n";
        return false;
    }

    std::cout
        << "[XRCamera] GLEW Initialized: "
        << glewGetString(GLEW_VERSION)
        << "\n[XRCamera] Framebuffer functions available"
        << std::endl;

    return true;
}

unsigned int XRCamera::GetLeftTextureId() const
{
    return stereoRenderReady_ ? leftTextureId_ : 0;
}

unsigned int XRCamera::GetRightTextureId() const
{
    return stereoRenderReady_ ? rightTextureId_ : 0;
}
