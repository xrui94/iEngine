import resolve from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';
import terser from '@rollup/plugin-terser';
import typescript from '@rollup/plugin-typescript';
import glsl from 'rollup-plugin-glsl';

export default (commandLineArgs) => {
	const isProduction = commandLineArgs.configProd;

	return {
		input: 'src/index.ts', // 项目入口
		output: {
			// file: 'iengine',
			dir: 'dist',
			format: 'esm',
			entryFileNames: 'iengine.js',         // 主入口文件名
			chunkFileNames: 'iengine-[hash].js',  // 动态导入或共享模块的 chunk 名
			assetFileNames: 'iengine-[hash][extname]', // 静态资源（如 glsl 文件）
			sourcemap: true,
			// preserveModules: true, // 保持模块结构（可选）
			// preserveModulesRoot: 'src' // 保持输出目录结构
		},
		plugins: [
			resolve(), // 支持 node_modules 中的依赖
			commonjs(), // 转换 commonjs 为 ES6
			typescript({
				tsconfig: './tsconfig.json',
				sourceMap: true,
				declaration: true,
				declarationDir: 'dist/types',
			}),
			glsl({
				include: ['**/*.glsl'],
				sourceMap: false
			}),
			// 只在生产环境启用 terser 压缩
			isProduction ? terser() : null
		].filter(Boolean), // 过滤掉 null 插件
		external: ["node_modules", "dist"] // 这里列出不希望被打包进来的依赖，例如 ['three']
	}
};