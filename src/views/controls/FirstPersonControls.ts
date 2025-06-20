import { PerspectiveCamera } from '../cameras/PerspectiveCamera';
import { BaseControls } from './BaseControls';

export class FirstPersonControls extends BaseControls {
    private camera: PerspectiveCamera;
    private domElement: HTMLElement;
    private isDragging = false;
    private lastX = 0;
    private lastY = 0;
    private yaw = 0;
    private pitch = 0;
    private moveSpeed = 0.1;
    private lookSpeed = 0.002;

    constructor(camera: PerspectiveCamera, domElement: HTMLElement) {
        super();

        //
        this.camera = camera;
        this.domElement = domElement;

        // 初始化 yaw/pitch 与当前相机方向同步
        const dir = [
            camera.target.x - camera.position.x,
            camera.target.y - camera.position.y,
            camera.target.z - camera.position.z
        ];
        const len = Math.sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
        if (len > 0) {
            this.yaw = Math.atan2(dir[0], dir[2]);
            this.pitch = Math.asin(dir[1] / len);
        }

        // 保证初始状态
        this.updateCamera();

        domElement.addEventListener('mousedown', this.onMouseDown, { passive: false });
        domElement.addEventListener('wheel', this.onMouseWheel, { passive: false });
        window.addEventListener('keydown', this.onKeyDown, { passive: false });
    }

    private onMouseDown = (e: MouseEvent) => {
        this.isDragging = true;
        this.lastX = e.clientX;
        this.lastY = e.clientY;
        window.addEventListener('mousemove', this.onMouseMove, { passive: false });
        window.addEventListener('mouseup', this.onMouseUp, { passive: false });
    };

    private onMouseMove = (e: MouseEvent) => {
        if (!this.isDragging) return;
        const dx = e.clientX - this.lastX;
        const dy = e.clientY - this.lastY;
        this.lastX = e.clientX;
        this.lastY = e.clientY;

        this.yaw -= dx * this.lookSpeed;
        this.pitch -= dy * this.lookSpeed;
        this.pitch = Math.max(-Math.PI / 2, Math.min(Math.PI / 2, this.pitch));
        this.updateCamera();
    };

    private onMouseUp = () => {
        this.isDragging = false;
        window.removeEventListener('mousemove', this.onMouseMove);
        window.removeEventListener('mouseup', this.onMouseUp);
    };

    private onMouseWheel = (e: WheelEvent) => {
        // 可用于前后移动
        this.camera.position.z += e.deltaY * 0.01;
    };

    private onKeyDown = (e: KeyboardEvent) => {
        // 简单的WASD移动
        const forward = [
            Math.sin(this.yaw),
            0,
            Math.cos(this.yaw)
        ];
        const right = [
            Math.cos(this.yaw),
            0,
            -Math.sin(this.yaw)
        ];
        if (e.key === 'w') {
            this.camera.position.x += forward[0] * this.moveSpeed;
            this.camera.position.z += forward[2] * this.moveSpeed;
        }
        if (e.key === 's') {
            this.camera.position.x -= forward[0] * this.moveSpeed;
            this.camera.position.z -= forward[2] * this.moveSpeed;
        }
        if (e.key === 'a') {
            this.camera.position.x -= right[0] * this.moveSpeed;
            this.camera.position.z -= right[2] * this.moveSpeed;
        }
        if (e.key === 'd') {
            this.camera.position.x += right[0] * this.moveSpeed;
            this.camera.position.z += right[2] * this.moveSpeed;
        }
        this.updateCamera();
    };

    private updateCamera() {
        // 计算朝向
        const dir = [
            Math.sin(this.yaw) * Math.cos(this.pitch),
            Math.sin(this.pitch),
            Math.cos(this.yaw) * Math.cos(this.pitch)
        ];
        const lookAt = [
            this.camera.position.x  + dir[0],
            this.camera.position.y  + dir[1],
            this.camera.position.z  + dir[2]
        ];
        this.camera.lookAt(lookAt[0], lookAt[1], lookAt[2]);
    }

    dispose() {
        this.domElement.removeEventListener('mousedown', this.onMouseDown);
        this.domElement.removeEventListener('wheel', this.onMouseWheel);
        window.removeEventListener('mousemove', this.onMouseMove);
        window.removeEventListener('mouseup', this.onMouseUp);
        window.removeEventListener('keydown', this.onKeyDown);
    }
}