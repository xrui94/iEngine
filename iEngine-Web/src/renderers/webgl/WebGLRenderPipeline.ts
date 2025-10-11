import { WebGLShaderProgram } from './WebGLShaderProgram';
import type { Mesh } from '../../core/Mesh';
import type { WebGLContext } from './WebGLContext';

// WebGPU中，也有类似的概念：
// export interface WebGLDepthStencilState {
//     depthTest?: boolean;
//     depthWrite?: boolean;
//     depthFunc?: number; // gl.LESS, gl.LEQUAL, etc.
//     stencilTest?: boolean;
//     stencilFunc?: { func: number, ref: number, mask: number };
//     stencilOp?: { fail: number, zfail: number, zpass: number };
// }

// export interface WebGLBlendState {
//     enabled?: boolean;
//     srcRGB?: number;
//     dstRGB?: number;
//     srcAlpha?: number;
//     dstAlpha?: number;
//     equationRGB?: number;
//     equationAlpha?: number;
// }

export interface WebGLRenderPipelineState {
    // Depth/Stencil
    depthTest?: boolean;
    depthWrite?: boolean;
    depthFunc?: number;
    stencilTest?: boolean;
    stencilFunc?: { func: number, ref: number, mask: number };
    stencilOp?: { fail: number, zfail: number, zpass: number };
    // Blend
    blend?: boolean;
    blendFunc?: { srcRGB: number, dstRGB: number, srcAlpha: number, dstAlpha: number };
    blendEquation?: { rgb: number, alpha: number };
    blendColor?: [number, number, number, number];
    // Cull
    cullFace?: boolean;
    cullMode?: number; // gl.BACK, gl.FRONT, etc.
    frontFace?: number; // gl.CCW, gl.CW
    // Color Mask
    colorMask?: [boolean, boolean, boolean, boolean];
    // Viewport/Scissor
    viewport?: [number, number, number, number];
    scissorTest?: boolean;
    scissor?: [number, number, number, number];
    // Polygon Offset
    polygonOffsetFill?: boolean;
    polygonOffset?: { factor: number, units: number };
    // Alpha To Coverage
    sampleAlphaToCoverage?: boolean;
}

export class WebGLRenderPipeline {
    public readonly shader: WebGLShaderProgram;
    public readonly vao: WebGLVertexArrayObject | null;
    public readonly hash: string;
    public readonly state: WebGLRenderPipelineState | {};

    constructor(
        context: WebGLContext,
        mesh: Mesh,
        shader: WebGLShaderProgram,
        state?: WebGLRenderPipelineState
    ) {
        if (!(shader.program instanceof WebGLProgram)) {
            throw 'Shader program is not a valid WebGLShader';
        }

        this.shader = shader;
        this.state = state ?? {};
        this.hash = WebGLRenderPipeline.makeHash(mesh, shader, this.state);

        const gl = context.getDevice();

        // 创建VAO
        this.vao = context.createVAO();
        if (this.vao) {
            // 绑定VAO
            context.bindVAO(this.vao);

            // 绑定VBO
            gl.bindBuffer(gl.ARRAY_BUFFER, mesh.vbo);

            // 设置顶点 attribute 指针
            context.setVertexAttrPointer(shader.program, mesh.getVertexLayout());

            // 绑定IBO
            if (mesh.ibo) {
                gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.ibo);
            }

            // 解绑VAO
            context.unbindVAO();
        }
    }

    static makeHash(mesh: Mesh, shader: WebGLShaderProgram, state?: WebGLRenderPipelineState): string {
        // 用 mesh 的顶点布局和 shader program id 生成唯一key
        if (!(shader.program instanceof WebGLProgram)) {
            throw 'Shader program is not a valid WebGLShader';
        }
        return mesh.primitive.type + '_' + JSON.stringify(mesh.getVertexLayout()) + '_' + shader.program + '_' + JSON.stringify(state);
    }

    bind(context: WebGLContext) {
        const gl = context.getDevice();

        this.shader.use();

        //
        context.bindVAO(this.vao);

        // 设置深度模板状态
        const state = this.state as WebGLRenderPipelineState; // 假设构造时保存了 options 到 this.state

        // 深度测试
        if (state.depthTest !== undefined) {
            state.depthTest ? gl.enable(gl.DEPTH_TEST) : gl.disable(gl.DEPTH_TEST);
        }
        if (state.depthWrite !== undefined) {
            gl.depthMask(!!state.depthWrite);
        }
        if (state.depthFunc !== undefined) {
            gl.depthFunc(state.depthFunc);
        }

        // 模板测试
        if (state.stencilTest !== undefined) {
            state.stencilTest ? gl.enable(gl.STENCIL_TEST) : gl.disable(gl.STENCIL_TEST);
        }
        if (state.stencilFunc) {
            const f = state.stencilFunc;
            gl.stencilFunc(f.func, f.ref, f.mask);
        }
        if (state.stencilOp) {
            const o = state.stencilOp;
            gl.stencilOp(o.fail, o.zfail, o.zpass);
        }

        // 混合
        if (state.blend !== undefined) {
            state.blend ? gl.enable(gl.BLEND) : gl.disable(gl.BLEND);
        }
        if (state.blendFunc) {
            gl.blendFuncSeparate(
                state.blendFunc.srcRGB, state.blendFunc.dstRGB,
                state.blendFunc.srcAlpha, state.blendFunc.dstAlpha
            );
        }
        if (state.blendEquation) {
            gl.blendEquationSeparate(
                state.blendEquation.rgb, state.blendEquation.alpha
            );
        }
        if (state.blendColor) {
            gl.blendColor(...state.blendColor);
        }

        // 剔除
        if (state.cullFace !== undefined) {
            state.cullFace ? gl.enable(gl.CULL_FACE) : gl.disable(gl.CULL_FACE);
        }
        if (state.cullMode !== undefined) {
            gl.cullFace(state.cullMode);
        }
        if (state.frontFace !== undefined) {
            gl.frontFace(state.frontFace);
        }

        // 颜色掩码
        if (state.colorMask) {
            gl.colorMask(...state.colorMask);
        }

        // 视口
        if (state.viewport) {
            gl.viewport(...state.viewport);
        }

        // Scissor
        if (state.scissorTest !== undefined) {
            state.scissorTest ? gl.enable(gl.SCISSOR_TEST) : gl.disable(gl.SCISSOR_TEST);
        }
        if (state.scissor) {
            gl.scissor(...state.scissor);
        }

        // Polygon Offset
        if (state.polygonOffsetFill !== undefined) {
            state.polygonOffsetFill ? gl.enable(gl.POLYGON_OFFSET_FILL) : gl.disable(gl.POLYGON_OFFSET_FILL);
        }
        if (state.polygonOffset) {
            gl.polygonOffset(state.polygonOffset.factor, state.polygonOffset.units);
        }

        // Alpha To Coverage (WebGL2)
        if (state.sampleAlphaToCoverage && gl instanceof WebGL2RenderingContext) {
            gl.enable(gl.SAMPLE_ALPHA_TO_COVERAGE);
        }
    }

    unbind(context: WebGLContext) {
        context.unbindVAO();
    }
}