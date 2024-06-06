// example.c
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>

#include <cstdio>
#include <string>

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glCtx;

GLuint compileShader(GLenum shaderType, const char *src)
{
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (!isCompiled)
  {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char *buf = (char*)malloc(maxLength+1);
    glGetShaderInfoLog(shader, maxLength, &maxLength, buf);
    printf("%s\n", buf);
    free(buf);
    return 0;
  }

   return shader;
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader)
{
   GLuint program = glCreateProgram();
   glAttachShader(program, vertexShader);
   glAttachShader(program, fragmentShader);
   glBindAttribLocation(program, 0, "apos");
   glBindAttribLocation(program, 1, "acolor");
   glLinkProgram(program);
   return program;
}

bool init(const std::string& containerId) {
    EM_ASM({
        const containerId = UTF8ToString($0);

        const canvas = document.createElement('canvas');
        canvas.id = 'engine-canvas';
        canvas.width = 400;
        canvas.height = 300;
        const offscreen = canvas.transferControlToOffscreen();
        Module.offscreenCanvas = offscreen;
        const parentElement = document.getElementById(containerId);
        if (parentElement?.tagName === 'DIV') {
            parentElement.appendChild(canvas);
        }
    }, containerId.c_str());

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    
    #ifdef EXPLICIT_SWAP
        attr.explicitSwapControl = 1;
    #endif

    #ifdef DRAW_FROM_CLIENT_MEMORY
        // This test verifies that drawing from client-side memory when enableExtensionsByDefault==false works.
        attr.enableExtensionsByDefault = 0;
    #endif

    // glCtx = emscripten_webgl_create_context("#engine-canvas", &attrs);
    glCtx = emscripten_webgl_create_context("offscreenCanvas", &attrs);
    if (!glCtx) {
        printf("Failed to create WebGL context.\n");
        return false;
    }

    // Initialize WebGL2
    // Your WebGL initialization code here
    emscripten_webgl_make_context_current(glCtx);

    static const char vertex_shader[] =
        "attribute vec4 apos;"
        "attribute vec4 acolor;"
        "varying vec4 color;"
        "void main() {"
        "color = acolor;"
        "gl_Position = apos;"
        "}";
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertex_shader);

    static const char fragment_shader[] =
        "precision lowp float;"
        "varying vec4 color;"
        "void main() {"
        "gl_FragColor = color;"
        "}";
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragment_shader);

    GLuint program = createProgram(vs, fs);
    glUseProgram(program);

    static const float pos_and_color[] = {
    //     x,     y, r, g, b
        -0.6f, -0.6f, 1, 0, 0,
        0.6f, -0.6f, 0, 1, 0,
        0.f,   0.6f, 0, 0, 1,
    };

    #ifdef DRAW_FROM_CLIENT_MEMORY
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, pos_and_color);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (void*)(pos_and_color+2));
    #else
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pos_and_color), pos_and_color, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (void*)8);
    #endif

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glClearColor(0.3f,0.3f,0.3f,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    #ifdef EXPLICIT_SWAP
        emscripten_webgl_commit_frame();
    #endif

    #ifdef REPORT_RESULT
        REPORT_RESULT(0);
    #endif

    return true;
}

void render() {
    // Your rendering code here
}



// int main() {
//     init();
//     // emscripten_set_main_loop(render, 0, 1);
//     return 0;
// }

#ifdef __EMSCRIPTEN__
    #include <emscripten/bind.h>
    #include <emscripten/val.h>

    emscripten::val _createEngine(const std::string& containerId) {
        bool res = init(containerId);
        return emscripten::val(true);
    }

    EMSCRIPTEN_BINDINGS(wasmMain)
    {
        emscripten::function("createEngine", &_createEngine);
        // emscripten::function("loadOsgb_t", &_loadOsgb_t);
    }

#endif

                        
// 原文链接：https://blog.csdn.net/lixuefeng2305/article/details/132620927