import { IUniform } from '../IUniform'
import { iMath } from '../../math/Math';

import type { WebGPUContext } from './WebGPUContext';
import { Matrix4 } from '../../math/Matrix4';



// export type BindGroupEntryInfo = {
//     binding: number;
//     visibility: number;
//     buffer?: { type: 'uniform' | 'storage' };
//     sampler?: {};
//     texture?: {};
// };

export type BindGroupEntryInfo = Record<string, GPUBindGroupLayoutEntry>;

export type TextureResource = { binding: number; textureView: GPUTextureView; sampler: GPUSampler };

export class WebGPUUniforms implements IUniform{
    private context: WebGPUContext;
    private bindGroupLayout: GPUBindGroupLayout;
    // private bindGroupEntries: GPUBindGroupEntry[] = [];
    // 资源绑定（binding号 -> entry），用于创建bindGroup
    private bindGroupEntries: Map<number, GPUBindGroupEntry> = new Map();
    private bindGroup: GPUBindGroup | null = null;
    private uniformBuffer: GPUBuffer | null = null;

    // 挂起待同步的数据
    private _pendingUniforms: Record<string, any> = {};
    private _pendingTextures: TextureResource[] = [];

    private _dirty: boolean = false;

    constructor(context: WebGPUContext, bindGroupLayout: GPUBindGroupLayout) {
        this.context = context;
        this.bindGroupLayout = bindGroupLayout;
    }

