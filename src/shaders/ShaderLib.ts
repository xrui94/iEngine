import { ShaderPreprocessor } from './ShaderPreprocessor';
import { BaseMaterialShader } from './glsl/BaseMaterialShader';
import { BaseMaterialShader as BaseMaterialWGSL } from './wgsl/BaseMaterialShader.wgsl';
import { BaseWireframeShader } from './glsl/BaseWireframeShader';
import { BaseWireframeShader as BaseWireframeWGSL } from './wgsl/BaseWireframeShader.wgsl';
import { BasePhongShader } from './glsl/BasePhongShader';
// import { BasePhongShader as BasePhongWGSL } from './webgpu/lib/BasePhongShader.wgsl';
import { BasePbrShader } from './glsl/BasePbrShader';
import { BasePbrShader as BasePbrWGSL } from './wgsl/BasePbrShader.wgsl';

import type { GraphicsAPI } from '../renderers/Renderer';
import type { UniformInfo } from '../renderers/webgl/WebGLUniforms';
import type { BindGroupEntryInfo } from '../renderers/webgpu/WebGPUUniforms';


export type DefineMap = Record<string, string | number | boolean>;

export type ShaderVariantOptions = {
    defines?: DefineMap;
    overrideDefaultDefines?: boolean;
};

type GLSLSource = {
    vertCode: string | null;
    fragCode: string | null;
    defines?: DefineMap;
    uniforms?: UniformInfo[];
};

type WGSLSource = {
    code: string | null;
    defines?: DefineMap;
    // bindGroupEntries?: GPUBindGroupLayoutEntry[];
    bindGroupEntryInfos?: BindGroupEntryInfo,
};

type ShaderVariantKey = string;

export type ShaderVariants = {
    /** WebGL 的 GLSL 源码 */
    webgl?: GLSLSource;
    /** WebGPU 的 WGSL 源码 */
    webgpu?: WGSLSource;
};

// export type ProcessedShader = {
//     glsl?: {
//         vert?: string;
//         frag?: string;
//     };
//     wgsl?: string;
// };

export const BaseShaders = [
    'base_material',
    'base_wireframe',
    'base_phong',
    'base_pbr'
]

export function makeShaderVariantKey(shaderName: string, defines: DefineMap): string {
    // const sorted = Object.keys(defines).sort().map(k => `${k}=${defines[k]}`).join(';');
    // return `${shaderName}__${sorted}`;
    if (!defines || Object.keys(defines).length === 0) return shaderName;
    const sorted = Object.entries(defines)
        .sort(([a], [b]) => a.localeCompare(b))
        .map(([k, v]) => `${k}=${v}`)
        .join(';');
    return `${shaderName}__${sorted}`;
}

export class ShaderLib {
    // 注册的shader
    private static readonly shaders: Record<string, ShaderVariants> = {};
    // 变体缓存
    private static readonly processedShaders: Record<ShaderVariantKey, ShaderVariants> = {};

    static register(name: string, variants: ShaderVariants): void {
        this.shaders[name] = variants;
    }

    static unregister(name: string): void {
        delete this.shaders[name];
        // 清理所有相关变体缓存
        Object.keys(this.processedShaders).forEach(k => {
            if (k.startsWith(name + '__')) {
                delete this.processedShaders[k];
            }
        });
    }

    static clearCache(): void {
        Object.keys(this.processedShaders).forEach(k => delete this.processedShaders[k]);
    }

