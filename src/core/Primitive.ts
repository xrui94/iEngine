import { WebGLConstants } from "../renderers/webgl/WebGLConstants";

export enum PrimitiveType {
    POINTS = 'POINTS',
    LINES = 'LINES',
    LINE_STRIP = 'LINE_STRIP',
    TRIANGLES = 'TRIANGLES',
    TRIANGLE_STRIP = 'TRIANGLE_STRIP'
}


// Vertex format	Data type	Components	byteSize	Example WGSL type
// "uint8"	unsigned int	1	1	u32
// "uint8x2"	unsigned int	2	2	vec2<u32>
// "uint8x4"	unsigned int	4	4	vec4<u32>
// "sint8"	signed int	1	1	i32
// "sint8x2"	signed int	2	2	vec2<i32>
// "sint8x4"	signed int	4	4	vec4<i32>
// "unorm8"	unsigned normalized	1	1	f32
// "unorm8x2"	unsigned normalized	2	2	vec2<f32>
// "unorm8x4"	unsigned normalized	4	4	vec4<f32>
// "snorm8"	signed normalized	1	1	f32
// "snorm8x2"	signed normalized	2	2	vec2<f32>
// "snorm8x4"	signed normalized	4	4	vec4<f32>
// "uint16"	unsigned int	1	2	u32
// "uint16x2"	unsigned int	2	4	vec2<u32>
// "uint16x4"	unsigned int	4	8	vec4<u32>
// "sint16"	signed int	1	2	i32
// "sint16x2"	signed int	2	4	vec2<i32>
// "sint16x4"	signed int	4	8	vec4<i32>
// "unorm16"	unsigned normalized	1	2	f32
// "unorm16x2"	unsigned normalized	2	4	vec2<f32>
// "unorm16x4"	unsigned normalized	4	8	vec4<f32>
// "snorm16"	signed normalized	1	2	f32
// "snorm16x2"	signed normalized	2	4	vec2<f32>
// "snorm16x4"	signed normalized	4	8	vec4<f32>
// "float16"	float	1	2	f32
// "float16x2"	float	2	4	vec2<f16>
// "float16x4"	float	4	8	vec4<f16>
// "float32"	float	1	4	f32
// "float32x2"	float	2	8	vec2<f32>
// "float32x3"	float	3	12	vec3<f32>
// "float32x4"	float	4	16	vec4<f32>
// "uint32"	unsigned int	1	4	u32
// "uint32x2"	unsigned int	2	8	vec2<u32>
// "uint32x3"	unsigned int	3	12	vec3<u32>
// "uint32x4"	unsigned int	4	16	vec4<u32>
// "sint32"	signed int	1	4	i32
// "sint32x2"	signed int	2	8	vec2<i32>
// "sint32x3"	signed int	3	12	vec3<i32>
// "sint32x4"	signed int	4	16	vec4<i32>
// "unorm10-10-10-2"	unsigned normalized	4	4	vec4<f32>
// "unorm8x4-bgra"	unsigned normalized	4	4	vec4<f32>

/**
 * 基于WebGPU的“GPUVertexFormat”完善，这里增加了“size”属性
 */
// export type VertexFormat =
//     | "uint8"
//     | "uint8x2"
//     | "uint8x4"
//     | "sint8"
//     | "sint8x2"
//     | "sint8x4"
//     | "unorm8"
//     | "unorm8x2"
//     | "unorm8x4"
//     | "snorm8"
//     | "snorm8x2"
//     | "snorm8x4"
//     | "uint16"
//     | "uint16x2"
//     | "uint16x4"
//     | "sint16"
//     | "sint16x2"
//     | "sint16x4"
//     | "unorm16"
//     | "unorm16x2"
//     | "unorm16x4"
//     | "snorm16"
//     | "snorm16x2"
//     | "snorm16x4"
//     | "float16"
//     | "float16x2"
//     | "float16x4"
//     | "float32"
//     | "float32x2"
//     | "float32x3"
//     | "float32x4"
//     | "uint32"
//     | "uint32x2"
//     | "uint32x3"
//     | "uint32x4"
//     | "sint32"
//     | "sint32x2"
//     | "sint32x3"
//     | "sint32x4"
//     | "unorm10-10-10-2"
//     | "unorm8x4-bgra";

