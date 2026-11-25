27.1. include/XRCamera.hh မှာ ထည့်ပါ
```
public: အောက်မှာ
bool ValidateStereoTextures();
```


private: အောက်မှာ
```
bool textureValidationReported_ = false;
```


27.2. src/XRCamera.cc မှာ ထည့်ပါ
```
#include <GL/glew.h>
```


အောက်ပါ Function ကို File အဆုံးမှာ ထည့်ပါ။

```
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
```
ဒီ Code က Texture Binding ကို မူလအခြေအနေ ပြန်ထားပေးပါတယ်။ Camera ကို ထပ်ပြီး Render() မခေါ်တာကြောင့် Scene::PreRender() Lifecycle ကို မထိပါဘူး။

သို့သော် ဒီ Test က Gazebo ရဲ့ Render Thread နဲ့ OpenGL Context မှာပဲ Execute လုပ်ရပါမယ်။

27.3. GazeboOpenXR.cc မှာ ပြင်ပါ

လက်ရှိ RenderStereoDiagnostic() အောင်မြင်ပြီးနောက်မှာ အောက်ပါ Code ထည့်ပါ။

```
if (xrCamera_.IsStereoRenderReady())
{
    xrCamera_.ValidateStereoTextures();
}
```



ရှိပြီးသား Scene Rendering Code ကို မဖျက်ပါနဲ့။

27.4. Build & Test
```
cd ~/openxr_workspace/gazebo_openxr

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build -j8
```

```
export GZ_GUI_PLUGIN_PATH="$PWD/build"

export XR_RUNTIME_JSON="$(readlink -f \
"$HOME/.config/openxr/1/active_runtime.json")"

export XR_LOADER_DEBUG=error

gz sim --force-version 8 -v 4 shapes.sdf
```


Output
```
[XRCamera] Left Eye | Texture: 33 | Size: 1024x1024 | Format: 0x8c43
[XRCamera] Right Eye | Texture: 43 | Size: 1024x1024 | Format: 0x8c43
[XRCamera] Stereo Textures Validated
```

