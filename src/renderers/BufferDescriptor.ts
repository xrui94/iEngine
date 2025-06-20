export interface BufferDescriptor {
    byteLength: number;
    usage?: number;              // WebGL: gl.STATIC_DRAW, WebGPU: usage flags
    mappedAtCreation?: boolean;  // 仅 WebGPU 用
    label?: string;              // 调试用
};

export interface WriteBufferDescriptor {
    buffer: any;
    data: ArrayBufferView;
    target?: number;         // WebGL: gl.ARRAY_BUFFER, WebGPU: 可忽略
    usage?: number;          // WebGL: gl.STATIC_DRAW, WebGPU: 可忽略
    offset?: number;         // 偏移
    mappedAtCreation?: boolean; // WebGPU 用
};
