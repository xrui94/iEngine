import { Context } from '../Context';
import { Matrix4 } from '../../math/Matrix4';
import { Vector3 } from '../../math/Vector3';
import { iMath } from '../../math/Math';
import { Primitive, VertexLayout } from '../../core/Primitive';
import { Texture } from '../../textures/Texture';
import { Renderable } from '../Renderable';
import { WriteBufferDescriptor } from '../BufferDescriptor';
import { getWebGLWrapMode, getWebGLMinFilter, getWebGLMagFilter } from '../../textures/TextureUtils';

import type { Mesh } from '../../core/Mesh';
import type { TextureDescriptor } from '../TextureDescriptor';


export type WebGLContextOptions = {
    useWebGL1?: boolean;
    attrs?: {
        antialias?: boolean;
        alpha?: boolean;
        depth?: boolean;
        stencil?: boolean;
        premultipliedAlpha?: boolean;
        preserveDrawingBuffer?: boolean;
        powerPreference?: 'default' | 'high-performance' | 'low-power';
    }
};

// import type { OES_vertex_array_object } from './OES_vertex_array_object';

// export type WebGLVertexArrayObject = WebGLVertexArrayObjectOES | WebGLVertexArrayObject;

export class WebGLContext extends Context {
    private gl!: WebGLRenderingContext;
    private vaoExt: OES_vertex_array_object | null = null;
    private _textureMap: WeakMap<Texture, WebGLTexture> = new WeakMap();
    public useWebGL1: boolean;

    maxTextureUnits!: number;

    constructor(canvas: HTMLCanvasElement, options: WebGLContextOptions = {}) {
        super(canvas);

        //
        this.useWebGL1 = options.useWebGL1 ?? false;
        if (this.useWebGL1) {
            console.warn('Using WebGL 1.0 context, consider using WebGL 2.0 for better performance and features.');
        }

        //
        // this.init(options);
    }

    get width(): number {
        return this._canvas.width;
    }

    set width(value: number) {
        this._canvas.width = value;
        this.gl.viewport(0, 0, this._canvas.width, this._canvas.height);
    }

    get height(): number {
        return this._canvas.height;
    }

    set height(value: number) {
        this._canvas.height = value;
        this.gl.viewport(0, 0, this._canvas.width, this._canvas.height);
    }

    init() {
        if (this.useWebGL1) {
            // 如果需要使用 WebGL 1.0，则强制获取 WebGL 1.0 上下文
            this.gl = this._canvas.getContext('webgl') as WebGLRenderingContext;
            if (!this.gl) {
                throw new Error('WebGL 1.0 not supported');
            }
        } else {
            // 尝试获取 WebGL 2.0 上下文
            this.gl = this._canvas.getContext('webgl2') as WebGL2RenderingContext
                || this._canvas.getContext('webgl') as WebGLRenderingContext;

            if (this.gl instanceof WebGLRenderingContext) {
                console.warn('WebGL 2.0 is not supported, replaced with WebGL 1.0!')
            }
        }

        // 如果获取不到 WebGL 上下文，则抛出错误
        if (!this.gl) throw new Error('WebGL not supported');

        // 针对WebGL1上下文获取 VAO 扩展
        if (!(this.gl instanceof WebGL2RenderingContext)) {
            this.vaoExt = this.gl.getExtension('OES_vertex_array_object');
        }

        //
        this.maxTextureUnits = this.gl.getParameter(this.gl.MAX_COMBINED_TEXTURE_IMAGE_UNITS);

        // 设置初始状态
        this.gl.clearColor(0.1, 0.1, 0.1, 1.0);
        this.gl.enable(this.gl.DEPTH_TEST);
        this.gl.depthFunc(this.gl.LEQUAL);
    }

    getDevice(): WebGLRenderingContext {
        return this.gl;
    }

    getVAOExt() {
        return this.vaoExt;
    }

    clear(): void {
        const gl = this.gl;
        gl.clearColor(0, 0, 0, 1);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    }

    clearViewport(viewport: { x: number; y: number; width: number; height: number }, color?: [number, number, number, number]): void {
        const gl = this.gl;
        if (color) {
            gl.clearColor(color[0], color[1], color[2], color[3]);
        }
        gl.enable(gl.SCISSOR_TEST);
        gl.scissor(viewport.x, viewport.y, viewport.width, viewport.height);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        gl.disable(gl.SCISSOR_TEST);
    }

