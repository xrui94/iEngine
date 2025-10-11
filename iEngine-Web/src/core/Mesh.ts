import { Geometry } from '../geometries/Geometry';
import { Primitive } from './Primitive';

import type { VertexAttribute, VertexLayout } from './Primitive';
import type { Context } from '../renderers/Context';
// import { WebGLConstants } from '../renderers/webgl/WebGLConstants';
import { WebGLContext } from '../renderers/webgl/WebGLContext';
import { WebGPUContext } from '../renderers/webgpu/WebGPUContext';

type VertexAttributeData = {
    name: string,
    // size: number,
    format: string,
    shaderLocation: number,
    data?: Float32Array
}

/**
 * 未来如果需要进一步优化，可以将当前Mesh设计为基类，然后，派生出StaticMesh和DynamicMesh等子类。
 */
export class Mesh {
    public geometry: Geometry;
    public primitive: Primitive;
    public uploaded: boolean = false; // 是否已上传到GPU

    public transform: number[] = [
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    ]; // 4x4变换矩阵

    // 顶点属性描述和数据映射
    public vertexAttributes: VertexAttributeData[] = [];
    public vertexAttributeDataMap: Record<string, Float32Array | undefined> = {};
    
    // WebGL资源
    // private vao: WebGLVertexArrayObject | null = null;
    // private vbo: WebGLBuffer | null = null;
    // // private vboMap: Record<string, WebGLBuffer | null> = {};    // 记录各属性的VBO
    // private ibo: WebGLBuffer | null = null;

    public vbo: WebGLBuffer | GPUBuffer | null = null;
    public ibo: WebGLBuffer | GPUBuffer | null = null;

    constructor(geometry: Geometry, primitive: Primitive) {
        this.geometry = geometry;
        this.primitive = primitive;

        // 初始化顶点属性描述和数据映射
        // this.vertexAttributes = [
        //     { name: 'aPosition', size: 3, data: geometry.vertices },
        //     { name: 'aNormal', size: 3, data: geometry.normals },
        //     { name: 'aTexCoord', size: 2, data: geometry.texCoords },
        //     { name: 'aColor0', size: 4, data: geometry.colors0 },
        //     { name: 'aColor1', size: 4, data: geometry.colors1 },
        //     { name: 'aTangent', size: 3, data: geometry.tangents },
        //     { name: 'aBitangent', size: 3, data: geometry.bitangents },
        // ];

        this.vertexAttributes = [
            { name: 'aPosition', format: 'float32x3', shaderLocation: 0, data: geometry.vertices },
            { name: 'aNormal', format: 'float32x3', shaderLocation: 1, data: geometry.normals },
            { name: 'aTexCoord', format: 'float32x2', shaderLocation: 2, data: geometry.texCoords },
            { name: 'aColor0', format: 'float32x4', shaderLocation: 3, data: geometry.colors0 },
            { name: 'aColor1', format: 'float32x4', shaderLocation: 4, data: geometry.colors1 },
            { name: 'aTangent', format: 'float32x3', shaderLocation: 5, data: geometry.tangents },
            { name: 'aBitangent', format: 'float32x3', shaderLocation: 6, data: geometry.bitangents },
        ];

        this.vertexAttributeDataMap = {
            aPosition: geometry.vertices,
            aNormal: geometry.normals,
            aTexCoord: geometry.texCoords,
            aColor0: geometry.colors0,
            aColor1: geometry.colors1,
            aTangent: geometry.tangents,
            aBitangent: geometry.bitangents,
        };
    }

    get hasNormal(): boolean {
        return !!this.vertexAttributeDataMap.aNormal && this.vertexAttributeDataMap.aNormal.length > 0;
    }

    get hasUV(): boolean {
        return !!this.vertexAttributeDataMap.aTexCoord && this.vertexAttributeDataMap.aTexCoord.length > 0;
    }

    get hasColor0(): boolean {
        return !!this.vertexAttributeDataMap.aColor0 && this.vertexAttributeDataMap.aColor0.length > 0;
    }

    get hasColor1(): boolean {
        return !!this.vertexAttributeDataMap.aColor1 && this.vertexAttributeDataMap.aColor1.length > 0;
    }

