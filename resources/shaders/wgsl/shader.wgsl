const PI = 3.14159265359;

/* **************** BINDINGS **************** */

struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) color: vec3f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec3f,
	@location(1) normal: vec3f,
};

/**
 * A structure holding the value of our uniforms
 */
struct GlobalUniforms {
	projectionMatrix: mat4x4f,
	viewMatrix: mat4x4f,
	cameraWorldPosition: vec3f,
	time: f32,
	gamma: f32,
};

/**
 * Uniforms specific to a given GLTF node.
 */
struct NodeUniforms {
	modelMatrix: mat4x4f,
}

// General bind group
@group(0) @binding(0) var<uniform> uGlobal: GlobalUniforms;

// Node bind group
@group(1) @binding(0) var<uniform> uNode: NodeUniforms;

/* **************** VERTEX MAIN **************** */

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;

	// let worldPosition = uNode.modelMatrix * vec4f(in.position, 1.0);
	// out.position = uGlobal.projectionMatrix * uGlobal.viewMatrix * worldPosition;

	// 定义4x4的单位矩阵
	// var unitMatrix = mat4x4f(
	// 	1.0, 0.0, 0.0, 0.0,
	// 	0.0, 1.0, 0.0, 0.0,
	// 	0.0, 0.0, 1.0, 0.0,
	// 	0.0, 0.0, 0.0, 1.0
	// );
	// out.position = unitMatrix * vec4f(in.position, 1.0);

    out.position = uGlobal.projectionMatrix * uGlobal.viewMatrix * uNode.modelMatrix * vec4f(in.position, 1.0);

	out.normal = (uNode.modelMatrix * vec4f(in.normal, 0.0)).xyz;
	out.color = in.color;
	return out;
}

/* **************** FRAGMENT MAIN **************** */

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	let normal = normalize(in.normal);

	let lightColor1 = vec3f(1.0, 0.9, 0.6);
	let lightColor2 = vec3f(0.6, 0.9, 1.0);
	let lightDirection1 = vec3f(0.5, -0.9, 0.1);
	let lightDirection2 = vec3f(0.2, 0.4, 0.3);
	let shading1 = max(0.0, dot(lightDirection1, normal));
	let shading2 = max(0.0, dot(lightDirection2, normal));
	let shading = shading1 * lightColor1 + shading2 * lightColor2;
	let color = in.color * shading;

	// Gamma-correction
	let corrected_color = pow(color, vec3f(2.2));
	return vec4f(corrected_color, 1.0);
}
