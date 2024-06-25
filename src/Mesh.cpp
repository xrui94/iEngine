#include "Mesh.h"
#include "renderers/webgpu/WebGPUDevice.h"

// #include <glad/glad.h>

#include <memory>
#include <iostream>

#if defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    Mesh::Mesh(const emscripten::val& vertexBufferVal, const emscripten::val& indexBufferVal, const emscripten::val& vertexAttrInfoListVal)
        : m_IsPrepared(false),
        m_NodeUniformBuffer(nullptr),
        m_BindGroupLayouts({}), m_BindGroups({}),
        m_GPUVertexBuffer(nullptr), m_GPUIndexBuffer(nullptr), /*m_BlendState(nullptr),*/ m_Pipeline(nullptr), m_Shader(nullptr)
    {
        // m_VertexCount = 3;
        // m_VertexBuffer =  {
        //     // position             // normal        // color
        //     -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,
        //      0.5f, -0.5f, 1.0f,     0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,
        //      0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,
        // };

        // // index
        // m_IndexCount = 3;
        // m_IndexBuffer = {0, 1, 2};

        uint32_t vertexBufferLength = vertexBufferVal["length"].as<uint32_t>();
        m_VertexCount  = vertexBufferLength / 3 / 3;
        m_VertexBuffer.resize(vertexBufferLength);
        auto vertexMemory = emscripten::val::module_property("HEAPU8")["buffer"];
        auto vertexMemoryView = vertexBufferVal["constructor"].new_(vertexMemory, reinterpret_cast<uintptr_t>(m_VertexBuffer.data()), vertexBufferLength);
        vertexMemoryView.call<void>("set", vertexBufferVal);

        m_IndexCount = indexBufferVal["length"].as<uint32_t>();
        m_IndexBuffer.resize(m_IndexCount);
        auto indexMemory = emscripten::val::module_property("HEAPU8")["buffer"];
        auto indexMemoryView = indexBufferVal["constructor"].new_(indexMemory, reinterpret_cast<uintptr_t>(m_IndexBuffer.data()), m_IndexCount);
        indexMemoryView.call<void>("set", indexBufferVal);

        // std::cout << "vertex:" << std::endl;
        // for(auto& v : m_VertexBuffer)
        // {
        //     std::cout << v << "," << std::endl;
        // }

        // std::cout << "indices:" << std::endl;
        // for(auto& v : m_IndexBuffer)
        // {
        //     std::cout << v << "," << std::endl;
        // }

        std::vector<WebVertexAttrInfo> vertexAttrInfoList = emscripten::vecFromJSArray<WebVertexAttrInfo>(vertexAttrInfoListVal);
        for(auto &item : vertexAttrInfoList)
        {
            // std::cout << item.location << std::endl;
            // std::cout << item.format << std::endl;
            m_VertexAttrInfos.push_back({
                .location = item.location,
                .format = wgpu::VertexFormat::Float32x3,
                .offset = item.offset
            });
        }
    }
#endif

Mesh::Mesh(std::vector<float> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfos)
    : m_IsPrepared(false), m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_VertexAttrInfos(vertexAttrInfos),
    m_NodeUniformBuffer(nullptr),
    m_BindGroupLayouts({}), m_BindGroups({}),
    m_GPUVertexBuffer(nullptr), m_GPUIndexBuffer(nullptr), /*m_BlendState(nullptr),*/ m_Pipeline(nullptr), m_Shader(nullptr)
{
    m_VertexCount = static_cast<uint32_t>(vertexBuffer.size()) / 3 / 3;
    m_IndexCount = static_cast<uint32_t>(indexBuffer.size());

    // std::cout << sizeof(*buffer) << std::endl;
    // m_Buffer = (float*)malloc(81);
    // if (m_Buffer != nullptr)
    // {
    //     memcpy(m_Buffer, buffer, 81);
    // }

    //m_VertexAttrInfo = vertexAttrInfoList;
    //m_VertexAttrInfo.name = vertexAttrInfo.name,
    //m_VertexAttrInfo.index = vertexAttrInfo.index,
    //m_VertexAttrInfo.count = vertexAttrInfo.count,
    //m_VertexAttrInfo.dataType = vertexAttrInfo.dataType,
    //m_VertexAttrInfo.normalized = vertexAttrInfo.normalized,
    //m_VertexAttrInfo.stride = vertexAttrInfo.stride,
    //m_VertexAttrInfo.offset = vertexAttrInfo.offset
}

