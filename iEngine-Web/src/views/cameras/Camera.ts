import { Matrix4 } from "../../math/Matrix4";
import { Vector3 } from "../../math/Vector3";

export abstract class Camera {
    // 相机位置和方向
    public position: Vector3 = new Vector3(0, 0, 5);
    public target: Vector3 = new Vector3(0, 0, 0);
    public up: Vector3 = new Vector3(0, 1, 0);
    
    // 投影和视图矩阵
    protected projectionMatrix: Matrix4 = new Matrix4();
    protected viewMatrix: Matrix4 = new Matrix4();
    
    // 视图矩阵脏标记
    protected viewMatrixDirty: boolean = true;
    // 投影矩阵脏标记
    protected projectionMatrixDirty: boolean = true;
    
    constructor() {
        // this.updateProjectionMatrix();
        this.updateViewMatrix();
    }
    
    // 更新视图矩阵
    updateViewMatrix(): void {
        if (!this.viewMatrixDirty) return;
        
        this.viewMatrix.lookAt(
            this.position.x, this.position.y, this.position.z,
            this.target.x, this.target.y, this.target.z,
            this.up.x, this.up.y, this.up.z
        );
        
        this.viewMatrixDirty = false;
    }
    
    // 抽象方法：子类实现投影矩阵更新
    abstract updateProjectionMatrix(): void;
    
    // 设置相机位置
    setPosition(x: number, y: number, z: number): this {
        this.position.set(x, y, z);
        this.viewMatrixDirty = true;
        return this;
    }
    
    // 设置相机目标
    setTarget(x: number, y: number, z: number): this {
        this.target.set(x, y, z);
        this.viewMatrixDirty = true;
        return this;
    }
    
    // 设置上方向
    setUp(x: number, y: number, z: number): this {
        this.up.set(x, y, z);
        this.viewMatrixDirty = true;
        return this;
    }
    
    // 获取视图矩阵
    getViewMatrix(): Matrix4 {
        this.updateViewMatrix();
        return this.viewMatrix;
    }
    
    // 获取投影矩阵
    getProjectionMatrix(): Matrix4 {
        this.updateProjectionMatrix();
        return this.projectionMatrix;
    }
    
    // 获取视图投影矩阵
    getViewProjectionMatrix(): Matrix4 {
        this.updateViewMatrix();
        this.updateProjectionMatrix();
        return this.projectionMatrix.multiply(this.viewMatrix);
    }
}
