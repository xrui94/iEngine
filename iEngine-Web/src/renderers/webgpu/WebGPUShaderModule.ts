import { WebGPUUniforms, BindGroupEntryInfo, TextureResource } from "./WebGPUUniforms";
import { WebGLConstants } from "../webgl/WebGLConstants";


import type { Mesh } from "../../core/Mesh";
import type { Material, TextureInfo } from "../../materials/Material";
import type { Texture } from "../../textures/Texture";
import type { WebGPUContext } from "./WebGPUContext";

export function createPipelineKey(params: {
  shaderName: string;
  defines?: Record<string, any>;
  vertexLayout: string;
  renderTargetFormat: string;
  transparent: boolean;
  doubleSided: boolean;
  depthCompare: string;
  depthWrite: boolean;
  frontFace: string;
}): string {
  const defineStr = params.defines
    ? Object.entries(params.defines)
        .sort()
        .map(([k, v]) => `${k}=${v}`)
        .join(";")
    : "";

  return [
    params.shaderName,
    defineStr,
    `vtx=${params.vertexLayout}`,
    `fmt=${params.renderTargetFormat}`,
    `trans=${params.transparent ? 1 : 0}`,
    `cull=${params.doubleSided ? "none" : "back"}`,
    `depth=${params.depthCompare}:${params.depthWrite ? "w" : "r"}`,
    `face=${params.frontFace}`
  ].join("|");
}

export class WebGPUShaderModule {
    private context: WebGPUContext;
    private shaderModule: GPUShaderModule;
    private uniforms: WebGPUUniforms;
    private bindGroupLayoutEntryInfo: BindGroupEntryInfo;
    private bindGroupLayout: GPUBindGroupLayout;

    private pipelineCache: Map<string, GPURenderPipeline> = new Map();

    constructor(context: WebGPUContext, code: string/*, bindGroupLayout: GPUBindGroupLayout*/, bindGroupLayoutEntryInfo: BindGroupEntryInfo) {
        this.context = context;
        this.shaderModule = this.context.createShaderModule({
            code: code,
        });
        this.bindGroupLayoutEntryInfo = bindGroupLayoutEntryInfo;
        this.bindGroupLayout = this.context.createBindGroupLayout({
            label: 'shader_bind_group_layout',
            entries: Object.values(this.bindGroupLayoutEntryInfo) // 提取所有 value (即 GPUBindGroupLayoutEntry[] 类型)组成新的数组
        });
        this.uniforms = new WebGPUUniforms(this.context, this.bindGroupLayout);
    }

    getModule(): GPUShaderModule {
        return this.shaderModule;
    }

    getBindGroupLayoutEntryInfo(): BindGroupEntryInfo {
        return this.bindGroupLayoutEntryInfo;
    }

    getBindGroupLayout(): GPUBindGroupLayout {
        return this.bindGroupLayout;
    }

    getBindGroup(): GPUBindGroup {
        const group = this.uniforms.getBindGroup();
        if (!group) throw new Error("BindGroup has not been initialized. Call setUniforms() first.");
        return group;
    }

    setUniforms(uniformData: Record<string, any>) {
        this.uniforms.setUniforms(uniformData);
    }

    // setTextures(textureResources: { binding: number; textureView: GPUTextureView; sampler: GPUSampler }[]) {
    //     // for (const name in this.bindGroupLayoutEntryInfo) {
    //     //     const entry: GPUBindGroupLayoutEntry = this.bindGroupLayoutEntryInfo[name];
    //     //     if (!entry) {
    //     //         console.error('Failed to get the texture\'s binding');
    //     //         continue;
    //     //     }
    //     //     if (entry.texture) {
    //     //         if (textures.hasOwnProperty(name)) { // 确保只遍历对象自身的属性，而非原型链上的
    //     //             const texture: Texture = textures[name];
    //     //             if (texture.needsUpdate()) {
    //     //                 texture.upload(this.context);
    //     //                 texture.markUpdated();

    //     //                 // const { view, sampler } = this.context.getBindGroupTextureEntry(texture);
    //     //                 const view = this.context.getTextureView(texture);
    //     //                 const sampler = this.context.getSampler(texture);
    //     //                 shader.setTextures([
    //     //                     {
    //     //                         binding: entry.binding,
    //     //                         textureView: view,
    //     //                         sampler
    //     //                     }
    //     //                 ]);
    //     //             }

    //     //         } else {

