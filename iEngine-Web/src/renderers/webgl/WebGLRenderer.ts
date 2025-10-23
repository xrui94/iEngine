import { Renderer } from '../Renderer';
import { Camera } from '../../views/cameras/Camera';
import { PerspectiveCamera } from '../../views/cameras/PerspectiveCamera';
import { Scene } from '../../scenes/Scene';
import { Light } from '../../lights/Light';
import { Renderable } from '../Renderable';
import { WebGLShaderProgram } from './WebGLShaderProgram';
import { WebGLContext } from './WebGLContext';
import { ShaderLib } from '../../shaders/ShaderLib';
import { WebGLRenderPipeline, WebGLRenderPipelineState } from './WebGLRenderPipeline';

import type { GraphicsAPI } from '../Renderer';
import type { Mesh } from '../../core/Mesh';
import type { WebGLContextOptions } from './WebGLContext';

export class WebGLRenderer extends Renderer {
    private _canvas: HTMLCanvasElement = null!;
    private _context!: WebGLContext;
    private _clearColor: [number, number, number, number] = [0, 0, 0, 1];
    private _viewport: { x: number; y: number; width: number; height: number } | null = null;
    private graphicsApi!: GraphicsAPI;
    private shaders: Map<string, WebGLShaderProgram> = new Map();
    private _isInitialized: boolean = false;
    private renderPipelineCache: Map<string, WebGLRenderPipeline> = new Map();
    
    // // 相机参数
    // private projectionMatrix = new Matrix4();
    // private viewMatrix = new Matrix4();
    // private modelMatrix = new Matrix4();
    
    // // 场景参数
    // private cameraPosition = new Vector3(0, 0, 5);
    // private cameraTarget = new Vector3(0, 0, 0);
    // private cameraUp = new Vector3(0, 1, 0);

    // 不再直接管理相机参数
    // 而是通过外部传入相机对象
    private currentCamera: Camera | null = null;

    constructor(/*canvas: HTMLCanvasElement, options: WebGLContextOptions = {}*/) {
        super();

        // // 确保传入的 canvas 元素存在
        // if (!canvas) {
        //     throw new Error('Canvas element is required for WebGLRenderer');
        // }

        // const useWebGL1 = options.useWebGL1 !== undefined ? options.useWebGL1 : false;
        // this.graphicsApi = useWebGL1 ? 'webgl1' : 'webgl2';

        // this.context = new WebGLContext(canvas, {
        //     useWebGL1
        // });
        // this.initShaders();
        // this.resize();
    }

    init(canvas: HTMLCanvasElement, options: WebGLContextOptions): void {
        this._canvas = canvas;
        this._context = new WebGLContext(canvas, options);
        const useWebGL1 = options.useWebGL1;
        this.graphicsApi = useWebGL1 ? 'webgl1' : 'webgl2';

        //
        this._context.init();
        // this.initShaders();
        this.resize();

        this._isInitialized = true;
    }

    isInitialized(): boolean {
        return this._isInitialized;
    }

    // 提供gl上下文的访问器
    get glContext(): WebGLRenderingContext {
        return this._context.getDevice();
    }

    // private initShaders(): void {
    //     // 加载所有预定义的着色器
    //     // for (const [name, source] of ShaderLib.getAllShaders(this.graphicsApi)) {
    //     //     const shader = new WebGLShaderProgram(
    //     //         this.glContext, source.vertex, source.fragment
    //     //     );
    //     //     this.shaders.set(name, shader);
    //     // }

    //     if (!Renderer.isGraphicsAPI(this.graphicsApi)) {
    //         throw 'Failed to get Graphic API.';
    //     }

    //     for (const shaderName of BaseShaders) {
    //         const source = ShaderLib.getVariant(shaderName, this.graphicsApi);
    //         if (source && source.vertex && source.fragment) {
    //             const shader = new WebGLShaderProgram(
    //                 this.context, source.vertex, source.fragment
    //             );
    //             this.shaders.set(shaderName, shader);
    //         }
    //     }        
    // }

    // getOrCreateShader(
    //     shaderName: string,
    //     defines: Record<string, string | number | boolean>
    // ): WebGLShaderProgram | null {
    //     const key = makeShaderVariantKey(shaderName, defines);
    //     let shader = this.shaders.get(key);
    //     if (!shader) {
    //         const variant = ShaderLib.getVariant(shaderName, this.graphicsApi, defines);
    //         if (!variant || !variant.vertex || !variant.fragment) {
    //             console.warn(`Shader variant not found for "${shaderName}"`);
    //             return null;
    //         }
    //         shader = new WebGLShaderProgram(
    //             this.context, variant.vertex, variant.fragment
    //         );
    //         this.shaders.set(key, shader);
    //     }
    //     return shader;
    // }

