import type { GraphicsAPI } from '../renderers/Renderer';
import type { DefineMap } from './ShaderLib';

export class ShaderPreprocessor {
    static preprocessGlsl(
        source: string,
        graphicsAPI: GraphicsAPI,
        type: 'vertex' | 'fragment',
        defines: DefineMap = {}
    ): string {
        // 1. 去除前导空白，以避免前后出现空行或空格，从而导致Shader代码的版本信息失效
        let code = source.trimStart();

        // // 2. 版本声明
        // if (graphicsAPI === 'webgl2') {
        //     code = '#version 300 es\n' + code;
        // }

        // // 3. 精度声明（WebGL2 需要手动加，且要在 #version 之后））
        // if (graphicsAPI === 'webgl2' && !/precision\s+mediump\s+float;/.test(code)) {
        //     // code = 'precision mediump float;\n' + code;
        //     code = code.replace(/(#version 300 es\n)/, '$1precision mediump float;\n');
        // }

        // 2. 版本声明
        let header = '';
        if (graphicsAPI === 'webgl2') {
            header += '#version 300 es\n';
        }

        // 3. 精度声明
        if (graphicsAPI === 'webgl2' && !/precision\s+mediump\s+float;/.test(code)) {
            header += 'precision mediump float;\n';
        }

        // 4. 宏定义
        // const macroStr = Object.entries(defines)
        //     .map(([k, v]) => `#define ${k} ${v}`)
        //     .join('\n');
        
        // 要处理布尔类型，防止出现 #define USE_LIGHTING true
        const macroStr = Object.entries(defines)
            .map(([k, v]) => {
                if (v === true) return `#define ${k}`;
                if (v === false) return ''; // 不定义
                return `#define ${k} ${v}`;
            })
            .filter(Boolean)
            .join('\n');
        if (macroStr) header += macroStr + '\n';

        code = header + code;

        // 5. attribute/varying 替换
        if (graphicsAPI === 'webgl2') {
            code = code
                .replace(/\battribute\b/g, 'in')
                .replace(/\bvarying\b/g, type === 'vertex' ? 'out' : 'in');
        }

        // 6. texture2D/texture 替换
        if (graphicsAPI === 'webgl2') {
            code = code.replace(/\btexture2D\b/g, 'texture');
        }

        // 7. 片元着色器输出
        if (graphicsAPI === 'webgl2' && type === 'fragment') {
            // 自动插入 out 变量
            code = code.replace(
                /void\s+main\s*\(\s*\)\s*{/,
                'out vec4 outColor;\nvoid main() {'
            );
            // 替换 gl_FragColor
            code = code.replace(/\bgl_FragColor\b/g, 'outColor');
        }

        return code;
    }



    /**
     * 
     * 
     * 建议使用 “始终绑定所有贴图，无贴图绑定默认纹理” 的方式：
     * 优点：
     *     不需要预处理 WGSL 源码，避免引入 JS 端错误或调试困难；
     *     渲染管线简单统一，shader 变体数量减少；
     *     与现代主流引擎一致（如：Unity、UE、Babylon.js、Filament）；
     *     支持 GPU pipeline cache（driver 更可能缓存结果）；
     *     便于 GPU bind group layout 的固定化，提升性能。
     * 缺点：
     *     多绑定几个纹理，即使用不到会略多一点 GPU memory（但实际 negligible）；
     *     加载初始化时需要准备一张默认纹理（通常只有 1x1 像素）；
     * 
     * @param wgsl 
     * @param defines 
     * @returns 
     */
    static preprocessWgsl(wgsl: string, defines: DefineMap = {}) {
        // let processed = wgsl;
        // for (const [key, value] of Object.entries(defines)) {
        //     const macro = new RegExp(`@define\\s+${key}\\b`, 'g');
        //     processed = processed.replace(macro, `${value}`);
        // }
        // return processed;

        // 先处理带代码块的宏，例如：
        // @define HAS_NORMAL {
        //     @location(1) aNormal: vec3<f32>,
        // }
        for (const [key, value] of Object.entries(defines)) {
            const blockPattern = new RegExp(`@define\\s+${key}\\s*\\{([\\s\\S]*?)\\}`, 'g');
            wgsl = wgsl.replace(blockPattern, (_match, code) => value ? code : '');
        }

        // 再处理单行宏标记，例如：
        // @define HAS_NORMAL
        const singleLinePattern = /@define\s+\w+.*\n/g;
        wgsl = wgsl.replace(singleLinePattern, '');

        return wgsl;
    }
}