    resize(width: number, height: number): void {
        // DPR-aware：根据显示尺寸与设备像素比设置绘制缓冲尺寸
        const dpr = Math.max(1, Math.floor((window as any).devicePixelRatio || 1));
        const drawWidth = Math.max(1, Math.floor(width * dpr));
        const drawHeight = Math.max(1, Math.floor(height * dpr));
        if (this._canvas.width !== drawWidth || this._canvas.height !== drawHeight) {
            this._canvas.width = drawWidth;
            this._canvas.height = drawHeight;
        }
        // 同步 CSS 尺寸，确保视觉大小与 clientWidth/clientHeight 对齐
        this._canvas.style.width = `${width}px`;
        this._canvas.style.height = `${height}px`;
        this.gl.viewport(0, 0, this._canvas.width, this._canvas.height);
    }

    // 可添加 buffer 绑定等方法
    createVAO(): WebGLVertexArrayObject | null {
        if (this.gl instanceof WebGL2RenderingContext) {
            return this.gl.createVertexArray();
        } else if (this.vaoExt) {
            return this.vaoExt.createVertexArrayOES();
        }
        return null;
    }

    bindVAO(vao: WebGLVertexArrayObject | null): void {
        if (this.gl instanceof WebGL2RenderingContext) {
            this.gl.bindVertexArray(vao);
        } else if (this.vaoExt) {
            this.vaoExt.bindVertexArrayOES(vao);
        }
    }

    unbindVAO(): void {
        if (this.gl instanceof WebGL2RenderingContext) {
            this.gl.bindVertexArray(null);
        } else if (this.vaoExt) {
            this.vaoExt.bindVertexArrayOES(null);
        }
    }

    // createVertexBuffer(data: ArrayBufferView): WebGLBuffer | null {
    //     const buffer = this.gl.createBuffer();
    //     if (!buffer) return null;
    //     this.gl.bindBuffer(this.gl.ARRAY_BUFFER, buffer);
    //     this.gl.bufferData(this.gl.ARRAY_BUFFER, data, this.gl.STATIC_DRAW);
    //     return buffer;
    // }

    // bindVertexBuffer(buffer: WebGLBuffer | null): void {
    //     this.gl.bindBuffer(this.gl.ARRAY_BUFFER, buffer);
    // }

    // createIndexBuffer(data: ArrayBufferView): WebGLBuffer | null {
    //     const buffer = this.gl.createBuffer();
    //     if (!buffer) return null;
    //     this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, buffer);
    //     this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, data, this.gl.STATIC_DRAW);
    //     return buffer;
    // }

    // bindIndexBuffer(buffer: WebGLBuffer | null): void {
    //     this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, buffer);
    // }

    // createInstanceBuffer(data: ArrayBufferView): WebGLBuffer | null {
    //     // 实际上和 createVertexBuffer 一样
    //     return this.createVertexBuffer(data);
    // }

    // bindInstanceBuffer(buffer: WebGLBuffer | null): void {
    //     // 实际上和 bindVertexBuffer 一样
    //     this.bindVertexBuffer(buffer);
    // }

    createVertexBuffer(): WebGLBuffer {
        const buffer = this.gl.createBuffer();
        return buffer;
    }

    createIndexBuffer(): WebGLBuffer {
        const buffer = this.gl.createBuffer();
        return buffer;
    }

    createInstanceBuffer(): WebGLBuffer {
        const buffer = this.gl.createBuffer();
        return buffer;
    }

    createUniformBuffer(): WebGLBuffer {
        // WebGL 2 only, fallback or throw for WebGL 1
        if (!this.gl.createBuffer) throw new Error("WebGL1 does not support UBOs directly.");
        const buffer = this.gl.createBuffer();
        return buffer;
    }

    writeBuffer(desc: WriteBufferDescriptor): void {
        const { buffer, data, target = this.gl.ARRAY_BUFFER, usage = this.gl.STATIC_DRAW, offset = 0 } = desc;
        this.gl.bindBuffer(target, buffer);
        if (offset === 0) {
            this.gl.bufferData(target, data, usage);
        } else {
            this.gl.bufferSubData(target, offset, data);
        }
    }

