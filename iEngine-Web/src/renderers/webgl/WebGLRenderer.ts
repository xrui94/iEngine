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
    private context!: WebGLContext;
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

    init(context: WebGLContext): void {
        this.context = context;
        const useWebGL1 = context.useWebGL1;
        this.graphicsApi = useWebGL1 ? 'webgl1' : 'webgl2';

        //
        this.context.init();
        // this.initShaders();
        this.resize();

        this._isInitialized = true;
    }

    isInitialized(): boolean {
        return this._isInitialized;
    }

    // 提供gl上下文的访问器
    get glContext(): WebGLRenderingContext {
        return this.context.getDevice();
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
            pipeline = new WebGLRenderPipeline(this.context, mesh, shader, state);
            this.renderPipelineCache.set(key, pipeline);
        }
        return pipeline;
    }

    clear(): void {
        this.context.clear();
    }

    resize(): void {
        const displayWidth = this.context.displayWidth;
        const displayHeight = this.context.displayHeight;
        this.context.resize(displayWidth, displayHeight);
        
        // 更新相机宽高比（如果是透视相机）
        if (this.currentCamera instanceof PerspectiveCamera) {
            this.currentCamera.setAspect(this.context.width / this.context.height);
        }
    }

    render(scene: Scene): void {
        this.currentCamera = scene.activeCamera;
        if (!this.currentCamera ) {
            console.error("No active camera set for rendering");
            return;
        }

        // 从场景中收集所有可渲染对象
        const renderables: Renderable[] = [];
        scene.collectRenderables(renderables);

        // 检查场景是否有组件
        // 如果没有组件，直接返回
        if (renderables.length === 0) {
            console.warn("No components to render in the scene");
            return;
        }
        
        // 获取场景中的所有光照
        const lights: Light[] = [];
        scene.collectLights(lights);
        
        // 清除画布
        this.context.clear();
        
        // 遍历所有组件，渲染每个组件
        for (const component of renderables) {
            // // 更新model中的一些状态等？
            // component.update(this.context, camera, shader);

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
                component.mesh.upload(this.context);
            }

            // 2. 根据材质特性，创建Shder
            const defines: Record<string, string | number | boolean> = {
                ...component.mesh.getShaderMacroDefines(),
                ...component.material.getShaderMacroDefines()
            };
            // const shader = this.getOrCreateShader(component.material.shaderName, defines);
            const shader = component.material.getShader(this.context, this.graphicsApi, {
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
            pipeline.bind(this.context);

            // 5. 设置 uniform，将相机、材质、光照等参数数据绑定到Shader的uniform，以最终上传到GPU
            
            // 设置相机参数（模型变换矩阵）
            // shader.setMatrix4('uModelMatrix', this.transform);
            // this.modelMatrix.identity();
            // shader.setMatrix4('uModelViewMatrix', this.viewMatrix.multiply(this.modelMatrix));
            // shader.setMatrix4('uProjectionMatrix', this.projectionMatrix);
            // shader.setMatrix4('uViewMatrix', this.currentCamera.getViewMatrix());
            
            // 计算模型-视图矩阵（将模型的变换矩阵和相机的视图矩阵相乘）
            // const modelViewMatrix = this.currentCamera.getViewMatrix().multiply(component.transform);
            // shader.setMatrix4('uModelViewMatrix', modelViewMatrix);
            // shader.setMatrix4('uProjectionMatrix', this.currentCamera.getProjectionMatrix());

            // // 设置材质颜色
            // shader.setVector3('uColor', component.material.color[0], component.material.color[1], component.material.color[2]);

            // 设置光照参数

            // 让材质/Shader自己决定需要哪些uniform
            const uniforms = component.material.getUniforms(
                this.context, this.currentCamera, component, lights
            );
            const textures = component.material.getTextures();
            // WebGL中texture也是一种特殊的uniform，因此uniform和texture可以一起设置
            shader.setUniforms({
                ...uniforms,
                ...textures
            });

            // 6. 绘制(DrawCall)
            this.context.draw(component);

            // 7. 解绑渲染管线，主要是VAO（可选，因为现代的 WebGL 的 VAO 绑定是全局的，
            // 若下一帧/下一个 draw 会切换 pipeline，则新的 pipeline.bind 会自动覆盖
            // 之前的绑定，因此，理论上不 unbind 也不会出错）
            pipeline.unbind(this.context);
        }
    }

}