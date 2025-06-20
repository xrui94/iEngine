export class Geometry {
    public vertices: Float32Array;
    public normals?: Float32Array;
    public texCoords?: Float32Array;
    public colors0?: Float32Array;
    public colors1?: Float32Array;
    public tangents?: Float32Array;
    public bitangents?: Float32Array;
    public indices?: Uint16Array | Uint32Array;
    public boundingBox: { min: number[], max: number[] };
    public vertexCount: number;
    public indexCount: number;

    constructor(
        params: {
            vertices: number[] | Float32Array,
            normals?: number[] | Float32Array,
            texCoords?: number[] | Float32Array,
            colors0?: number[] | Float32Array,
            colors1?: number[] | Float32Array,
            tangents?: number[] | Float32Array,
            bitangents?: number[] | Float32Array,
        },
        indices?: number[]
    ) {
        // 位置
        this.vertices = Array.isArray(params.vertices) && !ArrayBuffer.isView(params.vertices)
            ? new Float32Array(params.vertices)
            : params.vertices as Float32Array;
        this.vertexCount = this.vertices.length / 3;

        // 其它属性
        if (params.normals)
            this.normals = Array.isArray(params.normals) && !ArrayBuffer.isView(params.normals)
                ? new Float32Array(params.normals)
                : params.normals as Float32Array;
        if (params.texCoords)
            this.texCoords = Array.isArray(params.texCoords) && !ArrayBuffer.isView(params.texCoords)
                ? new Float32Array(params.texCoords)
                : params.texCoords as Float32Array;
        if (params.colors0)
            this.colors0 = Array.isArray(params.colors0) && !ArrayBuffer.isView(params.colors0)
                ? new Float32Array(params.colors0)
                : params.colors0 as Float32Array;
        if (params.colors1)
            this.colors1 = Array.isArray(params.colors1) && !ArrayBuffer.isView(params.colors1)
                ? new Float32Array(params.colors1)
                : params.colors1 as Float32Array;
        if (params.tangents)
            this.tangents = Array.isArray(params.tangents) && !ArrayBuffer.isView(params.tangents)
                ? new Float32Array(params.tangents)
                : params.tangents as Float32Array;
        if (params.bitangents)
            this.bitangents = Array.isArray(params.bitangents) && !ArrayBuffer.isView(params.bitangents)
                ? new Float32Array(params.bitangents)
                : params.bitangents as Float32Array;

        // 索引
        if (indices) {
            this.indexCount = indices.length;
            this.indices = this.indexCount > 65535 ?
                new Uint32Array(indices) :
                new Uint16Array(indices);
        } else {
            this.indexCount = 0;
        }
        
        // 计算包围盒
        this.boundingBox = this.computeBoundingBox();
    }

    private computeBoundingBox(): { min: number[], max: number[] } {
        const min = [Infinity, Infinity, Infinity];
        const max = [-Infinity, -Infinity, -Infinity];
        
        for (let i = 0; i < this.vertices.length; i += 3) {
            const x = this.vertices[i];
            const y = this.vertices[i + 1];
            const z = this.vertices[i + 2];
            
            min[0] = Math.min(min[0], x);
            min[1] = Math.min(min[1], y);
            min[2] = Math.min(min[2], z);
            
            max[0] = Math.max(max[0], x);
            max[1] = Math.max(max[1], y);
            max[2] = Math.max(max[2], z);
        }
        
        return { min, max };
    }

}