    get hasTangent(): boolean {
        return !!this.vertexAttributeDataMap.aTangent && this.vertexAttributeDataMap.aTangent.length > 0;
    }

    get hasBitangent(): boolean {
        return !!this.vertexAttributeDataMap.aBitangent && this.vertexAttributeDataMap.aBitangent.length > 0;
    }


    static buildVertexLayout(
        attributes: VertexAttributeData[]
    ): VertexLayout {
        let stride = 0;
        for (const attr of attributes) {
            const size = Primitive.VertexFormatInfo[attr.format].size;
            if (attr.data) stride += size;
        }
        // const strideBytes = stride * 4;
        const attrs: VertexAttribute[] = [];
        let offset = 0;
        for (const attr of attributes) {
            if (attr.data) {
                // layout.push({
                //     name: attr.name,
                //     size: attr.size,
                //     type: WebGLConstants.FLOAT,
                //     normalized: false,
                //     stride: strideBytes,
                //     offset: offset * 4,
                // });
                const formatInfo = Primitive.VertexFormatInfo[attr.format];
                attrs.push({
                    name: attr.name,
                    format: attr.format,
                    // offset: offset * 4,
                    offset,
                    shaderLocation: attr.shaderLocation
                })
                offset += formatInfo.byteSize;
            }
        }

        const layout: VertexLayout = {
            arrayStride: stride * 4,
            attributes: attrs
        };

        return layout;
    }

    /**
     * 组装交错格式顶点数据
     * @param attributes 需要交错的属性列表（如 aPosition, aNormal...）
     * @param attrDataMap 属性名到数据的映射
     * @param vertexCount 顶点数量
     * @returns Float32Array 交错后的顶点数据
     */
    static buildInterleavedBuffer(
        // vertexLayout: { name: string, size: number }[],
        vertexLayout: VertexLayout,
        attrDataMap: Record<string, Float32Array | undefined>,
        vertexCount: number
    ): Float32Array {
        // 计算 stride
        let stride = 0;
        const attrs = vertexLayout.attributes;
        for (const attr of attrs) {
            const size = Primitive.VertexFormatInfo[attr.format].size;
            if (attrDataMap[attr.name]) stride += size;
        }
        const interleaved = new Float32Array(vertexCount * stride);

        for (let i = 0; i < vertexCount; i++) {
            let ptr = i * stride;
            const attrs = vertexLayout.attributes;
            for (const attr of attrs) {

                const data = attrDataMap[attr.name];
                if (data) {
                    const size = Primitive.VertexFormatInfo[attr.format].size;
                    for (let k = 0; k < size; k++) {
                        interleaved[ptr++] = data[i * size + k];
                    }
                }
            }
        }
        return interleaved;
    }

    getVertexLayout(): VertexLayout {
        // 优先用外部通过 Primitive 传进来的 VertexLayout 对象
        if (this.primitive.vertexLayout) return this.primitive.vertexLayout;
        return Mesh.buildVertexLayout(this.vertexAttributes);
    }

    getShaderMacroDefines(): Record<string, string | number | boolean> {
        const defines: Record<string, string | number | boolean> = {};
        if (this.hasNormal) defines.HAS_NORMAL =  true;
        if (this.hasUV) defines.HAS_TEXCOORD = true;
        if (this.hasColor0) defines.HAS_COLOR0 = true;
        if (this.hasColor1) defines.HAS_COLOR1 = true;
        if (this.hasTangent) defines.HAS_TANGENT = true;
        if (this.hasBitangent) defines.HAS_BITANGENT = true;
        return defines;
    }

    /**
     * 上传几何体数据到GPU
     * @param context WebGL上下文
     * @param shader 着色器程序
     */
    // upload(context: WebGLContext, shader: WebGLShaderProgram): void {
    //     if (!(shader.program instanceof WebGLProgram)) {
    //         console.error('Shader program is not a valid WebGLShader');
    //         return;
    //     }

    //     // 1. 组装交错数据，并生成VertexLayout
    //     const vertexCount = this.geometry.vertexCount;
    //     const vertexLayout = Mesh.buildVertexLayout(
    //         this.vertexAttributes, context.getDevice()
    //     );
    //     const interleavedVertexBuffer = Mesh.buildInterleavedBuffer(
    //         vertexLayout, this.vertexAttributeDataMap, vertexCount
    //     );

