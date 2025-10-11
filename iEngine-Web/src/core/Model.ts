import { Mesh } from './Mesh';
import { Material } from '../materials/Material';
import { Matrix4 } from '../math/Matrix4';
import { Vector3 } from '../math/Vector3'

export interface Renderable {
    name: string;
    mesh: Mesh;
    material: Material;
    transform: Matrix4;
    update(/*context: WebGLContext, camera: Camera, */deltaTime: number/*, shder: any*/): void;
}

type AnimationCallback = (model: Model, deltaTime: number) => void;

export class Model implements Renderable {
    name: string;
    mesh: Mesh;
    material: Material;
    transform: Matrix4;
    // 动画回调列表
    private animations: AnimationCallback[] = [];

    constructor(name: string, mesh: Mesh, material: Material, transform?: Matrix4) {
        this.name = name;
        this.mesh = mesh;
        this.material = material;
        this.transform = Matrix4.identity();
        if (transform) this.transform = transform;
    }

    // 自定义变换方法
    translate(offset: Vector3): void {
        this.transform = this.transform.multiply(Matrix4.fromTranslation(offset));
    }

    rotate(axis: Vector3, angleRad: number): void {
        this.transform = this.transform.multiply(Matrix4.fromRotation(axis, angleRad));
    }

    scale(s: Vector3): void {
        this.transform = this.transform.multiply(Matrix4.fromScaling(s));
    }

    // 添加动画
    addAnimation(callback: AnimationCallback): void {
        this.animations.push(callback);
    }

    // 每帧调用，驱动动画
    private updateAnimation(deltaTime: number): void {
        for (const anim of this.animations) {
            anim(this, deltaTime);
        }
    }

    update(/*context: WebGLContext, camera: Camera, */deltaTime: number/*, shder: any*/): void {
        // 更新动画
        this.updateAnimation(deltaTime);
        
        // if (!shader.program) return;
        // const program: WebGLProgram = shader.program;

        // // 检查顶点、索引、实例矩阵等Buffer资源是否已经传到GPU
        // if (!this.mesh.uploaded) {
        //     this.mesh.upload(context, shader);
        // }

        // // 将相机、材质、光照等参数数据绑定到Shader的uniform（webgl）或bindgroup（webgpu）
        
        // // 设置相机参数（模型变换矩阵）
        // // shader.setMatrix4('uModelMatrix', this.transform);
        // // this.modelMatrix.identity();
        // // shader.setMatrix4('uModelViewMatrix', this.viewMatrix.multiply(this.modelMatrix));
        // // shader.setMatrix4('uProjectionMatrix', this.projectionMatrix);
        // // shader.setMatrix4('uViewMatrix', camera.getViewMatrix());
        // shader.setMatrix4('uModelViewMatrix', camera.getViewMatrix());
        // shader.setMatrix4('uProjectionMatrix', camera.getProjectionMatrix());

        // // 设置材质颜色
        // shader.setVector3('uColor', ...this.material.color);

        // // 设置光照参数

    }
}