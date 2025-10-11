#pragma once

#include <memory>

namespace iengine {
    class IRenderable {
    public:
        virtual ~IRenderable() = default;
        
        // 渲染接口
        virtual void render() = 0;
    };
}