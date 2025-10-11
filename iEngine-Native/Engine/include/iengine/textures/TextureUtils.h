#ifndef IENGINE_TEXTURE_UTILS_H
#define IENGINE_TEXTURE_UTILS_H

#include "Texture.h"

// OpenGL 前向声明
class OpenGLContext;

namespace iengine {

    // OpenGL 映射
    unsigned int getOpenGLWrapMode(TextureWrapMode mode);
    unsigned int getOpenGLMinFilter(TextureMinFilter filter);
    unsigned int getOpenGLMagFilter(TextureMagFilter filter);

    // WebGPU 映射 (预留)
    // 在实际实现中，这些函数将映射到 WebGPU 的枚举值
    // 由于 WebGPU 需要 dawn 库，这里只提供声明
    /*
    const char* getWebGPUWrapMode(TextureWrapMode mode);
    const char* getWebGPUMinMagFilter(TextureMinFilter filter);
    */

} // namespace iengine

#endif // IENGINE_TEXTURE_UTILS_H