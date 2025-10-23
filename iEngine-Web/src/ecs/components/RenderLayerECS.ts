import { RenderLayerID } from '../../renderers/RenderLayer';

export class RenderLayerECS {
    constructor(public layer: RenderLayerID = RenderLayerID.Opaque) {}
}