    getOrCreatePipeline(mesh: Mesh, shader: WebGLShaderProgram, state?: WebGLRenderPipelineState): WebGLRenderPipeline {
        const key = WebGLRenderPipeline.makeHash(mesh, shader, state);
        let pipeline = this.renderPipelineCache.get(key);
        if (!pipeline) {
            pipeline = new WebGLRenderPipeline(this._context, mesh, shader, state);
            this.renderPipelineCache.set(key, pipeline);
        }
        return pipeline;
    }

    setClearColor(color?: [number, number, number, number]): void {
        if (color) {
            this._clearColor = color;
            const gl = this.glContext;
            gl.clearColor(color[0], color[1], color[2], color[3]);
        }
    }

    setViewport(viewport?: { x: number; y: number; width: number; height: number }): void {
        const gl = this.glContext;
        if (viewport) {
            this._viewport = viewport;
            gl.viewport(viewport.x, viewport.y, viewport.width, viewport.height);
        } else {
            this._viewport = null;
            gl.viewport(0, 0, this._context.width, this._context.height);
        }
    }

    clear(): void {
        this._context.clear();
    }

    resize(): void {
        const displayWidth = this._context.displayWidth;
        const displayHeight = this._context.displayHeight;
        this._context.resize(displayWidth, displayHeight);
        
        // 更新相机宽高比（如果是透视相机）
        if (this.currentCamera instanceof PerspectiveCamera) {
            this.currentCamera.setAspect(this._context.width / this._context.height);
        }
    }

    render(scene: Scene, options?: { clearCanvas?: boolean }): void {
        this.currentCamera = scene.activeCamera;
        if (!this.currentCamera ) {
            console.error("No active camera set for rendering");
            return;
        }

        // 使用基类统一聚合（去重）
        const final = this.aggregateRenderables(scene);

        // 检查是否有可渲染对象
        if (final.length === 0) {
            console.warn("No components to render in the scene");
            return;
        }
        
        // 使用基类统一聚合光源（去重）
        const finalLights = this.aggregateLights(scene);
        
        // 根据选项决定是否清除画布（默认清除）
        if (options?.clearCanvas !== false) {
            if (this._viewport) {
                // 如果设置了viewport，使用局部清屏
                this._context.clearViewport(this._viewport, this._clearColor);
            } else {
                // 否则清除整个画布
                this._context.clear();
            }
        }
        
        // 遍历所有组件，渲染每个组件
        for (const component of final) {
            // // 更新model中的一些状态等？
            // component.update(this._context, camera, shader);

            // const shader = this.shaders.get(component.material.shaderName);
            // if (!shader) {
            //     console.warn(`Shader "${component.material.shaderName}" not found for component "${component.name}"`);
            //     continue;
            // }

            // 1. 确保 mesh 顶点、索引、实例矩阵等Buffer资源已经传到GPU
            if (!component || !component.mesh) {
                console.warn("A invalid component instance was found!");
                continue;
            }
            if (!component.mesh.uploaded) {
                component.mesh.upload(this._context);
            }

            // 2. 根据材质特性，创建Shder
            const defines: Record<string, string | number | boolean> = {
                ...component.mesh.getShaderMacroDefines(),
                ...component.material.getShaderMacroDefines()
            };
            const shader = component.material.getShader(this._context, this.graphicsApi, {
                defines,
                overrideDefaultDefines: true
            });
            if (!shader) {
                console.error('Failed to get or create shader.');
                continue;
            }
            
            // 3. 获取/创建 RenderPipeline
            const pipelineState = component.material.getRenderPipelineState();
            const pipeline = this.getOrCreatePipeline(
                component.mesh, shader as WebGLShaderProgram, pipelineState
            );

            // 4. 切换 pipeline（切换use着色器程序、绑定VAO、设置渲染状态）
            pipeline.bind(this._context);

            // 让材质/Shader自己决定需要哪些uniform
            const uniforms = component.material.getUniforms(
                this._context, this.currentCamera, component, finalLights
            );
            const textures = component.material.getTextures();
            shader.setUniforms({
                ...uniforms,
                ...textures
            });

            // 6. 绘制(DrawCall)
            this._context.draw(component);
        }
    }

}