Mesh::Mesh(std::vector<VertexAttrs> vertexVec3f, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfos)
    : m_IsPrepared(false), m_VertexVec3f(vertexVec3f), m_IndexBuffer(indexBuffer), m_VertexAttrInfos(vertexAttrInfos),
    m_NodeUniformBuffer(nullptr),
    m_BindGroupLayouts({}), m_BindGroups({}),
    m_GPUVertexBuffer(nullptr), m_GPUIndexBuffer(nullptr), /*m_BlendState(nullptr),*/ m_Pipeline(nullptr), m_Shader(nullptr),
    m_VertexCount(static_cast<uint32_t>(vertexVec3f.size())), m_IndexCount(static_cast<uint32_t>(vertexVec3f.size()))
{
    // std::cout << sizeof(*buffer) << std::endl;
    // m_Buffer = (float*)malloc(81);
    // if (m_Buffer != nullptr)
    // {
    //     memcpy(m_Buffer, buffer, 81);
    // }

    //m_VertexAttrInfo = vertexAttrInfoList;
    //m_VertexAttrInfo.name = vertexAttrInfo.name,
    //m_VertexAttrInfo.index = vertexAttrInfo.index,
    //m_VertexAttrInfo.count = vertexAttrInfo.count,
    //m_VertexAttrInfo.dataType = vertexAttrInfo.dataType,
    //m_VertexAttrInfo.normalized = vertexAttrInfo.normalized,
    //m_VertexAttrInfo.stride = vertexAttrInfo.stride,
    //m_VertexAttrInfo.offset = vertexAttrInfo.offset
}

Mesh::~Mesh()
{
    // if (m_Buffer != nullptr)
    // {
    //     free(m_Buffer);
    // }

    Destroy();
}

void Mesh::Destroy()
{
#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    m_NodeUniformBuffer.destroy();
    m_NodeUniformBuffer.release();

    m_GPUVertexBuffer.destroy();
    m_GPUVertexBuffer.release();

    m_GPUIndexBuffer.destroy();
    m_GPUIndexBuffer.release();

    for (auto& item : m_BindGroups)
    {
        item.second.bindGroup.release();
    }
    m_BindGroups.clear();

    m_Pipeline.release();

    for (auto& item : m_BindGroupLayouts)
    {
        item.second.release();
    }
    m_BindGroupLayouts.clear();
#elif defined(IE_ONLY_EMSCRIPTEN)
    m_NodeUniformBuffer.Destroy();
    m_GPUVertexBuffer.Destroy();
    m_GPUIndexBuffer.Destroy();


    m_BindGroups.clear();
    // m_Pipeline.release();
    m_BindGroupLayouts.clear();
#endif
}

void Mesh::UpdateBindGroup(std::vector<ResourceBindGroup>& resourceBindGroups)
{
    for (const ResourceBindGroup& item : resourceBindGroups)
    {
        // 使用“[]”运算符，虽然满足这样的机制：key不存在时，会插入value，而key存在时，会更新value
        // 但是，由于wgpu::BindGroupLayout没有在默认构造函数，而导致无法直接使用“[]”进行赋值更新
        // 最终导致编译失败！可以先获取Value（本质上是获取了引用），然后进行赋值即可
        //m_BindGroupLayouts[item.label] = item.layout;

        auto iter = m_BindGroupLayouts.find(item.label);
        if (iter != m_BindGroupLayouts.end()) {
            // 键已存在，更新对应的值
            iter->second = item.layout;
        }
        else {
            // 键不存在，插入新的键值对
            m_BindGroupLayouts.emplace(item.label, item.layout);
        }

        // 
        m_BindGroups[item.label] = {
            static_cast<uint32_t>(m_BindGroups.size()),
            item.group
        };
    }
}