    /**
     * 删除 WebGLBuffer
     * @param buffer WebGLBuffer
     */
    deleteBuffer(buffer: WebGLBuffer | null | undefined): void {
        if (buffer) {
            this.gl.deleteBuffer(buffer);
        }
    }

    /**
     * 获取 attribute 在指定 program 中的位置
     */
    getAttrLocation(program: WebGLProgram, name: string): number {
        return this.gl.getAttribLocation(program, name);
    }

    /**
     * 启用指定 attribute location 的顶点属性数组
     */
    enableVertexAttrArray(location: number): void {
        this.gl.enableVertexAttribArray(location);
    }

    setVertexAttrPointer(program: WebGLProgram, vertextBufferLayouts: VertexLayout): void {
        const attrs = vertextBufferLayouts.attributes;
        for (const attr of attrs) {
            const formatInfo = Primitive.VertexFormatInfo[attr.format];
            // const loc = attr.location ?? this.gl.getAttribLocation(program, attr.name);
            if (attr.shaderLocation >= 0) {
                this.gl.enableVertexAttribArray(attr.shaderLocation);
                this.gl.vertexAttribPointer(
                    attr.shaderLocation,
                    formatInfo.size,
                    formatInfo.type,
                    formatInfo.normalized,
                    vertextBufferLayouts.arrayStride,
                    attr.offset
                );
            }
        }
    }

