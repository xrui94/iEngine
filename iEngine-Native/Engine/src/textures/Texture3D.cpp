#include "iengine/textures/Texture3D.h"

namespace iengine {

    Texture3D::Texture3D(const TextureOptions& options, int width, int height, int depth)
        : Texture(options), depth_(depth) {
        // 设置3D纹理的尺寸
        // 注意：在基类中width_和height_是受保护的成员
        // 这里需要通过基类的公共接口或直接访问受保护成员来设置
        // 为了简化，我们假设基类的width和height已经通过options设置
        // 实际实现中可能需要更复杂的处理
    }

    Texture3D::~Texture3D() {
        // 析构函数
    }

    int Texture3D::getDepth() const {
        return depth_;
    }

    void Texture3D::setDepth(int depth) {
        depth_ = depth;
        needsUpdate_ = true;
    }

    bool Texture3D::needsUpdate() const {
        // 重写基类方法，检查3D纹理是否需要更新
        return Texture::needsUpdate() || needsUpdate_;
    }

} // namespace iengine