void Mesh::CreateBuffer(std::vector<float> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfos)
{
    // // Vertex fetch
	std::vector<wgpu::VertexAttribute> vertexAttrs(3);
	// //                                         ^ This was a 2

	// // Position attribute
	// vertexAttrs[0].shaderLocation = 0;
	// vertexAttrs[0].format = wgpu::VertexFormat::Float32x3;
	// vertexAttrs[0].offset = 0;

	// // Normal attribute
	// vertexAttrs[1].shaderLocation = 1;
	// vertexAttrs[1].format = wgpu::VertexFormat::Float32x3;
	// vertexAttrs[1].offset = offsetof(VertexAttrs, normal);

	// // Color attribute
	// vertexAttrs[2].shaderLocation = 2;
	// vertexAttrs[2].format = wgpu::VertexFormat::Float32x3;
	// vertexAttrs[2].offset = offsetof(VertexAttrs, color);

    for(uint16_t i = 0; i < 3; i++)
    {
        vertexAttrs[i].shaderLocation = vertexAttrInfos[i].location;
        vertexAttrs[i].format = vertexAttrInfos[i].format;
        vertexAttrs[i].offset = vertexAttrInfos[i].offset;
    }

    auto vertexBufferLayout = WebGPUDevice::Get().CreateVertexBufferLayout(vertexAttrs);
    m_GPUVertexBuffer = WebGPUDevice::Get().CreateVertexBuffer(vertexBuffer, "vertexAttr");
    m_GPUIndexBuffer = WebGPUDevice::Get().CreateIndexBuffer(indexBuffer, "indices");

    // 创建节点Uniform，相当于当前模型的变换矩阵
    m_NodeUniformBuffer = WebGPUDevice::Get().CreateUniformBuffer<NodeUniforms>("NodeUniforms");

    NodeUniforms nodeUniforms{};
    nodeUniforms.modelMatrix = glm::mat4(1.0f); // 使用单位矩阵进行调试
    WebGPUDevice::Get().WriteUniformBuffer(
        m_NodeUniformBuffer,
        offsetof(NodeUniforms, modelMatrix),
        &nodeUniforms.modelMatrix,
        sizeof(NodeUniforms::modelMatrix)
    );

    // 模型变换矩阵
    std::vector<UniformLayoutEntryInfo> uniformLayoutEntryInfos = {
		UniformLayoutEntryInfo {
			static_cast<uint32_t>(0),
            wgpu::ShaderStage::Vertex,
			sizeof(NodeUniforms)
		}
	};
	std::vector<wgpu::BindGroupLayoutEntry> uniformBindGroupLayoutEntries;
    WebGPUDevice::Get().CreateUniformBindGroupLayoutEntries(uniformLayoutEntryInfos, uniformBindGroupLayoutEntries);
	auto nodeBindGroupLayout = WebGPUDevice::Get().CreateBindGroupLayout(uniformBindGroupLayoutEntries);
    m_BindGroupLayouts.emplace("NodeUniforms", nodeBindGroupLayout);

	// 模型变换矩阵
	std::vector<UniformEntryInfo> uniformEntryInfos = {
		{
			0,
			m_NodeUniformBuffer,
            0,
			sizeof(NodeUniforms)
		}
	};
	std::vector<wgpu::BindGroupEntry> uniformBindGroupEntries;
	WebGPUDevice::Get().CreateUniformBindGroupEntry(uniformEntryInfos, uniformBindGroupEntries);
    auto nodeBindGroup = WebGPUDevice::Get().CreateBindGroup(uniformBindGroupEntries, nodeBindGroupLayout);
    m_BindGroups.emplace("NodeUniforms", BindGroupData{ static_cast<uint32_t>(m_BindGroups.size()), nodeBindGroup });


    // 暂时没有Material、texture等需要创建Uniform
    // m_GPUUniformBuffer = WebGPUDevice::Get().CreateUniformBuffer(baseUniforms, "baseUniforms");

    // 暂时没有Material等需要创建BindGroup
    // auto bindGroupLayout = WebGPUDevice::Get().CreateBindGroupLayout();
    // m_BindGroup = WebGPUDevice::Get().CreateBindGroup(bindGroupLayout, m_GPUUniformBuffer);

    auto blendState = WebGPUDevice::Get().CreateBlendState();

    //auto pipelineLayout = WebGPUDevice::Get().CreateRenderPipelineLayout({ m_BindGroupLayouts.begin(), m_BindGroupLayouts.end()});  // 使用范围构造函数将 map 中的值复制到 vector 中（！！！似乎不行，编译时报错）
    std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
    for (auto& item : m_BindGroupLayouts)
    {
        bindGroupLayouts.push_back(item.second);
    }
    auto pipelineLayout = WebGPUDevice::Get().CreateRenderPipelineLayout(bindGroupLayouts);
    m_Pipeline = WebGPUDevice::Get().CreateRenderPipeline(vertexBufferLayout, blendState,  pipelineLayout, m_Shader->GetShaderModule());
}