    createTexture(): WebGLTexture {
        // Context 只负责创建，缓存建议交给 AssetManager 管理。
        // let glTexture = this._textureMap.get(texture);
        // // 如果 needsUpdate，强制重新上传
        // if (glTexture && !texture.needsUpdate()) return glTexture;

        const gl = this.gl;
        const gpuTexture = gl.createTexture();
        // gl.bindTexture(gl.TEXTURE_2D, glTexture);

        // // 设置参数（这里的参数需要把自定义的枚举值映射为WebGL中的常量值）
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, this.getWrapMode(gl, texture.wrapS));
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, this.getWrapMode(gl, texture.wrapT));
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, this.getMinFilter(gl, texture.minFilter));
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, this.getMagFilter(gl, texture.magFilter));

        // // 判断图片是否准备好，否则用默认占位贴图
        // const image = texture.image ?? Texture.DefaultImageData;

        // // 上传像素数据
        // if (image instanceof HTMLImageElement ||
        //     image instanceof HTMLCanvasElement ||
        //     image instanceof HTMLVideoElement) {
        //     gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
        // } else if (image instanceof ImageData) {
        //     gl.texImage2D(
        //         gl.TEXTURE_2D,
        //         0,
        //         gl.RGBA,
        //         image.width,
        //         image.height,
        //         0,
        //         gl.RGBA,
        //         gl.UNSIGNED_BYTE,
        //         image.data
        //     );
        // } else {
        //     // 兜底：1x1白色像素
        //     gl.texImage2D(
        //         gl.TEXTURE_2D,
        //         0,
        //         gl.RGBA,
        //         1,
        //         1,
        //         0,
        //         gl.RGBA,
        //         gl.UNSIGNED_BYTE,
        //         new Uint8Array([255, 255, 255, 255])
        //     );
        // }

        // // mipmap
        // if ((image instanceof HTMLImageElement || image instanceof HTMLCanvasElement || image instanceof ImageData) &&
        //     iMath.isPowerOf2((image as any).width) &&
        //     iMath.isPowerOf2((image as any).height)) {
        //     gl.generateMipmap(gl.TEXTURE_2D);
        // } else {
        //     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        //     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        //     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        // }

        // this._textureMap.set(texture, glTexture);
        // texture.markUpdated(); // 标记已上传
        return gpuTexture;
    }

    bindTexture(glTex: WebGLTexture, unit: number): void {
        const gl = this.gl;
        // const glTex = this.createTexture(texture);
        // if (!glTex) return;
        gl.activeTexture(gl.TEXTURE0 + unit);
        gl.bindTexture(gl.TEXTURE_2D, glTex);
    }
    
    writeTexture(texture: Texture): void {
        const gl = this.gl;

        //
        if (!texture.gpuTexture) throw new Error('No GPU texture bound!');
        gl.bindTexture(gl.TEXTURE_2D, texture.gpuTexture as WebGLTexture);

        // 设置参数（这里的参数需要把自定义的枚举值映射为WebGL中的常量值）
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, getWebGLWrapMode(gl, texture.wrapS));
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, getWebGLWrapMode(gl, texture.wrapT));
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, getWebGLMinFilter(gl, texture.minFilter));
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, getWebGLMagFilter(gl, texture.magFilter));

        // 判断图片是否准备好，否则用默认占位贴图
        if (!texture.image) {
            console.error('Null texture image is invalid!');
            return;
        }
        const image = texture.image;

        // 上传像素数据
        if (image instanceof HTMLImageElement ||
            image instanceof HTMLCanvasElement ||
            image instanceof HTMLVideoElement) {
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
        } else if (image instanceof ImageData) {
            gl.texImage2D(
                gl.TEXTURE_2D,
                0,
                gl.RGBA,
                image.width,
                image.height,
                0,
                gl.RGBA,
                gl.UNSIGNED_BYTE,
                image.data
            );
        } else {
            // 兜底：1x1白色像素
            gl.texImage2D(
                gl.TEXTURE_2D,
                0,
                gl.RGBA,
                1,
                1,
                0,
                gl.RGBA,
                gl.UNSIGNED_BYTE,
                new Uint8Array([255, 255, 255, 255])
            );
        }

        // mipmap
        if ((image instanceof HTMLImageElement || image instanceof HTMLCanvasElement || image instanceof ImageData) &&
            iMath.isPowerOf2((image as any).width) &&
            iMath.isPowerOf2((image as any).height)) {
            gl.generateMipmap(gl.TEXTURE_2D);
        } else {
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        }

        // texture.markUpdated(); // 标记已上传
    }

        /**
     * 删除 WebGLTexture
     * @param texture WebGLTexture
     */
    deleteTexture(texture: WebGLTexture | null | undefined): void {
        if (texture) {
            this.gl.deleteTexture(texture);
        }
    }

    getUniformCount(program: WebGLProgram): number {
        return this.gl.getProgramParameter(program, this.gl.ACTIVE_UNIFORMS);
    }

    getUniformLocation(program: WebGLProgram, name: string): WebGLUniformLocation | null {
        return this.gl.getUniformLocation(program, name);
    }

    getActiveUniform(program: WebGLProgram, index: GLuint): WebGLActiveInfo | null {
        return this.gl.getActiveUniform(program, index);
    }

    setUniform(type: GLenum, loc: WebGLUniformLocation, value: any): void {
        const gl = this.gl;
        switch (type) {
            case gl.FLOAT: gl.uniform1f(loc, value); break;
            case gl.FLOAT_VEC2:
                Array.isArray(value) ? gl.uniform2fv(loc, value) : gl.uniform2f(loc, value.x, value.y); break;
            case gl.FLOAT_VEC3:
                Array.isArray(value) ? gl.uniform3fv(loc, value) : gl.uniform3f(loc, value.x, value.y, value.z); break;
            case gl.FLOAT_VEC4:
                Array.isArray(value) ? gl.uniform4fv(loc, value) : gl.uniform4f(loc, value.x, value.y, value.z, value.w); break;
            case gl.INT:
            case gl.BOOL: gl.uniform1i(loc, value); break;
            case gl.INT_VEC2:
            case gl.BOOL_VEC2:
                Array.isArray(value) ? gl.uniform2iv(loc, value) : gl.uniform2i(loc, value.x, value.y); break;
            case gl.INT_VEC3:
            case gl.BOOL_VEC3:
                Array.isArray(value) ? gl.uniform3iv(loc, value) : gl.uniform3i(loc, value.x, value.y, value.z); break;
            case gl.INT_VEC4:
            case gl.BOOL_VEC4:
                Array.isArray(value) ? gl.uniform4iv(loc, value) : gl.uniform4i(loc, value.x, value.y, value.z, value.w); break;
            case gl.FLOAT_MAT2:
                value.elements ? gl.uniformMatrix2fv(loc, false, value.elements) : gl.uniformMatrix2fv(loc, false, value); break;
            case gl.FLOAT_MAT3:
                value.elements ? gl.uniformMatrix3fv(loc, false, value.elements) : gl.uniformMatrix3fv(loc, false, value); break;
            case gl.FLOAT_MAT4:
                value.elements ? gl.uniformMatrix4fv(loc, false, value.elements) : gl.uniformMatrix4fv(loc, false, value); break;
            case gl.SAMPLER_2D:
            case gl.SAMPLER_CUBE:
                // v: { texture: WebGLTexture, unit: number }
                gl.activeTexture(gl.TEXTURE0 + value.unit);
                gl.bindTexture(type === gl.SAMPLER_2D ? gl.TEXTURE_2D : gl.TEXTURE_CUBE_MAP, value.gpuTexture);
                gl.uniform1i(loc, value.unit);
                break;
            default:
                // 数组类型
                if (Array.isArray(value)) {
                    if (type === gl.FLOAT) gl.uniform1fv(loc, value);
                    else if (type === gl.FLOAT_VEC2) gl.uniform2fv(loc, value.flat());
                    else if (type === gl.FLOAT_VEC3) gl.uniform3fv(loc, value.flat());
                    else if (type === gl.FLOAT_VEC4) gl.uniform4fv(loc, value.flat());
                    else if (type === gl.INT) gl.uniform1iv(loc, value);
                    else if (type === gl.INT_VEC2) gl.uniform2iv(loc, value.flat());
                    else if (type === gl.INT_VEC3) gl.uniform3iv(loc, value.flat());
                    else if (type === gl.INT_VEC4) gl.uniform4iv(loc, value.flat());
                    else if (type === gl.FLOAT_MAT2) gl.uniformMatrix2fv(loc, false, value.flat());
                    else if (type === gl.FLOAT_MAT3) gl.uniformMatrix3fv(loc, false, value.flat());
                    else if (type === gl.FLOAT_MAT4) gl.uniformMatrix4fv(loc, false, value.flat());
                } else {
                    console.warn(`Unsupported uniform type: ${type}`);
                }
        }
    }

    private compile(type: GLenum, src: string): WebGLShader {
        const shader = this.gl.createShader(type)!;
        this.gl.shaderSource(shader, src);
        this.gl.compileShader(shader);
        if (!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
            throw new Error('Shader compile error: ' + this.gl.getShaderInfoLog(shader));
        }
        return shader;
    }

    createProgram(vertCode: string, fragCode: string): WebGLProgram | null {
        const vertShader = this.compile(this.gl.VERTEX_SHADER, vertCode);
        const fragShader = this.compile(this.gl.FRAGMENT_SHADER, fragCode);
        
        if (!vertShader || !fragShader) return null;
        
        const program = this.gl.createProgram();
        if (!program) return null;
        
        this.gl.attachShader(program, vertShader);
        this.gl.attachShader(program, fragShader);
        this.gl.linkProgram(program);
        
        if (!this.gl.getProgramParameter(program, this.gl.LINK_STATUS)) {
            console.error('Program link error:', this.gl.getProgramInfoLog(program));
            this.gl.deleteProgram(program);
            return null;
        }
        
        // 删除着色器，它们已经链接到程序中
        this.gl.deleteShader(vertShader);
        this.gl.deleteShader(fragShader);
        
        return program;
    }

    useProgram(program: WebGLProgram): void {
        this.gl.useProgram(program);
    }

    draw(component: Renderable | { mesh: Mesh }) {
        // 绑定VAO
        // const vao = component.mesh.getVertexArrayObject();
        // if (!vao) {
        //     console.warn(`Model ${component.name} has no VAO bound, skipping draw call.`);
        //     return;
        // }
        // this.bindVAO(vao);

        // 绘制
        if (component.mesh.geometry.indices) {
            this.gl.drawElements(
                component.mesh.primitive.getGLPrimitiveType(),
                component.mesh.geometry.indices.length,
                component.mesh.geometry.indices instanceof Uint16Array ? 
                    this.gl.UNSIGNED_SHORT : this.gl.UNSIGNED_INT,
                0
            );
        } else {
            this.gl.drawArrays(
                component.mesh.primitive.getGLPrimitiveType(),
                0,
                component.mesh.geometry.vertexCount
            );
        }

        // // 绘制完成后解绑VAO
        // this.bindVAO(null);
    }
    
}