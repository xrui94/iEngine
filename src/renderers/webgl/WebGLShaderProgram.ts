import { WebGLUniforms, UniformInfo, UniformValue } from "../../renderers/webgl/WebGLUniforms";

import type { DefineMap } from "../../shaders/ShaderLib";

import type { WebGLContext } from "../../renderers/webgl/WebGLContext";

export class WebGLShaderProgram {

    public program: WebGLProgram | null = null;
    private context: WebGLContext;
    private vertCode: string;
    private fragCode: string;
    public uniforms: WebGLUniforms;

    constructor(context: WebGLContext, vertCode: string, fragCode: string, defines: DefineMap, uniforms: UniformInfo[]) {
        this.context = context;
        this.vertCode = vertCode;
        this.fragCode = fragCode;
        this.program = this.createProgram();

        //
        if (!this.program) {
            throw new Error('Failed to create WebGL program');
        }
        this.uniforms = new WebGLUniforms(this.context, this.program);
    }

    private createProgram(): WebGLProgram | null {        
        const program = this.context.createProgram(this.vertCode, this.fragCode);
        if (!program) {
            console.error('Failed to create WebGL program');
            return null;
        };
        
        return program;
    }

    use(): void {
        if (this.program) {
            this.context.useProgram(this.program);
        }
    }

    /**
     * 批量设置uniform
     * @param uniforms 形如 { uModelMatrix: ..., uColor: ..., ... }
     */
    setUniforms(uniforms: Record<string, UniformValue>): void {
        this.uniforms.setUniforms(uniforms);
    }

}