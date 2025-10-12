#include "iengine/windowing/WindowFactory.h"
#include <iostream>

namespace iengine {
    
    // 静态成员初始化
    std::map<WindowType, WindowCreatorFunction> WindowFactory::creators_;
    
    void WindowFactory::registerWindowCreator(WindowType type, WindowCreatorFunction creator) {
        creators_[type] = creator;
        std::cout << "WindowFactory: Registered window creator for type " << static_cast<int>(type) << std::endl;
    }
    
    std::unique_ptr<WindowInterface> WindowFactory::createWindow(WindowType type) {
        auto it = creators_.find(type);
        if (it != creators_.end()) {
            return it->second();
        }
        
        std::cerr << "WindowFactory: No creator registered for window type " << static_cast<int>(type) << std::endl;
        return nullptr;
    }
    
    bool WindowFactory::isWindowTypeAvailable(WindowType type) {
        return creators_.find(type) != creators_.end();
    }
    
    std::vector<WindowType> WindowFactory::getAvailableWindowTypes() {
        std::vector<WindowType> types;
        for (const auto& pair : creators_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    void WindowFactory::clearCreators() {
        creators_.clear();
        std::cout << "WindowFactory: Cleared all window creators" << std::endl;
    }
    
} // namespace iengine