void Mesh::CreateBuffer(std::vector<VertexAttrs> vertexVec3f, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfos)
{
    // // Vertex fetch
	std::vector<wgpu::VertexAttribute> vertexAttrs(3);
	// //                                         ^ This was a 2

	// // Position attribute
	// vertexAttrs[0].shaderLocation = 0;
	// vertexAttrs[0].format = wgpu::VertexFormat::Float32x3;
	// vertexAttrs[0].offset = 0;

	// // Normal attribute
	// vertexAttrs[1].shaderLocation = 1;
	// vertexAttrs[1].format = wgpu::VertexFormat::Float32x3;
	// vertexAttrs[1].offset = offsetof(VertexAttrs, normal);

	// // Color attribute
	// vertexAttrs[2].shaderLocation = 2;
	// vertexAttrs[2].format = wgpu::VertexFormat::Float32x3;
	// vertexAttrs[2].offset = offsetof(VertexAttrs, color);

    for(uint16_t i = 0; i < 3; i++)
    {
        vertexAttrs[i].shaderLocation = vertexAttrInfos[i].location;
        vertexAttrs[i].format = vertexAttrInfos[i].format;
        vertexAttrs[i].offset = vertexAttrInfos[i].offset;
    }

    auto vertexBufferLayout = WebGPUDevice::Get().CreateVertexBufferLayout(vertexAttrs);
    m_GPUVertexBuffer = WebGPUDevice::Get().CreateVertexBuffer(vertexVec3f, "vertexAttr");
    m_GPUIndexBuffer = WebGPUDevice::Get().CreateIndexBuffer(indexBuffer, "indices");

    // 创建节点Uniform，相当于当前模型的变换矩阵
    m_NodeUniformBuffer = WebGPUDevice::Get().CreateUniformBuffer<NodeUniforms>("NodeUniforms");

    NodeUniforms nodeUniforms{};
    nodeUniforms.modelMatrix = glm::mat4(1.0f); // 使用单位矩阵进行调试
    WebGPUDevice::Get().WriteUniformBuffer(
        m_NodeUniformBuffer,
        offsetof(NodeUniforms, modelMatrix),
        &nodeUniforms.modelMatrix,
        sizeof(NodeUniforms::modelMatrix)
    );

    // 模型变换矩阵
    std::vector<UniformLayoutEntryInfo> uniformLayoutEntryInfos = {
		UniformLayoutEntryInfo {
			static_cast<uint32_t>(0),
            wgpu::ShaderStage::Vertex,
			sizeof(NodeUniforms)
		}
	};
	std::vector<wgpu::BindGroupLayoutEntry> uniformBindGroupLayoutEntries;
    WebGPUDevice::Get().CreateUniformBindGroupLayoutEntries(uniformLayoutEntryInfos, uniformBindGroupLayoutEntries);
	auto nodeBindGroupLayout = WebGPUDevice::Get().CreateBindGroupLayout(uniformBindGroupLayoutEntries);
    m_BindGroupLayouts.emplace("NodeUniforms", nodeBindGroupLayout);

	// 模型变换矩阵
	std::vector<UniformEntryInfo> uniformEntryInfos = {
		{
			0,
			m_NodeUniformBuffer,
            0,
			sizeof(NodeUniforms)
		}
	};
	std::vector<wgpu::BindGroupEntry> uniformBindGroupEntries;
	WebGPUDevice::Get().CreateUniformBindGroupEntry(uniformEntryInfos, uniformBindGroupEntries);
    auto nodeBindGroup = WebGPUDevice::Get().CreateBindGroup(uniformBindGroupEntries, nodeBindGroupLayout);
    m_BindGroups.emplace("NodeUniforms", BindGroupData{ static_cast<uint32_t>(m_BindGroups.size()), nodeBindGroup });


    // 暂时没有Material、texture等需要创建Uniform
    // m_GPUUniformBuffer = WebGPUDevice::Get().CreateUniformBuffer(baseUniforms, "baseUniforms");

    // 暂时没有Material等需要创建BindGroup
    // auto bindGroupLayout = WebGPUDevice::Get().CreateBindGroupLayout();
    // m_BindGroup = WebGPUDevice::Get().CreateBindGroup(bindGroupLayout, m_GPUUniformBuffer);

    auto blendState = WebGPUDevice::Get().CreateBlendState();

    //auto pipelineLayout = WebGPUDevice::Get().CreateRenderPipelineLayout({ m_BindGroupLayouts.begin(), m_BindGroupLayouts.end()});  // 使用范围构造函数将 map 中的值复制到 vector 中（！！！似乎不行，编译时报错）
    std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
    for (auto& item : m_BindGroupLayouts)
    {
        bindGroupLayouts.push_back(item.second);
    }
    auto pipelineLayout = WebGPUDevice::Get().CreateRenderPipelineLayout(bindGroupLayouts);
    m_Pipeline = WebGPUDevice::Get().CreateRenderPipeline(vertexBufferLayout, blendState,  pipelineLayout, m_Shader->GetShaderModule());
}