    // 工具函数示例：打包 uniform 数据
    private packUniformData(uniforms: Record<string, any>): Float32Array {
        const data: number[] = [];
        // for (const key in uniforms) {
        //     const v = uniforms[key];
        //     if (typeof v === 'number') {
        //         data.push(v);
        //     } else if (Array.isArray(v)) {
        //         data.push(...v);
        //         if (v.length === 3) data.push(0);
        //     } else if ('x' in v && 'y' in v && 'z' in v/* && 'w' in v */) {
        //         data.push(v.x, v.y, v.z);
        //         if ('w' in v) data.push(v.w);
        //         else data.push(0);
        //     } else if ('elements' in v) {
        //         data.push(...v.elements);
        //         // if (v.length === 9) data.push(0, 0, 0);
        //     }
        //     // ...如有其它类型，则继续判断
        // }

        // data.push(...uniforms.uModelViewMatrix.elements);      // 16
        // data.push(...uniforms.uProjectionMatrix.elements);     // 16
        // data.push(...uniforms.uNormalMatrix.elements);         // 9
        // // data.push(0,0,0);                           // 补齐 mat3x3 到 12
        // data.push(...uniforms.uCameraPos); data.push(0);      // 3 + 1
        // data.push(...uniforms.uBaseColor); data.push(0);      // 3 + 1
        // data.push(uniforms.uMetallic);
        // data.push(uniforms.uRoughness);
        // data.push(...uniforms.uLightDir); data.push(0);       // 3 + 1
        // data.push(...uniforms.uLightColor); data.push(0);     // 3 + 1
        // data.push(uniforms.uLightIntensity);
        // data.push(...uniforms.uAmbientColor); data.push(0);   // 3 + 1
        // data.push(uniforms.uAoStrength);
        // data.push(uniforms.uNormalScale);


        if (uniforms.uModelViewMatrix) data.push(...uniforms.uModelViewMatrix.elements);
        if (uniforms.uProjectionMatrix) data.push(...uniforms.uProjectionMatrix.elements);
        if (uniforms.uNormalMatrix) {
            // data.push(...uniforms.uNormalMatrix.elements);
            const m = uniforms.uNormalMatrix.elements;
            // if (m.length === 9) {
            //     data.push(m[0], m[1], m[2], 0);
            //     data.push(m[3], m[4], m[5], 0);
            //     data.push(m[6], m[7], m[8], 0);
            // } else {
            //     // 其它情况按实际补齐
            //     data.push(...m);
            // }
            const normalMatrix = new Matrix4([
                m[0], m[3], m[6], 0,
                m[1], m[4], m[7], 0,
                m[2], m[5], m[8], 0,
                0,    0,    0,    1
            ]);
            data.push(...normalMatrix.elements);
        }

        // 相机位置
        if (uniforms.uCameraPos) {
            data.push(...uniforms.uCameraPos);
            if (uniforms.uCameraPos.length === 3) data.push(0); // padding，补齐4 float
        }

        // 基础颜色、金属光泽度、粗糙度
        if (uniforms.uBaseColor) {
            data.push(...(uniforms.uBaseColor.toArray ? uniforms.uBaseColor.toArray() : uniforms.uBaseColor));
            if (uniforms.uBaseColor.length === 3) data.push(0); // padding，补齐4 float
        }
        if ('uMetallic' in uniforms) data.push(uniforms.uMetallic ?? 0);
        if ('uRoughness' in uniforms) data.push(uniforms.uRoughness ?? 1);
        
        // 法线贴图
        if ('uNormalScale' in uniforms) data.push(uniforms.uNormalScale ?? 1);

        // 环境遮蔽
        if ('uAoStrength' in uniforms) data.push(uniforms.uAoStrength ?? 1);
        
        // 自发光
        if (uniforms.uEmissiveColor) {
            data.push(...(uniforms.uEmissiveColor.toArray ? uniforms.uEmissiveColor.toArray() : uniforms.uEmissiveColor));
            if (uniforms.uEmissiveColor.length === 3) data.push(0); // padding，补齐4 float
        }
        if ('uEmissiveIntensity' in uniforms) {
            data.push(uniforms.uEmissiveIntensity ?? 1);
            data.push(0, 0, 0); // padding，补齐4 float
        }
        
        // 环境光
        if (uniforms.uAmbientColor) {
            data.push(...(uniforms.uAmbientColor.toArray ? uniforms.uAmbientColor.toArray() : uniforms.uAmbientColor));
            if (uniforms.uAmbientColor.length === 3) data.push(0); // padding，补齐4 float
        }

        // 平行光源
        if (uniforms.uLightDir) {
            data.push(...(uniforms.uLightDir.toArray ? uniforms.uLightDir.toArray() : uniforms.uLightDir));
            if (uniforms.uLightDir.length === 3) data.push(0); // padding，补齐4 float
        }
        if (uniforms.uLightColor) {
            data.push(...(uniforms.uLightColor.toArray ? uniforms.uLightColor.toArray() : uniforms.uLightColor));
            if (uniforms.uLightColor.length === 3) data.push(0); // padding，补齐4 float
        }
        if ('uLightIntensity' in uniforms) {
            data.push(uniforms.uLightIntensity ?? 1);
            data.push(0, 0, 0,0); // padding，补齐4 float
        }

        // data.push(0); // padding，补齐4 float

        // 最后补齐到 16 字节倍数
        while (data.length % 4 !== 0) {
            data.push(0);
        }

        return new Float32Array(data);
    }

    // WebGPU 的 uniform buffer（UBO）必须是 16 字节对齐的，并且 struct 总大小也要对齐到 16 字节的倍数。
    createOrUpdateUniformBuffer(data: Float32Array): void {
        const alignedSize = iMath.getAlignedSize(data.byteLength);
        // const buffer = this.device.createBuffer({
        //     label: 'uniformBuffer',
        //     size: data.byteLength,
        //     usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
        //     mappedAtCreation: true
        // });
        // new Float32Array(buffer.getMappedRange()).set(data);
        // buffer.unmap();
        // this.uniformBuffer = buffer;
        // return buffer;

        if (!this.uniformBuffer || this.uniformBuffer.size < alignedSize) {
            // 创建一次大于或等于对齐后的 buffer
            this.uniformBuffer = this.context.createUniformBuffer({
                label: 'uniformBuffer',
                byteLength: alignedSize,
                usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
            });
            this.bindGroup = null; // 绑定的资源发生变化
        }

        // 直接写入数据
        this.context.writeBuffer({
            buffer: this.uniformBuffer,
            data,
            offset: 0,
            mappedAtCreation: false
        });
    }
    
