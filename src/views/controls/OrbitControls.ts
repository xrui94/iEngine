import { PerspectiveCamera } from '../cameras/PerspectiveCamera';
import { Vector3 } from '../../math/Vector3';
import { BaseControls } from './BaseControls';

export class OrbitControls extends BaseControls {
    private camera: PerspectiveCamera;
    private domElement: HTMLElement;
    private target: Vector3 = new Vector3(0, 0, 0);
    private isDragging = false;
    private lastX = 0;
    private lastY = 0;
    private theta = 0;
    private phi = 0;
    private radius = 5;

    constructor(camera: PerspectiveCamera, domElement: HTMLElement) {
        super();

        //
        this.camera = camera;
        this.domElement = domElement;

        // 这种方式会导致，初始的视角不对，看不到，或看不全模型
        // this.radius = Math.hypot(
        //     camera.position.x - this.target.x ,
        //     camera.position.y - this.target.y ,
        //     camera.position.z - this.target.z 
        // );

        const dx = camera.position.x - this.target.x;
        const dy = camera.position.y - this.target.y;
        const dz = camera.position.z - this.target.z;
        this.radius = Math.sqrt(dx * dx + dy * dy + dz * dz);
        this.theta = Math.atan2(dx, dz);
        this.phi = Math.acos(dy / this.radius);

        // 保证初始状态
        this.updateCamera();

        domElement.addEventListener('mousedown', this.onMouseDown, { passive: false });
        domElement.addEventListener('wheel', this.onMouseWheel, { passive: false });
    }

    private onMouseDown = (e: MouseEvent) => {
        this.isDragging = true;
        this.lastX = e.clientX;
        this.lastY = e.clientY;
        window.addEventListener('mousemove', this.onMouseMove);
        window.addEventListener('mouseup', this.onMouseUp);
    };

    private onMouseMove = (e: MouseEvent) => {
        if (!this.isDragging) return;
        const dx = e.clientX - this.lastX;
        const dy = e.clientY - this.lastY;
        this.lastX = e.clientX;
        this.lastY = e.clientY;

        this.theta -= dx * 0.01;
        this.phi -= dy * 0.01;
        this.phi = Math.max(0.01, Math.min(Math.PI - 0.01, this.phi));
        this.updateCamera();
    };

    private onMouseUp = () => {
        this.isDragging = false;
        window.removeEventListener('mousemove', this.onMouseMove);
        window.removeEventListener('mouseup', this.onMouseUp);
    };

    private onMouseWheel = (e: WheelEvent) => {
        this.radius += e.deltaY * 0.01;
        this.radius = Math.max(1, this.radius);
        this.updateCamera();
    };

    private updateCamera() {
        const x = this.target.x  + this.radius * Math.sin(this.phi) * Math.sin(this.theta);
        const y = this.target.y  + this.radius * Math.cos(this.phi);
        const z = this.target.z  + this.radius * Math.sin(this.phi) * Math.cos(this.theta);
        this.camera.setPosition(x, y, z);
        this.camera.lookAt(this.target.x, this.target.y, this.target.z);
    }

    
    dispose() {
        this.domElement.removeEventListener('mousedown', this.onMouseDown);
        this.domElement.removeEventListener('wheel', this.onMouseWheel);
        window.removeEventListener('mousemove', this.onMouseMove);
        window.removeEventListener('mouseup', this.onMouseUp);
    }
}