    //     // 2. 创建并绑定顶点数组对象 VAO
    //     this.vao = context.createVAO();
    //     if (!this.vao) {
    //         console.error('Failed to create VAO');
    //         return;
    //     }
    //     context.bindVAO(this.vao);

    //     // 创建并绑定顶点缓冲区 VBO
    //     this.vbo = context.createVertexBuffer(interleavedVertexBuffer);
    //     context.bindVertexBuffer(this.vbo);

    //     // 设置顶点 attribute 指针
    //     context.setVertexAttrPointer(shader.program, vertexLayout);

    //     // 创建并绑定索引缓冲区 IBO
    //     if (this.geometry.indices) {
    //         this.ibo = context.createIndexBuffer(this.geometry.indices);
    //         context.bindIndexBuffer(this.ibo);
    //     }

    //     // 解绑 VAO，并标记顶点数据、索引数据为已上传的状态
    //     context.bindVAO(null);
    //     this.uploaded = true;
    // }

    // getVertexArrayObject(): WebGLVertexArrayObject | null {
    //     return this.vao;
    // }

    /**
     * 上传顶点数据和索引数据到GPU
     * @param context GPU上下文
     */
    upload(context: Context, force: boolean = false): void {
        if (this.uploaded && !force) return;

        //
        const vertexCount = this.geometry.vertexCount;
        const vertexLayout = this.getVertexLayout();
        const interleavedVertexBuffer = Mesh.buildInterleavedBuffer(
            vertexLayout, this.vertexAttributeDataMap, vertexCount
        );

        if (this.vbo) context.deleteBuffer(this.vbo); // 清理旧 buffer
        
        // 创建顶点数据Buffer对象
        this.vbo = context.createVertexBuffer({
            byteLength: interleavedVertexBuffer.byteLength
        });
        context.writeBuffer({
            buffer: this.vbo,
            data: interleavedVertexBuffer,
            target: (context as WebGLContext).getDevice().ARRAY_BUFFER
        }); // 当前Mesh为静态Mesh，后续须将类名改为StaticMesh，由于是静态Mesh，因此，这里立即写入数据，其实动态Mesh，也可以写入，然后帧循环时，更新即可

        // 创建索引数据Buffer对象
        if (this.geometry.indices) {
            if (this.ibo) context.deleteBuffer(this.ibo); // 清理旧 buffer
            this.ibo = context.createIndexBuffer({
                byteLength: this.geometry.indices.byteLength

            });
            context.writeBuffer({
                buffer: this.ibo,
                data: this.geometry.indices,
                target: (context as WebGLContext).getDevice().ELEMENT_ARRAY_BUFFER
            }); // 当前Mesh为静态Mesh，后续须将类名改为StaticMesh，由于是静态Mesh，因此，这里立即写入数据，其实动态Mesh，也可以写入，然后帧循环时，更新即可
        }

        // 标记为已上传
        this.uploaded = true;
    }

    /**
     * 动态更新已有 GPU buffer 的内容，主要用于DynamicMesh
     * @param context GPU 上下文
     * @param options { vertexData?, indexData?, vertexOffset?, indexOffset? }
     */
    updateBuffer(
        context: WebGLContext | WebGPUContext,
        options: {
            vertexData?: Float32Array,
            indexData?: Uint16Array | Uint32Array,
            vertexOffset?: number,
            indexOffset?: number
        }
    ): void {
        if (!this.uploaded) {
            console.warn('Mesh not uploaded yet, cannot updateBuffer.');
            return;
        }

        if (options.vertexData && this.vbo) {
            context.writeBuffer({
                buffer: this.vbo,
                data: options.vertexData,
                target: (context as WebGLContext).getDevice().ARRAY_BUFFER,
                offset: options.vertexOffset ?? 0
            });
        }
        if (options.indexData && this.ibo) {
            context.writeBuffer({
                buffer: this.ibo,
                data: options.indexData,
                target: (context as WebGLContext).getDevice().ELEMENT_ARRAY_BUFFER,
                offset: options.indexOffset ?? 0
            });
        }
    }

}