    /** 内部方法：同步所有资源并重建BindGroup（如有必要） */
    private _syncResources() {
        if (!this._dirty) return;

        // 1. 更新UniformBuffer
        if (this._pendingUniforms) {
            const data = this.packUniformData(this._pendingUniforms);
            if (!this.uniformBuffer || this.uniformBuffer.size < data.byteLength) {
                this.createOrUpdateUniformBuffer(data);
                this.bindGroup = null; // buffer替换需重建bindGroup
            }
            if (!this.uniformBuffer) throw new Error('Failed to create uniform buffer');
            this.context.writeBuffer({
                buffer: this.uniformBuffer,
                data,
                offset: 0,
                mappedAtCreation: false
            });

            // binding 0: uniform buffer
            this.bindGroupEntries.set(0, { binding: 0, resource: { buffer: this.uniformBuffer } });
        }

        // 2. 更新纹理和采样器
        if (this._pendingTextures) {
            for (const { binding, textureView, sampler } of this._pendingTextures) {
                this.bindGroupEntries.set(binding, { binding, resource: textureView });
                this.bindGroupEntries.set(binding + 1, { binding: binding + 1, resource: sampler }); // 假设采样器紧挨纹理
            }
        }

        // 3. 创建BindGroup
        this.bindGroup = this.context.createBindGroup({
            layout: this.bindGroupLayout,
            entries: Array.from(this.bindGroupEntries.values())
        });

        this._dirty = false;
    }

    /** 设置uniform数据（buffer类型） */
    setUniforms(uniforms: Record<string, any>) {
        this._pendingUniforms = uniforms;
        this._dirty = true;
    }

    /** 设置纹理和采样器资源 */
    setTextures(textureResources: TextureResource[]) {
        this._pendingTextures = textureResources;
        this._dirty = true;
    }

    /** 获取最新的BindGroup，自动同步所有资源 */
    getBindGroup(): GPUBindGroup {
        this._syncResources();
        if (!this.bindGroup) throw new Error('BindGroup has not been initialized.');
        return this.bindGroup;
    }

    // setUniforms(uniforms: Record<string, any>): void {
    //     // 将所有数据转换为 Float32Array 并上传至 uniformBuffer
    //     const data = this.packUniformData(uniforms);
        
    //     if (!this.uniformBuffer || this.uniformBuffer.size < data.byteLength) {
    //         // 创建或重建较大的 buffer
    //         const alignedSize = Math.ceil(data.byteLength / 16) * 16; // 16 字节对齐
    //         this.createOrUpdateUniformBuffer(data);

    //         // ⚠️ Buffer 替换后必须重建 BindGroup
    //         this.bindGroup = null;
    //     }

    //     // 更新数据，即将数据写入到 buffer
    //     if (!this.uniformBuffer) {
    //         throw 'Failed create uniform buffer';
    //     }
    //     this.device.queue.writeBuffer(this.uniformBuffer, 0, data);

    //     // 如果没有创建 bindGroup，或者 buffer 换了（支持热更新），就重新创建
    //     if (!this.bindGroup) {
    //         this.bindGroup = this.createBindGroup(
    //             this.bindGroupLayout,
    //             [
    //                 {
    //                     binding: 0,
    //                     resource: { buffer: this.uniformBuffer }
    //                 },
    //                 // ...this.bindGroupEntries
    //                 ...Array.from(this.bindGroupEntries.values())
    //             ]
    //         );
    //     }
    // }

    // addBinding(entry: GPUBindGroupEntry): void {
    //     // this.bindGroupEntries.push(entry);
    //     this.bindGroupEntries.set(entry.binding, entry);
    //     this.bindGroup = null; // 有变更，标记需重建
    // }

    // getBindGroup(): GPUBindGroup | null {
    //     return this.bindGroup;
    // }

    set(name: string, value: any): void {
        // WebGPU 中不逐个 set，而是整体写 buffer，故此处可留空或做缓存
    }

    bind(resourceIndex: number = 0): void {
        // bindGroup 由外部提前创建，调用前 setPipeline/setBindGroup 时传入
    }

    
}