export type VertexAttribute = {
    /**
     * 该属性仅对WebGL有效
     */
    name: string;

    // /**
    //  * 
    //  * 该参数仅用于WebGL，对于WebGPU无效，描述当前顶点的数据类型，可参考：
    //  * <a href="https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/vertexAttribPointer">"vertexAttribPointer</a>
    //  * 该参数的有效值如下：
    //  * gl.BYTE: signed 8-bit integer, with values in [-128, 127]
    //  * gl.SHORT: signed 16-bit integer, with values in [-32768, 32767]
    //  * gl.UNSIGNED_BYTE: unsigned 8-bit integer, with values in [0, 255]
    //  * gl.UNSIGNED_SHORT: unsigned 16-bit integer, with values in [0,65535]
    //  * gl.FLOAT: 32-bit IEEE floating point number
    //  * 
    //  * 
    //  * 如下有效值，仅对WebGL2有效
    //  * gl.HALF_FLOAT: 16-bit IEEE floating point number
    //  * gl.INT: 32-bit signed binary integer
    //  * gl.UNSIGNED_INT: 32-bit unsigned binary integer
    //  * gl.INT_2_10_10_10_REV: 32-bit signed integer with values in [-512, 511]
    //  * gl.UNSIGNED_INT_2_10_10_10_REV: 32-bit unsigned integer with values in [0, 1023]
    //  * 
    //  */
    // type: GLenum;

    // /**
    //  * 该属性仅对WebGL有效
    //  */
    // size: GLint;

    // /**
    //  * 该属性仅对WebGL有效
    //  */
    // normalized: boolean;

    /**
     * 该属性仅对WebGPU有效，相当于WebGL的“type” + “size” + “normalized”的组合
     */
    format: string;

    /**
     * 
     */
    offset: GLintptr | GPUSize64;

    /**
     * 
     */
    shaderLocation: GLuint| GPUIndex32;
};

/**
 * 定义兼容WebGL和WebGPU中的相关要求。对于WebGPU，其于“GPUVertexBufferLayout”对象的定义相当，
 * 即用于创建“GPURenderPipeline”实例的参数类型的，“GPURenderPipelineDescriptor”对象中的，
 * “GPUVertexState”字段下的“GPUVertexBufferLayout”对象
 * 
 */
export type VertexLayout = {
    /**
     * 
     */
    arrayStride: GLsizei | GPUSize64;

    /**
     * 
     */
    attributes: VertexAttribute[],

    /**
     * 有效值："vertex"、"instance";
     */
    stepMode?: GPUVertexStepMode
};

export class Primitive {
    public type: PrimitiveType;

    public vertexLayout: VertexLayout | null;

    public renderState: {
        depthTest: boolean;
        depthWrite: boolean;
        cullFace: boolean;
    };