void Mesh::BuildShader()
{
    // m_Shader = std::make_unique<iEngine::ShaderModule>(RESOURCE_DIR "/shaders/wgsl/shader.wgsl", WebGPUDevice::Get().GetDevice());
    std::cout << "Build shader" << std::endl;
    std::string shaderCode = R"(
        const PI = 3.14159265359;

        /* **************** BINDINGS **************** */

        struct VertexInput {
            @location(0) position: vec3f,
            @location(1) normal: vec3f,
            @location(2) color: vec3f,
        };

        struct VertexOutput {
            @builtin(position) position: vec4f,
            @location(0) color: vec3f,
            @location(1) normal: vec3f,
        };

        /**
         * A structure holding the value of our uniforms
         */
        struct GlobalUniforms {
            projectionMatrix: mat4x4f,
            viewMatrix: mat4x4f,
            cameraWorldPosition: vec3f,
            time: f32,
            gamma: f32,
        };

        /**
         * Uniforms specific to a given GLTF node.
         */
        struct NodeUniforms {
            modelMatrix: mat4x4f,
        }

        // General bind group
        @group(0) @binding(0) var<uniform> uGlobal: GlobalUniforms;

        // Node bind group
        @group(1) @binding(0) var<uniform> uNode: NodeUniforms;

        /* **************** VERTEX MAIN **************** */

        @vertex
        fn vs_main(in: VertexInput) -> VertexOutput {
            var out: VertexOutput;

            // let worldPosition = uNode.modelMatrix * vec4f(in.position, 1.0);
            // out.position = uGlobal.projectionMatrix * uGlobal.viewMatrix * worldPosition;

            // 定义4x4的单位矩阵
            // var unitMatrix = mat4x4f(
            // 	1.0, 0.0, 0.0, 0.0,
            // 	0.0, 1.0, 0.0, 0.0,
            // 	0.0, 0.0, 1.0, 0.0,
            // 	0.0, 0.0, 0.0, 1.0
            // );
            // out.position = unitMatrix * vec4f(in.position, 1.0);
            // out.normal = (unitMatrix * vec4f(in.normal, 0.0)).xyz;

            out.position = uGlobal.projectionMatrix * uGlobal.viewMatrix * uNode.modelMatrix * vec4f(in.position, 1.0);
            // out.normal = (uNode.modelMatrix * vec4f(in.normal, 0.0)).xyz;
            out.normal = in.normal;
            out.color = in.color;
            return out;
        }

        /* **************** FRAGMENT MAIN **************** */

        @fragment
        fn fs_main(in: VertexOutput) -> @location(0) vec4f {
            // let normal = normalize(in.normal);
            let normal = vec3f(1.0, 1.0, 1.0);

            let lightColor1 = vec3f(1.0, 0.9, 0.6);
            let lightColor2 = vec3f(0.6, 0.9, 1.0);
            let lightDirection1 = vec3f(0.5, -0.9, 0.1);
            let lightDirection2 = vec3f(0.2, 0.4, 0.3);
            let shading1 = max(0.0, dot(lightDirection1, normal));
            let shading2 = max(0.0, dot(lightDirection2, normal));
            let shading = shading1 * lightColor1 + shading2 * lightColor2;
            let color = in.color * shading;
            //  let color = in.color;

            // Gamma-correction
            let corrected_color = pow(color, vec3f(2.2));
            return vec4f(corrected_color, 1.0);
        }
    )";
    m_Shader = std::make_unique<iEngine::ShaderModule>(shaderCode);
}

void Mesh::SendToBuffer()
{
    BuildShader();

    if (m_VertexVec3f.size() > 0) CreateBuffer(m_VertexVec3f, m_IndexBuffer, m_VertexAttrInfos);

    if (m_VertexBuffer.size() > 0) CreateBuffer(m_VertexBuffer, m_IndexBuffer, m_VertexAttrInfos);

    m_IsPrepared = true;
}