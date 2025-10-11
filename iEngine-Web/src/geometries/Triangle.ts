import { Geometry } from './Geometry';

export class Triangle extends Geometry {
  constructor() {
    const vertices = new Float32Array([
      0.0, 1.0, 0.0,
     -1.0, -1.0, 0.0,
      1.0, -1.0, 0.0
    ]);
    super({ vertices });
  }
}
