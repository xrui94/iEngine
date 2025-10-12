#include "iengine/textures/TextureUtils.h"

// OpenGL 头文件 (在实际项目中可能需要包含具体的OpenGL头文件)
// #include <GL/glew.h> 或 #include <glad/glad.h>

namespace iengine {

    unsigned int getOpenGLWrapMode(TextureWrapMode mode) {
        switch (mode) {
            case TextureWrapMode::Repeat:
                // return GL_REPEAT;
                return 0x2901; // GL_REPEAT 的值
            case TextureWrapMode::ClampToEdge:
                // return GL_CLAMP_TO_EDGE;
                return 0x812F; // GL_CLAMP_TO_EDGE 的值
            case TextureWrapMode::MirroredRepeat:
                // return GL_MIRRORED_REPEAT;
                return 0x8370; // GL_MIRRORED_REPEAT 的值
            default:
                // return GL_REPEAT;
                return 0x2901; // GL_REPEAT 的值
        }
    }

    unsigned int getOpenGLMinFilter(TextureMinFilter filter) {
        switch (filter) {
            case TextureMinFilter::Nearest:
                // return GL_NEAREST;
                return 0x2600; // GL_NEAREST 的值
            case TextureMinFilter::Linear:
                // return GL_LINEAR;
                return 0x2601; // GL_LINEAR 的值
            case TextureMinFilter::NearestMipmapNearest:
                // return GL_NEAREST_MIPMAP_NEAREST;
                return 0x2700; // GL_NEAREST_MIPMAP_NEAREST 的值
            case TextureMinFilter::LinearMipmapNearest:
                // return GL_LINEAR_MIPMAP_NEAREST;
                return 0x2701; // GL_LINEAR_MIPMAP_NEAREST 的值
            case TextureMinFilter::NearestMipmapLinear:
                // return GL_NEAREST_MIPMAP_LINEAR;
                return 0x2702; // GL_NEAREST_MIPMAP_LINEAR 的值
            case TextureMinFilter::LinearMipmapLinear:
                // return GL_LINEAR_MIPMAP_LINEAR;
                return 0x2703; // GL_LINEAR_MIPMAP_LINEAR 的值
            default:
                // return GL_LINEAR;
                return 0x2601; // GL_LINEAR 的值
        }
    }

    unsigned int getOpenGLMagFilter(TextureMagFilter filter) {
        switch (filter) {
            case TextureMagFilter::Nearest:
                // return GL_NEAREST;
                return 0x2600; // GL_NEAREST 的值
            case TextureMagFilter::Linear:
                // return GL_LINEAR;
                return 0x2601; // GL_LINEAR 的值
            default:
                // return GL_LINEAR;
                return 0x2601; // GL_LINEAR 的值
        }
    }

} // namespace iengine