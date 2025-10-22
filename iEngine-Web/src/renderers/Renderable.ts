// 渲染指令（临时 POD）
// Renderable 是纯数据接口，不引用 Node 或 Component
// 仅用于临时生成渲染所需的数据，用完即销毁
import { Matrix4 } from '../math';
import { RenderLayerID } from './RenderLayer';

import type { Mesh } from '../core/Mesh';
import type { Material } from '../materials/Material';


export interface Renderable {
  mesh: Mesh;
  material: Material;
  worldTransform: Matrix4;
  layer: RenderLayerID;
};