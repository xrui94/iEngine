# iEngine-web
iEngine for web(Test engine architecture). A tiny real-time graphics rendering engine based on webgl and webgpu.

## Getting Start

### Install Dependencies

Run the following command in the repo folder (e.g. iEngine-web)

```
npm install
```

### Build Library

For development environment (uncompressed, obfuscated code), Run:

```
npm run build-dev
```

For production environment，Run:

```
npm run build
```

Then, it will generate files at dist/

- iengine.js
- iengine.js.map
- types/... (Some *.d.ts types of files (for debugging))

### Run examples

Firstly, open the iEngine-web project using VSCode.
Then, open the "examples/index.html" file in VSCode.
Last, run it using the **"Live Server"** (author: Ritwick Dey) plugin of VSCode.

**Tip**: For more usage, please refer to the "examples/index.html" file.

## TODO

In the future, I will add the following designs:

- SceneLayer
- RenderLayer
- RenderGraph
- RenderPass
  - DepthPass
  - ShadowPass
  - PostProcessing
  - ...
- ...

Next, the general working logic of the engine will be updated as shown in the pseudocode below.

```js
Engine.frameLoop():
  for scene of this.scenes:
    renderer.render(scene)

Renderer.render(scene):
  const activeCamera = scene.activeCamera
  const context = scene.activeContext
  const renderGraph = scene.renderGraph

  // 每帧更新 Scene → RenderLayer（包括视锥裁剪、RenderQueue 构建）
  scene.updateRenderLayers()

  // 执行渲染图（按 Pass 顺序调度）
  renderGraph.execute(this, scene)

Scene.updateRenderLayers():
  for sceneLayer of this.sceneLayers:
    if (sceneLayer.visible):
      const renderLayers = sceneLayer.getRenderLayers()
      for rl of renderLayers:
        rl.collectRenderables() // 视锥裁剪 + push 到 RenderQueue

RenderGraph.execute(renderer, scene):
  for pass of this.passes:
    pass.execute(renderer, scene)

RenderPass.execute(renderer, scene):
  context.beginRenderPass(...)

  for renderLayer of scene.renderLayers:
    if (renderLayer.supportsPass(this.passType)):
      for renderable of renderLayer.renderQueue:
        this.drawRenderableInPass(renderable, renderer, scene)

  context.endRenderPass()

RenderPass.drawRenderableInPass(renderable, renderer, scene):
  const material = renderable.getMaterial()
  const mesh = renderable.getMesh()

  const passMaterial = material.getPassMaterial(this.passType)
  const shader = ShaderLib.getOrCompile(passMaterial.shaderName, passMaterial.getDefines())
  const pipeline = renderer.pipelineCache.getOrCreate(shader, passMaterial.getRenderState(), mesh.layout)

  pipeline.bind(context)
  mesh.uploadIfNeeded(context)

  shader.setUniforms(passMaterial.getUniforms(renderable, scene.camera, scene.lights))
  shader.setTextures(passMaterial.getTextures())

  context.draw(mesh, passMaterial.drawMode)
  pipeline.unbind?.()
```
