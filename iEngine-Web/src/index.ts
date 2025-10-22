// export { default as Engine } from './core/Engine'

import { Engine } from "./core/Engine";
import { PerspectiveCamera } from "./views/cameras/PerspectiveCamera";
import { OrthographicCamera } from "./views/cameras/OrthographicCamera";
import { OrbitControls } from "./views/controls/OrbitControls";
import { FirstPersonControls } from "./views/controls/FirstPersonControls";
import { Matrix4 } from "./math/Matrix4";
import { Vector3 } from "./math/Vector3";
import { Scene } from "./scenes/Scene";
import { Primitive, PrimitiveType } from "./core/Primitive";
import { Geometry } from "./geometries/Geometry";
import { Triangle } from "./geometries/Triangle";
import { Cube } from "./geometries/Cube";
import { Mesh } from "./core/Mesh";

import { Color } from "./materials/Color";
import { Material } from "./materials/Material";
import { BaseMaterial } from "./materials/BaseMaterial";
import { PhongMaterial } from "./materials/PhongMaterial";
import { PbrMaterial } from "./materials/PbrMaterial";
import { Texture } from "./textures/Texture";
import { Texture3D } from "./textures/Texture3D";

import { Node } from "./core/Node";

import { Light } from "./lights/Light";
import { LightComponent } from "./lights/LightComponent";
import { AmbientLight } from "./lights/AmbientLight";
import { DirectionalLight } from "./lights/DirectionalLight";
import { PointLight } from "./lights/PointLight";
import { SpotLight } from "./lights/SpotLight";

import { RenderLayerID } from "./renderers/RenderLayer";
import { RenderableComponent } from "./renderers/RenderableComponent";

const iEngine = {
    Engine,
    Vector3,
    Matrix4,
    PerspectiveCamera,
    OrthographicCamera,
    OrbitControls,
    FirstPersonControls,
    Scene,
    //
    Primitive,
    PrimitiveType,
    //
    Mesh,
    //
    Geometry,
    Triangle,
    Cube,
    //
    Color,
    Material,
    BaseMaterial,
    PhongMaterial,
    PbrMaterial,
    Texture,
    Texture3D,
    //
    Light,
    LightComponent,
    AmbientLight,
    DirectionalLight,
    PointLight,
    SpotLight,
    //
    Node,
    //
    RenderLayerID,
    RenderableComponent,
};

export default Object.freeze(iEngine);
