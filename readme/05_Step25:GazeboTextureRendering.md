## Gazebo Camera Texture Rendering


Gazebo OGRE 2 Scene
Scene Access: Ready
Left Camera
Gazebo Render Texture

Right Camera
Gazebo Render Texture


OpenGL Texture Transfer
Render / Copy into acquired XR images
OpenXR Projection Layer
Submit stereoscopic frames to WiVRn

Step 25.1: Camera Texture API စစ်ဆေးခြင်း

နောက်တစ်ဆင့်မှာ Code မပြင်ခင် Gazebo Rendering 8 ရဲ့ Camera Texture API ကို အရင်စစ်မယ်။
```
grep -R -nE \
  'RenderTextureGLId|RenderTexture|CreateRenderTexture' \
  /usr/include/gz/rendering8/gz/rendering \
  2>/dev/null | head -40
```

Rendering Engine မှာ OpenGL Texture API ဘယ်လိုဖော်ပြထားလဲကို ဒီ Command နဲ့ စစ်ပါ။