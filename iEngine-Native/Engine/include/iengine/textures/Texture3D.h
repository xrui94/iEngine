#ifndef IENGINE_TEXTURE3D_H
#define IENGINE_TEXTURE3D_H

#include "Texture.h"

namespace iengine {

    class Texture3D : public Texture {
    public:
        Texture3D(const TextureOptions& options, int width = 64, int height = 64, int depth = 64);
        ~Texture3D();

        // Getters
        int getDepth() const;

        // Setters
        void setDepth(int depth);

        // 重写基类方法
        virtual bool needsUpdate() const;

    private:
        int depth_;
    };

} // namespace iengine

#endif // IENGINE_TEXTURE3D_H