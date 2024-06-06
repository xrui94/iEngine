#pragma once

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    #include <webgpu/webgpu.hpp>
#elif defined(IE_ONLY_EMSCRIPTEN)
    #include <webgpu/webgpu_cpp.h>
#endif

#include <glm/vec3.hpp> // all types inspired from GLSL

#include <string>


/**
 * A structure that describes the data layout in the vertex buffer
 * We do not instantiate it but use it in `sizeof` and `offsetof`
 */
struct VertexAttrs {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
};

struct VertexAttrInfo
{
    // std::string name;
    uint32_t		    location;
    wgpu::VertexFormat	format;
    uint32_t		    offset;
};

#if defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    struct WebVertexAttrInfo
    {
        // std::string name;
        uint32_t		    location;
        std::string         format;
        uint32_t		    offset;
    };
#endif