#pragma once

namespace iengine {
    class BaseControls {
    public:
        virtual ~BaseControls() = default;
        
        // 子类实现：移除事件监听
        virtual void dispose() = 0;
    };
}