    // 获取shader变体（自动处理宏和后端）
    static getVariant(
        name: string,
        backend: GraphicsAPI,
        options?: ShaderVariantOptions
    ): ShaderVariants | null {
        // const key = name + '_' + backend + '_' + JSON.stringify(defines);
        // if (this.processedShaders[key]) return this.processedShaders[key];

        // // const shader = this.shaders[name]?.[backend];
        // // if (!shader) return null;

        // const shader = backend === 'webgpu'
        //     ? this.shaders[name]?.webgpu
        //     : this.shaders[name]?.webgl;
        // if (!shader) return null;

        // // 合并注册时和调用时的 defines
        // const mergedDefines = { ...(shader.defines || {}), ...defines };

        // let vertex: string | undefined = shader.vertex;
        // let fragment: string | undefined = shader.fragment;
        // let wgsl: string | undefined = shader.wgsl;

        // if (backend === 'webgl1' || backend === 'webgl2') {
        //     vertex = vertex
        //         ? ShaderPreprocessor.preprocess(vertex, backend, 'vertex', mergedDefines)
        //         : undefined;
        //     fragment = fragment
        //         ? ShaderPreprocessor.preprocess(fragment, backend, 'fragment', mergedDefines)
        //         : undefined;
        // } else if (backend === 'webgpu') {
        //     // 可扩展：对WGSL做宏替换（如有需要）
        //     // 这里简单插入 #define 宏到 WGSL 顶部（可根据WGSL规范调整）
        //     if (wgsl) {
        //         const macroStr = Object.entries(mergedDefines)
        //             .map(([k, v]) => `// #define ${k} ${v}`)
        //             .join('\n');
        //         wgsl = macroStr + '\n' + wgsl;
        //     }
        // }

        // const result = { vertex, fragment, wgsl };
        // this.processedShaders[key] = result;
        // return result;

        const shader = backend === 'webgpu'
            ? this.shaders[name]?.webgpu
            : this.shaders[name]?.webgl;
        if (!shader) return null;

        const override = options?.overrideDefaultDefines ?? true;
        // const mergedDefines: DefineMap = options?.overrideDefaultDefines
        //     ? (options.defines || {})
        //     : { ...(shader.defines || {}), ...(options?.defines || {}) };
        const mergedDefines = override
            ? { ...(shader.defines || {}), ...options?.defines }
            : { ...options?.defines, ...(shader.defines || {}) };

        const variantKey = makeShaderVariantKey(name, mergedDefines) + `__${backend}`;
        if (this.processedShaders[variantKey]) return this.processedShaders[variantKey];

        let result: ShaderVariants = {};

        if (backend === 'webgl1' || backend === 'webgl2') {
            const glsl = (shader as GLSLSource);
            const vertCode = glsl.vertCode
                ? ShaderPreprocessor.preprocessGlsl(glsl.vertCode, backend, 'vertex', mergedDefines)
                : null;
            const fragCode = glsl.fragCode
                ? ShaderPreprocessor.preprocessGlsl(glsl.fragCode, backend, 'fragment', mergedDefines)
                : null;
            result.webgl = {
                vertCode,
                fragCode,
                defines: glsl.defines,
                uniforms: glsl.uniforms
            };
        } else if (backend === 'webgpu') {
            const wgsl = shader as WGSLSource;
            if (wgsl.code && wgsl.code.length > 0) {
                // const macroStr = Object.entries(mergedDefines)
                //     .map(([k, v]) => `// #define ${k} ${v}`)
                //     .join('\n');
                // const code = macroStr + '\n' + wgsl.code;
                const code = ShaderPreprocessor.preprocessWgsl(wgsl.code, mergedDefines);
                result.webgpu = {
                    code,
                    defines: wgsl.defines,
                    bindGroupEntryInfos: wgsl.bindGroupEntryInfos
                };
            } else {
                result.webgpu = { code: null }
            }
        }

        this.processedShaders[variantKey] = result;
        return result;
    }

    // 获取所有shader名
    static getAllShaderNames(): string[] {
        return Object.keys(this.shaders);
    }

    // static getAllShaders(graphicsApi: GraphicsAPI): [string, { vertex: string, fragment: string }][] {
    //     Object.keys(ShaderLib.shaders).forEach(name => {
    //         if (!this.processedShaders[name]) {
    //             const processed = this.preprocessShader(name, graphicsApi);
    //             if (processed) {
    //                 this.processedShaders[name] = processed;
    //             }
    //         }
    //     });

    //     return Object.entries(ShaderLib.processedShaders);
    // }

    // static get(name: string): { vertex: string, fragment: string } | null {
    //     return this.shaders[name] || null;
    // }

}

