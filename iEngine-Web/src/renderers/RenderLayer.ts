import type { Renderable } from "./Renderable";

export enum RenderLayerID {
    Opaque = 0,
    Transparent = 1,
    ShadowCaster = 2,
    UI = 3,
    Custom0 = 4,
};

export interface RenderLayer {
    id: RenderLayerID;
    renderQueue: Renderable[];
    needsSorting: boolean;
};
