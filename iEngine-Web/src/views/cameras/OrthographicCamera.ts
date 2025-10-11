import { Camera } from "./Camera";

export class OrthographicCamera extends Camera {
    // 正交参数
    public left: number = -1;
    public right: number = 1;
    public top: number = 1;
    public bottom: number = -1;
    public near: number = 0.1;
    public far: number = 100;
    
    constructor(
        left: number = -1, 
        right: number = 1, 
        top: number = 1, 
        bottom: number = -1, 
        near: number = 0.1, 
        far: number = 100
    ) {
        super();
        this.left = left;
        this.right = right;
        this.top = top;
        this.bottom = bottom;
        this.near = near;
        this.far = far;
        this.updateProjectionMatrix();
    }
    
    // 实现投影矩阵更新
    updateProjectionMatrix(): void {
        if (!this.projectionMatrixDirty) return;
        
        this.projectionMatrix.orthographic(
            this.left,
            this.right,
            this.top,
            this.bottom,
            this.near,
            this.far
        );
        
        this.projectionMatrixDirty = false;
    }
    
    // 设置正交参数
    setOrthographicParams(
        left: number, 
        right: number, 
        top: number, 
        bottom: number
    ): this {
        this.left = left;
        this.right = right;
        this.top = top;
        this.bottom = bottom;
        this.projectionMatrixDirty = true;
        return this;
    }
}