export function registerBuiltInShaders() {
    ShaderLib.register('base_material', {
        webgl: {
            vertCode: BaseMaterialShader.vertex,
            fragCode: BaseMaterialShader.fragment
        },
        webgpu: {
            code: BaseMaterialWGSL,
            bindGroupEntryInfos: {
                uniforms: {
                    binding: 0,
                    visibility: GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT,
                    buffer: { type: 'uniform' }
                }
            }
        } 
    });

    ShaderLib.register('base_wireframe', {
        webgl: {
            vertCode: BaseWireframeShader.vertex,
            fragCode: BaseWireframeShader.fragment
        },
        webgpu: {
            code: BaseWireframeWGSL,
            bindGroupEntryInfos: {
                uniforms: {
                    binding: 0,
                    visibility: GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT,
                    buffer: { type: 'uniform' }
                }
            }
        }
    });

    ShaderLib.register('base_phong', {
        webgl: {
            vertCode: BasePhongShader.vertex,
            fragCode: BasePhongShader.fragment,
            defines: {
                HAS_COLOR: true
            }
        },
        webgpu: {
            code: ''
        }
    });
    ShaderLib.register('base_pbr', {
        webgl: {
            vertCode: BasePbrShader.vertex,
            fragCode: BasePbrShader.fragment,
            defines: {
                HAS_COLOR: true
            },
            uniforms: [
                { name: 'u_ModelMatrix', type: 'mat4' },
                { name: 'u_BaseColor', type: 'vec4' },
                { name: 'u_BaseColorMap', type: 'sampler2D' }
            ]
        },
        webgpu: {
            code: BasePbrWGSL,
            // bindGroupEntryInfos: [
            //     { 
            //         name: 'uniforms',
            //         value: { binding: 0, visibility: GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, buffer: { type: 'uniform' } },
            //     },
            //     // uBaseColorMap
            //     { 
            //         name: 'uBaseColorMap',
            //         value: { binding: 1, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
            //     },
            //     { 
            //         name: 'uBaseColorSampler',
            //         value: { binding: 2, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
            //     },
            //     // uMetallicRoughnessMap
            //     { 
            //         name: 'uMetallicRoughnessMap',
            //         value: { binding: 3, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
            //     },
            //     { 
            //         name: 'uMetallicRoughnessSampler',
            //         value: { binding: 4, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
            //     },
            //     // uNormalMap
            //     { 
            //         name: 'uNormalMap',
            //         value: { binding: 5, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
            //     },
            //     { 
            //         name: 'uNormalSampler',
            //         value: { binding: 6, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
            //     },
            //     // uAoMap
            //     { 
            //         name: 'uAoMap',
            //         value: { binding: 7, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
            //     },
            //     { 
            //         name: 'uAoSampler',
            //         value: { binding: 8, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
            //     },
            //     // uEmissiveMap
            //     { 
            //         name: 'uEmissiveMap',
            //         value: { binding: 9, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
            //     },
            //     { 
            //         name: 'uEmissiveSampler',
            //         value: { binding: 10, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
            //     }
            // ]
            bindGroupEntryInfos: {
                uniforms: {
                    binding: 0,
                    visibility: GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT,
                    buffer: { type: 'uniform' }
                },
                uBaseColorMap: {
                    binding: 1,
                    visibility: GPUShaderStage.FRAGMENT,
                    texture: { sampleType: 'float' }
                },
                uBaseColorSampler: {
                    binding: 2,
                    visibility: GPUShaderStage.FRAGMENT,
                    sampler: { type: 'filtering' }
                },
                uMetallicRoughnessMap: {
                    binding: 3,
                    visibility: GPUShaderStage.FRAGMENT,
                    texture: { sampleType: 'float' }
                },
                uMetallicRoughnessSampler: {
                    binding: 4,
                    visibility: GPUShaderStage.FRAGMENT,
                    sampler: { type: 'filtering' }
                },
                uNormalMap: {
                    binding: 5,
                    visibility: GPUShaderStage.FRAGMENT,
                    texture: { sampleType: 'float' }
                },
                uNormalSampler: {
                    binding: 6,
                    visibility: GPUShaderStage.FRAGMENT,
                    sampler: { type: 'filtering' }
                },
                uAoMap: {
                    binding: 7,
                    visibility: GPUShaderStage.FRAGMENT,
                    texture: { sampleType: 'float' }
                },
                uAoSampler: {
                    binding: 8,
                    visibility: GPUShaderStage.FRAGMENT,
                    sampler: { type: 'filtering' }
                },
                uEmissiveMap: {
                    binding: 9,
                    visibility: GPUShaderStage.FRAGMENT,
                    texture: { sampleType: 'float' }
                },
                uEmissiveSampler: {
                    binding: 10,
                    visibility: GPUShaderStage.FRAGMENT,
                    sampler: { type: 'filtering' }
                }
            }
        } 
    });
}