import { Vector3 } from '../math/Vector3';
import { Triangle } from '../geometries/Triangle';
import { Cube } from '../geometries/Cube';
import { Model } from '../core/Model';
import { PrimitiveType, Primitive } from '../core/Primitive';
import { Color } from '../materials/Color';
import { BaseMaterial } from '../materials/BaseMaterial';
import { PbrMaterial } from '../materials/PbrMaterial';
import { Mesh } from '../core/Mesh';
import { Scene } from './Scene';
import { WebGLContext, WebGLContextOptions } from '../renderers/webgl/WebGLContext';
import { WebGPUContext, WebGPUContextOptions } from '../renderers/webgpu/WebGPUContext';

export class SimpleScene extends Scene {
    /**
     * 创建一个简单的场景，该场景提供了一些基本的几何体模型创建方法。
     */    
    constructor(canvas: string | HTMLCanvasElement, options: WebGLContextOptions | WebGPUContextOptions) {
        super(canvas, options);
    }

    /**
     * 创建一个红色三角形模型并添加到场景中
     */
    public createTriangle(): Model {
        // 添加triangle
        const triangleMesh = new Mesh(
            new Triangle(),
            new Primitive(
                PrimitiveType.TRIANGLES,
                // {
                //     name: 'aPosition',
                //     size: 3,
                //     type: WebGLRenderingContext.FLOAT,
                //     normalized: false,
                //     stride: 12,
                //     offset: 0,
                // },
                {
                    arrayStride: 12,
                    attributes: [
                        {
                        name: 'aPosition',
                        format: 'float32x3',
                        offset: 0,
                        shaderLocation: 0
                        }
                    ]
                },
            )
        );

        //
        const material = new BaseMaterial({
            shaderName: 'base_material'
        });
        material.setColor(1.0, 0.0, 0.0); // 红色

        //
        const triangleModel = new Model('Triangle', triangleMesh, material);

        // // 添加一个自转动画
        // triangleModel.addAnimation((m, dt) => {
        //     m.rotate(new Vector3(0, 1, 0), dt * 0.5); // 每秒旋转0.5弧度
        // });

        // this.addComponent(triangleModel);
        return triangleModel;
    }

    /**
     * 创建一个立方体模型并添加到场景中
     */
    public createCube(): Model {
        // 添加Cube
        const cubeMesh = new Mesh(
            new Cube(),
            new Primitive(
                PrimitiveType.TRIANGLES,
                // [
                //     {
                //         name: 'aPosition',
                //         size: 3,
                //         type: WebGLRenderingContext.FLOAT,
                //         normalized: false,
                //         stride: 12,
                //         offset: 0,
                //     },
                //     {
                //         name: 'aNormal',
                //         size: 3,
                //         type: WebGLRenderingContext.FLOAT,
                //         normalized: false,
                //         stride: 12,
                //         offset: 0,
                //     },
                // ]
            )
        );

        //
        // const cubeMaterial = new BaseMaterial('base_material');
        // cubeMaterial.setColor(0.0, 0.5, 1.0); // 蓝色

        const cubeMaterial = new PbrMaterial({
            shaderName: 'base_pbr',
            baseColor: new Color(1.0, 1.0, 1.0),
            // baseColorMap: '../assets/textures/Wood048_1K-PNG/Wood048_1K-PNG_Color.png',
            // baseColorMap: '../assets/textures/building-facade-windows_256x256.jpg',
            baseColorMap: '../assets/textures/Metal001_1K-PNG/Metal001_1K-PNG_Color.png',
            metallic: 0.8,
            roughness: 0.01
        });

        //
        const cubeModel = new Model('Cube', cubeMesh, cubeMaterial);
        
        // 添加一个自转动画
        // cubeModel.addAnimation((m, dt) => {
        //     m.rotate(new Vector3(0, 1, 0), dt * 0.5); // 每秒旋转0.5弧度
        // });
        
        // this.addComponent(cubeModel);
        return cubeModel;
    }

    /**
     * 创建一个黄色线框立方体模型并添加到场景中
     */
    public createWireframeCube(): Model {
        // 添加Wireframe Cube
        const wireframeCubeMesh = new Mesh(
            new Cube(),
            new Primitive(
                PrimitiveType.LINES,
                // [
                //     {
                //         name: 'aPosition',
                //         size: 3,
                //         type: WebGLRenderingContext.FLOAT,
                //         normalized: false,
                //         stride: 12,
                //         offset: 0,
                //     },
                // ]
            )
        );
        
        // 创建一个黄色线框材质
        const wireframeMaterial = new BaseMaterial({
            shaderName: 'base_wireframe'
        });
        wireframeMaterial.setColor(1.0, 1.0, 0.0); // 黄色
        
        //
        const wireframeCubeModel = new Model('CubeWireframe', wireframeCubeMesh, wireframeMaterial);
        
        // 添加一个自转动画
        // wireframeCubeModel.addAnimation((m, dt) => {
        //     m.rotate(new Vector3(0, 1, 0), dt * 0.5); // 每秒旋转0.5弧度
        // });
        
        // this.addComponent(wireframeCubeModel);
        return wireframeCubeModel;
    }
}