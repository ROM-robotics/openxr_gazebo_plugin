#pragma once

#include <cstdint>

namespace texture_blit
{
bool BlitTexture(uint32_t source, uint32_t destination);
bool TestFramebufferBlit(uint32_t source);
}