     /**
     * WebGPU格式到WebGL属性的辅助映射表
     * <a herf="https://gpuweb.github.io/gpuweb/#enumdef-gpuvertexformat">gpu vertexformat</a>
     */
    public static VertexFormatInfo: Record<string, {
        type: number,        // WebGL type (如 gl.FLOAT)
        size: number,        // 分量数
        normalized: boolean, // 是否归一化
        byteSize: number     // 单个属性占用字节数
    }> = {
        "uint8":        { type: 0x1401, size: 1, normalized: false, byteSize: 1 },
        "uint8x2":      { type: 0x1401, size: 2, normalized: false, byteSize: 2 },
        "uint8x4":      { type: 0x1401, size: 4, normalized: false, byteSize: 4 },
        "sint8":        { type: 0x1400, size: 1, normalized: false, byteSize: 1 },
        "sint8x2":      { type: 0x1400, size: 2, normalized: false, byteSize: 2 },
        "sint8x4":      { type: 0x1400, size: 4, normalized: false, byteSize: 4 },
        "unorm8":       { type: 0x1401, size: 1, normalized: true,  byteSize: 1 },
        "unorm8x2":     { type: 0x1401, size: 2, normalized: true,  byteSize: 2 },
        "unorm8x4":     { type: 0x1401, size: 4, normalized: true,  byteSize: 4 },
        "snorm8":       { type: 0x1400, size: 1, normalized: true,  byteSize: 1 },
        "snorm8x2":     { type: 0x1400, size: 2, normalized: true,  byteSize: 2 },
        "snorm8x4":     { type: 0x1400, size: 4, normalized: true,  byteSize: 4 },
        "uint16":       { type: 0x1403, size: 1, normalized: false, byteSize: 2 },
        "uint16x2":     { type: 0x1403, size: 2, normalized: false, byteSize: 4 },
        "uint16x4":     { type: 0x1403, size: 4, normalized: false, byteSize: 8 },
        "sint16":       { type: 0x1402, size: 1, normalized: false, byteSize: 2 },
        "sint16x2":     { type: 0x1402, size: 2, normalized: false, byteSize: 4 },
        "sint16x4":     { type: 0x1402, size: 4, normalized: false, byteSize: 8 },
        "unorm16":      { type: 0x1403, size: 1, normalized: true,  byteSize: 2 },
        "unorm16x2":    { type: 0x1403, size: 2, normalized: true,  byteSize: 4 },
        "unorm16x4":    { type: 0x1403, size: 4, normalized: true,  byteSize: 8 },
        "snorm16":      { type: 0x1402, size: 1, normalized: true,  byteSize: 2 },
        "snorm16x2":    { type: 0x1402, size: 2, normalized: true,  byteSize: 4 },
        "snorm16x4":    { type: 0x1402, size: 4, normalized: true,  byteSize: 8 },
        "float16":      { type: 0x140B, size: 1, normalized: false, byteSize: 2 }, // HALF_FLOAT
        "float16x2":    { type: 0x140B, size: 2, normalized: false, byteSize: 4 },
        "float16x4":    { type: 0x140B, size: 4, normalized: false, byteSize: 8 },
        "float32":      { type: 0x1406, size: 1, normalized: false, byteSize: 4 },
        "float32x2":    { type: 0x1406, size: 2, normalized: false, byteSize: 8 },
        "float32x3":    { type: 0x1406, size: 3, normalized: false, byteSize: 12 },
        "float32x4":    { type: 0x1406, size: 4, normalized: false, byteSize: 16 },
        "uint32":       { type: 0x1405, size: 1, normalized: false, byteSize: 4 },
        "uint32x2":     { type: 0x1405, size: 2, normalized: false, byteSize: 8 },
        "uint32x3":     { type: 0x1405, size: 3, normalized: false, byteSize: 12 },
        "uint32x4":     { type: 0x1405, size: 4, normalized: false, byteSize: 16 },
        "sint32":       { type: 0x1404, size: 1, normalized: false, byteSize: 4 },
        "sint32x2":     { type: 0x1404, size: 2, normalized: false, byteSize: 8 },
        "sint32x3":     { type: 0x1404, size: 3, normalized: false, byteSize: 12 },
        "sint32x4":     { type: 0x1404, size: 4, normalized: false, byteSize: 16 },
        // 其它特殊格式可按需补充
        "unorm10-10-10-2": { type: 0x8C3B, size: 4, normalized: true, byteSize: 4 }, // UNSIGNED_INT_2_10_10_10_REV
        "unorm8x4-bgra":   { type: 0x1401, size: 4, normalized: true, byteSize: 4 }
    };

    constructor(type: PrimitiveType = PrimitiveType.TRIANGLES, vertexLayout?: VertexLayout) {
        this.type = type;
        this.vertexLayout = vertexLayout ?? null;
        this.renderState = {
            depthTest: true,
            depthWrite: true,
            cullFace: true
        };
    }

    public getGLPrimitiveType(): number {
        switch (this.type) {
            case PrimitiveType.POINTS: return WebGLConstants.POINTS;
            case PrimitiveType.LINES: return WebGLConstants.LINES;
            case PrimitiveType.LINE_STRIP: return WebGLConstants.LINE_STRIP;
            case PrimitiveType.TRIANGLES: return WebGLConstants.TRIANGLES;
            case PrimitiveType.TRIANGLE_STRIP: return WebGLConstants.TRIANGLE_STRIP;
            default: return WebGLConstants.TRIANGLES;
        }
    }

    public getGPUPrimitiveType(): GPUPrimitiveTopology {
        switch (this.type) {
            case PrimitiveType.POINTS: return 'point-list';
            case PrimitiveType.LINES: return 'line-list';
            case PrimitiveType.LINE_STRIP: return 'line-strip';
            case PrimitiveType.TRIANGLES: return 'triangle-list';
            case PrimitiveType.TRIANGLE_STRIP: return 'triangle-strip';
            default: return 'triangle-list';
        }
    }
}
