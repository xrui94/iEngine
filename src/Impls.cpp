#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    #define WEBGPU_CPP_IMPLEMENTATION   // 必须在第一次引用“webgpu.hpp”之前定义，否则会无法找到一系列的wgpu命名空间下的相关符号的调用
    #include <webgpu/webgpu.hpp>
#endif

// #define TINYOBJLOADER_IMPLEMENTATION
// #include "tiny_obj_loader.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"