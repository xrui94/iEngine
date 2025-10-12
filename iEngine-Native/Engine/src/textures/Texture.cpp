#include "iengine/textures/Texture.h"
#include "iengine/renderers/Context.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <memory>

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
          needsUpdate_(true),
          channels_(4) { // RGBA
        
        // 初始化为默认图像数据
        setImageData(defaultImageData_, DEFAULT_WIDTH, DEFAULT_HEIGHT, 4);
        
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

    void Texture::upload(std::shared_ptr<Context> context, bool force) {
        std::cout << "Uploading texture: " << name_ << " (" << width_ << "x" << height_ << ")" << std::endl;
        
        // 1. 判断是否需要重新创建GPU纹理
        if (!gpuTexture_ || force || 
            (gpuTextureWidth_ != width_ || gpuTextureHeight_ != height_)) {
            
            // 清理旧纹理
            if (gpuTexture_) {
                context->deleteTexture(gpuTexture_);
                gpuTexture_ = nullptr;
            }
            
            // 创建GPU纹理
            gpuTexture_ = context->createTexture(width_, height_, imageData_.get());
            gpuTextureWidth_ = width_;
            gpuTextureHeight_ = height_;
        }
        
        // 2. 上传数据
        if (needsUpdate_ && gpuTexture_) {
            context->writeTexture(gpuTexture_, imageData_.get(), width_, height_);
            needsUpdate_ = false;
        }
        
        std::cout << "Texture uploaded successfully: " << name_ << std::endl;
    }

    void Texture::updateTexture(std::shared_ptr<Context> context) {
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
        
        // 创建一个简单的棋盘格纹理数据作为示例
        channels_ = 4; // RGBA
        size_t dataSize = width_ * height_ * channels_;
        imageData_ = std::make_unique<uint8_t[]>(dataSize);
        
        // 生成棋盘格图案
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                size_t index = (y * width_ + x) * channels_;
                bool isWhite = ((x / 32) + (y / 32)) % 2 == 0;
                uint8_t color = isWhite ? 255 : 128;
                imageData_[index + 0] = color;     // R
                imageData_[index + 1] = color;     // G
                imageData_[index + 2] = color;     // B
                imageData_[index + 3] = 255;       // A
            }
        }
        
        needsUpdate_ = true;
    }
    
    void Texture::setImageData(const uint8_t* data, int width, int height, int channels) {
        width_ = width;
        height_ = height;
        channels_ = channels;
        
        size_t dataSize = width_ * height_ * channels_;
        imageData_ = std::make_unique<uint8_t[]>(dataSize);
        std::memcpy(imageData_.get(), data, dataSize);
        
        needsUpdate_ = true;
    }
    
    const uint8_t* Texture::getDefaultImageData() {
        return defaultImageData_;
    }

} // namespace iengine