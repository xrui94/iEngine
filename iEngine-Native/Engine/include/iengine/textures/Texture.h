#ifndef IENGINE_TEXTURE_H
#define IENGINE_TEXTURE_H

#include <string>
#include <memory>
#include <cstdint>

namespace iengine {

    enum class TextureKind {
        // PBR
        BaseColor,
        MetallicRoughness,
        Normal,
        AO,
        Emissive,
        Clearcoat,
        Sheen,
        Transmission,
        Iridescence,
        Anisotropy,
        // Blinn-Phong / 传统
        Diffuse,
        Specular,
        Glossiness,
        Opacity,
        Reflection,
        Bump,
        Displacement,
        Ambient,
        Light,
        // Lambert
        Albedo,
        // 卡通/Toon
        ToonRamp,
        ToonOutline,
        // 通用
        White,
        Black,
        // 用户自定义
        Custom
    };

    enum class TextureWrapMode {
        Repeat,
        ClampToEdge,
        MirroredRepeat
    };

    enum class TextureMinFilter {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear
    };

    enum class TextureMagFilter {
        Nearest,
        Linear
    };

    // 前向声明
    class OpenGLContext;
    class WebGPUContext;
    class Context;

    struct TextureOptions {
        std::string name;
        TextureWrapMode wrapS = TextureWrapMode::Repeat;
        TextureWrapMode wrapT = TextureWrapMode::Repeat;
        TextureMinFilter minFilter = TextureMinFilter::Linear;
        TextureMagFilter magFilter = TextureMagFilter::Linear;
        // 在C++版本中，我们使用文件路径而不是图像对象
        std::string sourcePath;
    };

    class Texture {
    public:
        Texture(const TextureOptions& options);
        virtual ~Texture();

        // Getters
        const std::string& getName() const;
        int getWidth() const;
        int getHeight() const;
        int getUnit() const;
        TextureWrapMode getWrapS() const;
        TextureWrapMode getWrapT() const;
        TextureMinFilter getMinFilter() const;
        TextureMagFilter getMagFilter() const;
        void* getGpuTexture() const;

        // Setters
        void setUnit(int unit);
        void setWrapS(TextureWrapMode wrapS);
        void setWrapT(TextureWrapMode wrapT);
        void setMinFilter(TextureMinFilter minFilter);
        void setMagFilter(TextureMagFilter magFilter);

        // 状态检查
        virtual bool needsUpdate() const;
        void markUpdated();

        // 上传到GPU
        virtual void upload(Context* context, bool force = false);

        // 更新纹理内容
        virtual void updateTexture(Context* context);

    protected:
        std::string name_;
        int width_;
        int height_;
        int unit_;
        void* gpuTexture_;
        int gpuTextureWidth_;
        int gpuTextureHeight_;
        TextureWrapMode wrapS_;
        TextureWrapMode wrapT_;
        TextureMinFilter minFilter_;
        TextureMagFilter magFilter_;
        bool needsUpdate_;

        // 加载图像数据
        void loadFromFile(const std::string& filePath);

    private:
        static const int DEFAULT_WIDTH = 2;
        static const int DEFAULT_HEIGHT = 2;
        static uint8_t defaultImageData_[16]; // 2x2 RGBA 数据
    };

} // namespace iengine

#endif // IENGINE_TEXTURE_H