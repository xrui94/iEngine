import { Camera } from "./Camera";

export class PerspectiveCamera extends Camera {
    // 透视参数
    public fov: number = 45; // 视野角度（度）
    public aspect: number = 1; // 宽高比
    public near: number = 0.1; // 近平面
    public far: number = 100; // 远平面
    
    constructor(fov: number = 45, aspect: number = 1, near: number = 0.1, far: number = 100) {
        super();
        this.fov = fov;
        this.aspect = aspect;
        this.near = near;
        this.far = far;
        this.updateProjectionMatrix();
    }
    
    lookAt(x: number, y: number, z: number): void {
        // this.target = new Vector3(x, y, z);
        // this.updateViewMatrix();
        // 上边的方法，会导致“第一人称控制器”无法发挥作用
        this.setTarget(x, y, z);
    }


     // 设置视野角度
    setFov(fov: number): this {
        this.fov = fov;
        this.projectionMatrixDirty = true;
        return this;
    }
    
    // 设置宽高比
    setAspect(aspect: number): this {
        this.aspect = aspect;
        this.projectionMatrixDirty = true;
        return this;
    }
    
    // 设置近平面
    setNear(near: number): this {
        this.near = near;
        this.projectionMatrixDirty = true;
        return this;
    }
    
    // 设置远平面
    setFar(far: number): this {
        this.far = far;
        this.projectionMatrixDirty = true;
        return this;
    }

    // 实现投影矩阵更新
    updateProjectionMatrix(): void {
        if (!this.projectionMatrixDirty) return;
        
        this.projectionMatrix.perspective(
            this.fov * Math.PI / 180, // 转换为弧度
            this.aspect,
            this.near,
            this.far
        );
        
        this.projectionMatrixDirty = false;
    }
    
}
