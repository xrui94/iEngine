#include "../../include/iengine/textures/Texture.h"
#include <iostream>
#include <cstring>

namespace iengine {

    // 默认2x2 RGBA 棋盘格数据
    uint8_t Texture::defaultImageData_[16] = {
        255, 255, 255, 255,   192, 192, 192, 255,
        192, 192, 192, 255,   255, 255, 255, 255
    };

    Texture::Texture(const TextureOptions& options) 
        : name_(options.name),
          width_(DEFAULT_WIDTH),
          height_(DEFAULT_HEIGHT),
          unit_(0),
          gpuTexture_(nullptr),
          gpuTextureWidth_(0),
          gpuTextureHeight_(0),
          wrapS_(options.wrapS),
          wrapT_(options.wrapT),
          minFilter_(options.minFilter),
          magFilter_(options.magFilter),
          needsUpdate_(false) {
        
        if (!options.sourcePath.empty()) {
            loadFromFile(options.sourcePath);
        }
    }

    Texture::~Texture() {
        // 在实际实现中，需要释放GPU纹理资源
        // 这里简化处理
        gpuTexture_ = nullptr;
    }

    const std::string& Texture::getName() const {
        return name_;
    }

    int Texture::getWidth() const {
        return width_;
    }

    int Texture::getHeight() const {
        return height_;
    }

    int Texture::getUnit() const {
        return unit_;
    }

    void Texture::setUnit(int unit) {
        unit_ = unit;
    }

    TextureWrapMode Texture::getWrapS() const {
        return wrapS_;
    }

    void Texture::setWrapS(TextureWrapMode wrapS) {
        wrapS_ = wrapS;
        needsUpdate_ = true;
    }

    TextureWrapMode Texture::getWrapT() const {
        return wrapT_;
    }

    void Texture::setWrapT(TextureWrapMode wrapT) {
        wrapT_ = wrapT;
        needsUpdate_ = true;
    }

    TextureMinFilter Texture::getMinFilter() const {
        return minFilter_;
    }

    void Texture::setMinFilter(TextureMinFilter minFilter) {
        minFilter_ = minFilter;
        needsUpdate_ = true;
    }

    TextureMagFilter Texture::getMagFilter() const {
        return magFilter_;
    }

    void Texture::setMagFilter(TextureMagFilter magFilter) {
        magFilter_ = magFilter;
        needsUpdate_ = true;
    }

    void* Texture::getGpuTexture() const {
        return gpuTexture_;
    }

    bool Texture::needsUpdate() const {
        return needsUpdate_;
    }

    void Texture::markUpdated() {
        needsUpdate_ = false;
    }

    void Texture::upload(Context* context, bool force) {
        // 在实际实现中，这里会根据上下文类型（OpenGL或WebGPU）上传纹理数据
        // 这里简化处理
        std::cout << "Uploading texture: " << name_ << std::endl;
        
        if (!gpuTexture_ || force) {
            // 创建GPU纹理
            // 这里应该根据具体的渲染API创建纹理对象
            gpuTexture_ = new char[1]; // 简化示例
            gpuTextureWidth_ = width_;
            gpuTextureHeight_ = height_;
        }
        
        if (needsUpdate_) {
            // 上传数据到GPU
            std::cout << "Uploading texture data to GPU" << std::endl;
            needsUpdate_ = false;
        }
    }

    void Texture::updateTexture(Context* context) {
        if (!gpuTexture_) {
            std::cout << "GPU texture not created, cannot update texture" << std::endl;
            return;
        }
        
        // 更新纹理内容
        std::cout << "Updating texture content: " << name_ << std::endl;
        needsUpdate_ = false;
    }

    void Texture::loadFromFile(const std::string& filePath) {
        // 在实际实现中，这里会加载图像文件并设置width_和height_
        // 这里简化处理，假设加载成功
        std::cout << "Loading texture from file: " << filePath << std::endl;
        width_ = 256;  // 示例值
        height_ = 256; // 示例值
        needsUpdate_ = true;
    }

} // namespace iengine