export interface IUniform {
    set(name: string, value: any): void;
    setUniforms(uniforms: Record<string, any>): void;
    bind(resourceIndex?: number): void; // WebGPU 绑定 BindGroup，WebGL 可为空实现
}