    //     //             // if (this.context.defaultTexture.needsUpdate()) {
    //     //             //     this.context.defaultTexture.upload(this.context);
    //     //             //     this.context.defaultTexture.markUpdated();

    //     //             //     //
    //     //             //     const view = this.context.getTextureView(this.context.defaultTexture);
    //     //             //     const sampler = this.context.getSampler(this.context.defaultTexture);
    //     //             //     shader.setTextures([
    //     //             //         {
    //     //             //             binding: entry.binding,
    //     //             //             textureView: view,
    //     //             //             sampler
    //     //             //         }
    //     //             //     ]);
    //     //             // }
    //     //             console.error('Failed to get the texture\'s binding');
    //     //         }
    //     //     }
    //     // }
        
    //     textureResources.forEach(({ binding, textureView, sampler }) => {
    //         this.uniforms.addBinding({ binding, resource: textureView });
    //         this.uniforms.addBinding({ binding: binding + 1, resource: sampler }); // 默认 sampler 在相邻 slot
    //     });
    // }

    setTextures(textureInfo: TextureInfo) {
        const textureResources: TextureResource[] = [];
        for (const name in this.bindGroupLayoutEntryInfo) {
            const entry: GPUBindGroupLayoutEntry = this.bindGroupLayoutEntryInfo[name];
            if (!entry) {
                console.error('Failed to get the texture\'s binding');
                continue;
            }
            if (entry.texture) {
                if (!textureInfo.hasOwnProperty(name)) { // 确保只遍历对象自身的属性，而非原型链上的
                    console.error('Failed to get the texture\'s binding');
                    continue;
                }

                const texture: Texture = textureInfo[name];
                if (texture.needsUpdate()) {
                    texture.upload(this.context, true);
                    // this.context.writeTexture(texture.gpuTexture as GPUTexture, texture);
                    // texture.markUpdated();

                    // const { view, sampler } = this.context.getBindGroupTextureEntry(texture);
                    const view = this.context.getTextureView(texture);
                    const sampler = this.context.getSampler(texture);
                    textureResources.push({
                        binding: entry.binding,
                        textureView: view,
                        sampler
                    });
                }
            }
        }

        // 设置纹理
        this.uniforms.setTextures(textureResources);
    }

    createPipelineDescriptor(mesh: Mesh, material: Material): GPURenderPipelineDescriptor {
        return {
            layout: this.context.createRenderPipelineLayout({
                bindGroupLayouts: [this.bindGroupLayout]
            }),
            vertex: {
                module: this.shaderModule,
                entryPoint: 'vs_main',
                buffers: [mesh.getVertexLayout()] as Iterable<GPUVertexBufferLayout>,
            },
            fragment: {
                module: this.shaderModule,
                entryPoint: 'fs_main',
                targets: [{
                    format: 'bgra8unorm',
                    blend: material.transparent ? {
                        color: {
                            srcFactor: 'src-alpha',
                            dstFactor: 'one-minus-src-alpha',
                            operation: 'add'
                        },
                        alpha: {
                            srcFactor: 'one',
                            dstFactor: 'zero',
                            operation: 'add'
                        }
                    } : undefined
                }]
            },
            primitive: {
                topology: mesh.primitive.getGPUPrimitiveType(),
                cullMode: material.doubleSided ? 'none' : 'back',
                frontFace: 'ccw'
            },
            depthStencil: {
                depthWriteEnabled: material.depthWrite,
                depthCompare: material.depthFunc === WebGLConstants.LEQUAL ? 'less-equal' : 'less',
                format: 'depth24plus'
            }
        };
    }

    getRenderPipeline(key: string, descriptor: GPURenderPipelineDescriptor): GPURenderPipeline {
        if (this.pipelineCache.has(key)) {
            return this.pipelineCache.get(key)!;
        }

        //
        const pipeline = this.context.createRenderPipeline(descriptor);
        this.pipelineCache.set(key, pipeline);
        return pipeline;
    }

    // WebGPURenderer类中的render函数类，直接：renderPass.setBindGroup(shader.getBindGroup())即可
    // public bindUniforms(encoder: GPURenderPassEncoder, index: number = 0) {
    //     encoder.setBindGroup(index, this.getBindGroup());
    // }

    destroy(): void {
        // this.shaderModule.destroy(); // WebGPU ShaderModule 目前无法显式销毁
        this.pipelineCache.clear();
        // this.uniforms.destroy?.(); // 取决于你是否实现 destroy 方法
    }
}