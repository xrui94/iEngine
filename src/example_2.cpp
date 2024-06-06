// #include "StringUtil.hpp"

#ifdef __EMSCRIPTEN__
    #include <GLES2/gl2.h>
    #include <emscripten.h>
    #include <emscripten/bind.h>
    #include <emscripten/val.h>
    #include <emscripten/threading.h>
    #include <emscripten/html5_webgl.h>
    #include <emscripten/html5.h>
#endif

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#include <iostream>
#include <string>
 
#define SR_OK 0;
#define SR_FAIL 1;
 
GLuint g_ShaderProgram;
GLuint g_Texture2D[3]; 
GLuint g_vertexPosBuffer;
GLuint g_texturePosBuffer;
 
 
unsigned char* pYUVData=NULL;
 
int g_yuvLen=0;
int g_nFrameWidth  = 704;
int g_nFrameHeight = 576;
int g_size=0;
const char* g_pHwd=NULL;

/*顶点着色器代码*/
static const char g_pGLVS[] =              ///<普通纹理渲染顶点着色器
"attribute vec4 position;"
"attribute vec4 acolor;"
"varying vec4 color;"
"void main() {"
"color = acolor;"
"gl_Position = position;"
"}";
 
/*像素着色器代码*/
static const char g_pGLFS[] =              ///<YV12片段着色器
"precision lowp float;"
"varying vec4 color;"
"void main() {"
"gl_FragColor = color;"
"}";
 
static const float posAndColor[] = {
//     x,     y, r, g, b
    -0.6f, -0.6f, 1, 0, 0,
    0.6f, -0.6f, 0, 1, 0,
    0.f,   0.6f, 0, 0, 1,
};

 
// EM_JS(void, call_alert, (std::string hWnd), {
// 	var hWin=hWnd.to_string();
// 	console.log("EM_JS hWin:"+hWin);
// 	var canvas = document.getElementById(hWin);
// 	Module['canvas'].parentElement.appendChild(canvas);
// 	canvas.id = hWin;
//     console.log('aaaaaaaaaaaaaa')
// });

// EM_JS(void, call_alert, (std::string hWnd), {
// 	// var hWin = hWnd.to_string();
// 	// console.log("EM_JS hWin:"+hWin);
// 	// var canvas = document.getElementById(hWin);
// 	// Module['canvas'].parentElement.appendChild(canvas);
// 	// canvas.id = hWin;

//     const containerId = hWnd.to_string();

//     const canvas = document.createElement('canvas');
//     canvas.id = 'engine-canvas';
//     canvas.width = 400;
//     canvas.height = 300;
//     const offscreen = canvas.transferControlToOffscreen();
//     Module.offscreenCanvas = offscreen;
//     const parentElement = document.getElementById(containerId);
//     if (parentElement?.tagName === 'DIV') {
//         parentElement.appendChild(canvas);
//     }

//     // var hWin = hWnd.to_string();
// 	// console.log("EM_JS hWin:"+hWin);
// 	// var canvas = document.getElementById(hWin);
// 	// Module['canvas'].parentElement.appendChild(canvas);
// 	// canvas.id = hWin;
// });

// EM_ASM({
//     const containerId = UTF8ToString($0);

//     const canvas = document.createElement('canvas');
//     canvas.id = 'engine-canvas';
//     canvas.width = 400;
//     canvas.height = 300;
//     const offscreen = canvas.transferControlToOffscreen();
//     Module.offscreenCanvas = offscreen;
//     const parentElement = document.getElementById(containerId);
//     if (parentElement?.tagName === 'DIV') {
//         parentElement.appendChild(canvas);
//     }
// }, containerId.c_str());
 

void initBuffers()
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posAndColor), posAndColor, GL_STATIC_DRAW);
 
    GLint nPosLoc = glGetAttribLocation(g_ShaderProgram, "position");
    glEnableVertexAttribArray(nPosLoc);
    glVertexAttribPointer(nPosLoc, 2, GL_FLOAT, GL_FALSE, 20, 0);
    // glBindBuffer(GL_ARRAY_BUFFER, NULL);
    // g_vertexPosBuffer=vertexPosBuffer;

    GLint nColorLoc = glGetAttribLocation(g_ShaderProgram, "acolor");
    glEnableVertexAttribArray(nColorLoc);
    glVertexAttribPointer(nColorLoc, 3, GL_FLOAT, GL_FALSE, 20, (void*)8);
    // glBindBuffer(GL_ARRAY_BUFFER, NULL);
 
    // GLuint texturePosBuffer;
    // glGenBuffers(1, &texturePosBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, texturePosBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(g_TexCoord), g_TexCoord, GL_STATIC_DRAW);
 
    // GLint nTexcoordLoc = glGetAttribLocation(g_ShaderProgram, "texCoord");
    // glEnableVertexAttribArray(nTexcoordLoc);
    // glVertexAttribPointer(nTexcoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // glBindBuffer(GL_ARRAY_BUFFER, NULL);
    // g_texturePosBuffer=texturePosBuffer;
}

// void initTexture()
// {
//     glUseProgram(g_ShaderProgram);
//     GLuint          nTexture2D[3];        ///<< YUV三层纹理数组
//     glGenTextures(3, nTexture2D);
//     for(int i = 0; i < 3; ++i)
//     {
//         glBindTexture(GL_TEXTURE_2D, nTexture2D[i]);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glBindTexture(GL_TEXTURE_2D, NULL);
//     }
    
//     GLuint          nTextureUniform[3];
//     nTextureUniform[Y] = glGetUniformLocation(g_ShaderProgram, "Ytexture");
//     nTextureUniform[U] = glGetUniformLocation(g_ShaderProgram, "Utexture");
//     nTextureUniform[V] = glGetUniformLocation(g_ShaderProgram, "Vtexture");
//     glUniform1i(nTextureUniform[Y], 0);
//     glUniform1i(nTextureUniform[U], 1);
//     glUniform1i(nTextureUniform[V], 2);
//     g_Texture2D[0]=nTexture2D[0];
//     g_Texture2D[1]=nTexture2D[1];
//     g_Texture2D[2]=nTexture2D[2];
//     glUseProgram(NULL);
// }

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
  else {
    printf("%s\n", "Failed to compile shader.");
  }

   return shader;
}

GLuint createProgram_old(GLuint vertexShader, GLuint fragmentShader)
{
   GLuint program = glCreateProgram();
   glAttachShader(program, vertexShader);
   glAttachShader(program, fragmentShader);
   glBindAttribLocation(program, 0, "apos");
   glBindAttribLocation(program, 1, "acolor");
   glLinkProgram(program);
   return program;
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLint nLinkRet;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &nLinkRet);
    if(0 == nLinkRet)
    {
        printf("%s\n", "Failed to link shader program.");
        return SR_FAIL;
    }
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

GLuint initShaderProgram()
{
    ///< 顶点着色器相关操作
    GLuint vs = compileShader(GL_VERTEX_SHADER, g_pGLVS);

    ///< 片段着色器相关操作
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, g_pGLFS);

	///<program相关
    return createProgram(vs, fs);
}

void initContextGL()
{
    // double dpr = emscripten_get_device_pixel_ratio();
    // emscripten_set_element_css_size(hWindow, nWmdWidth / dpr, nWndHeight / dpr);
    // emscripten_set_canvas_element_size(hWindow, nWmdWidth, nWndHeight);
    // printf("create size success\n");

    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.explicitSwapControl = EM_TRUE;
    attr.alpha = 0;
#if MAX_WEBGL_VERSION >= 2
    attr.majorVersion = 2;
#endif
    printf("create context fallback\n");
    attr.proxyContextToMainThread = EMSCRIPTEN_WEBGL_CONTEXT_PROXY_FALLBACK;//EMSCRIPTEN_WEBGL_CONTEXT_PROXY_FALLBACK
    attr.renderViaOffscreenBackBuffer = EM_TRUE;
    std::cout << "@@@@@@@@@@@@@@@-----" << g_pHwd << std::endl;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext = emscripten_webgl_create_context(g_pHwd, &attr);
    assert(glContext);
    //emscripten_set_canvas_element_size(g_pHwd, 355, 233);
    EMSCRIPTEN_RESULT res=emscripten_webgl_make_context_current(glContext);
    assert(res == EMSCRIPTEN_RESULT_SUCCESS);
    assert(emscripten_webgl_get_current_context() == glContext);
    
	printf("create context success 0821\n");
}

bool RenderFrame(int nFrameWidth, int nFrameHeight)
{
    glUseProgram(g_ShaderProgram);
    //将所有顶点数据上传至顶点着色器的顶点缓存
    #ifdef DRAW_FROM_CLIENT_MEMORY
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, pos_and_color);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (void*)(pos_and_color+2));
    #else
        GLint nPosLoc = glGetAttribLocation(g_ShaderProgram, "position");
        glEnableVertexAttribArray(nPosLoc);
        glVertexAttribPointer(nPosLoc, 2, GL_FLOAT, GL_FALSE, 20, 0);
        // glBindBuffer(GL_ARRAY_BUFFER, NULL);

        GLint nColorLoc = glGetAttribLocation(g_ShaderProgram, "acolor");
        glEnableVertexAttribArray(nColorLoc);
        glVertexAttribPointer(nColorLoc, 3, GL_FLOAT, GL_FALSE, 20, (void*)8);
        // glBindBuffer(GL_ARRAY_BUFFER, NULL);
    #endif

    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);

    glClearColor(0.3f,0.3f,0.3f,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
 
    #define EXPLICIT_SWAP
    #ifdef EXPLICIT_SWAP
        printf("EXPLICIT_SWAP emscripten_webgl_commit_frame\n");
        emscripten_webgl_commit_frame();
    #endif
 
    #ifdef REPORT_RESULT
        printf("REPORT_RESULT\n");
        REPORT_RESULT(0);
    #endif

    glUseProgram(NULL);
    return true;
}

EM_BOOL requestRender(double time,void* p)
{
    printf("requestRender  1 \n");  
  // while(1)
    {
        usleep(40*1000);
        RenderFrame(g_nFrameWidth, g_nFrameHeight);
    }
    // emscripten_request_animation_frame(requestRender,(void*)1);
  
    return 1; 
}

void request(void *)
{
    printf("request\n");
    emscripten_request_animation_frame(requestRender,(void*)1);
}

void* initRender(void *hWindow)
{ 
    printf("start subThread init\n");

    //初始化Context
    initContextGL();

    //初始化着色器程序
    g_ShaderProgram=initShaderProgram();

    //初始化buffer
    initBuffers();

    RenderFrame(0, 0);

    //初始化纹理
    // initTexture();
    // printf("init success\n");
	// printf("yuvLen:%d,g_size:%d\n",g_yuvLen,g_size);
	// if(pYUVData==NULL||g_yuvLen<g_size)
	// {
	// 	if(pYUVData!=NULL)
	// 	{
	// 		delete []pYUVData;
	// 		pYUVData=NULL;
	// 	}
	// 	pYUVData=new unsigned char[g_size];
	// 	if(pYUVData==NULL)
	// 		return NULL;
	// 	g_yuvLen=g_size;
	// }
	// printf("new yuv buffer success\n");
	
    glClearColor(1.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    printf("glClearColor\n");
    // if(NULL != pYUVData)
    // {
    //     pFile = fopen("./704_576.yuv", "rb");
	// 	if(NULL == pFile)
	// 	{
	// 		printf("open YUV file failed");
	// 		return NULL;
	// 	}
        
    // }
    printf("glClearColor  222 \n");
    // emscripten_async_call(request,0,1);
    return hWindow;
}

// EM_JS(void, createCanvas, (const char* containerId, const char* canvasId), {
//     const canvas = document.createElement('canvas');
//     canvas.id = UTF8ToString(canvasId);
//     canvas.width = 400;
//     canvas.height = 300;

//     console.log('containerId----------->', UTF8ToString(containerId));
//     const parentElement = document.getElementById(UTF8ToString(containerId));
//     console.log('parentElement----------->', parentElement);
//     if (parentElement?.tagName === 'DIV') {
//         parentElement.appendChild(canvas);
//     }

//     console.log(Module)
//     // Module['canvas'] = canvas;

//     // Module['canvas'].parentElement.appendChild(canvas);
//     // var hWin=hWnd.to_string();
//     // console.log("EM_JS hWin:"+hWin);
//     // var canvas = document.getElementById(hWin);
//     // Module['canvas'].parentElement.appendChild(canvas);
//     // canvas.id = hWin;
//     // console.log('aaaaaaaaaaaaaa')
// });

#ifdef __EMSCRIPTEN__
    void createElement(const std::string& containerId, const std::string& canvasId, const std::string& cssText)
    {
        emscripten::val canvas = emscripten::val::global("document").call<emscripten::val>("createElement", emscripten::val("canvas"));
        canvas.set("id", canvasId.substr(1).c_str());   // 当在JS中传过来的Canvas加了“#”号时，要去掉“#”号
        // // canvas.set("id", canvasId.c_str());  // 在JS中传过来的Canvas没有“#”号时，则无须处理，直接使用
        // canvas.set("style.width", "100%");
        // canvas.set("style.height", "100%");
        // canvas.set("style.margin", "0");
        // canvas.set("style.padding", "0");

        if (cssText.size() == 0)
        {
            canvas.call<void>("setAttribute", emscripten::val("style"), emscripten::val("width: 100%; height: 100%; margin: 0; padding: 0"));
        }
        else
        {
            canvas.call<void>("setAttribute", emscripten::val("style"), emscripten::val(cssText.c_str()));
        }

        //
        emscripten::val parentElement = emscripten::val::global("document").call<emscripten::val>("getElementById", emscripten::val(containerId.c_str()));
        parentElement.call<void>("appendChild", canvas);

        // emscripten::val updatedDiv = emscripten::val::global("document").call<emscripten::val>("getElementById", emscripten::val("myDiv"));
        // updatedDiv.set("innerHTML", "Updated content of the div element.");
    }
#endif
 
bool initEngine(int nFrameWidth, int nFrameHeight, const std::string& containerId, const std::string& hwnd, const std::string& cssText) {
    // EM_ASM({
    //     const containerId = UTF8ToString($0);
    //     const canvasId = UTF8ToString($1);
    //     // const containerId = $0.to_string();
    //     // const canvasId = $1.to_string();

    //     const canvas = document.createElement('canvas');
    //     canvas.id = canvasId;
    //     canvas.width = 400;
    //     canvas.height = 300;
    //     // const offscreen = canvas.transferControlToOffscreen();
    //     // Module.offscreenCanvas = offscreen;
    //     // const parentElement = document.getElementById(containerId);
    //     // if (parentElement?.tagName === 'DIV') {
    //     //     parentElement.appendChild(canvas);
    //     // }

    //     // Module['canvas'] = canvas;
    //     console.log(Module['canvas'])
    //     Module['canvas'].parentElement.appendChild(canvas);
    // }, containerId.c_str(), g_pHwd);

    // createCanvas(containerId.c_str(), hwnd.c_str());

    g_pHwd = hwnd.c_str();  // C++中加入“#”号，会导致乱码，因此暂且在JS中加“#”号

    createElement(containerId, hwnd, cssText);

    // g_pHwd=(toUtf8(std::string("#")) + hwnd).c_str();   // 需要加上“#”号，但C++中这里加入，会导致乱码，暂且在JS中加“#”号
    std::cout << " g_pHwd------------------>" << g_pHwd << std::endl;


    g_nFrameHeight=nFrameHeight;
    g_nFrameWidth=nFrameWidth;
    // g_size=g_yuvLen;
    printf("InitMain: nFrameWidth:%d,nFrameHeight:%d,hwnd:%s\n",nFrameWidth,nFrameHeight, g_pHwd);
    if (!emscripten_supports_offscreencanvas())
    {
        printf("Current browser does not support OffscreenCanvas. Skipping the rest of the tests.\n");
#ifdef REPORT_RESULT
        REPORT_RESULT(1);
#endif
        return false;
    }
 
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    emscripten_pthread_attr_settransferredcanvases(&attr, g_pHwd);
    pthread_t thread;
    printf("Creating thread.\n");
    pthread_create(&thread, &attr, initRender, NULL);
    pthread_detach(thread);
    // EM_ASM(noExitRuntime=true);
 
    return true;
}

// #ifdef __EMSCRIPTEN__

    emscripten::val _createEngine(const std::string& containerId, const std::string& canvasId, const std::string& cssText = "") {
        bool res = initEngine(400, 300, containerId, canvasId, cssText);
        return emscripten::val(res);
    }

    EMSCRIPTEN_BINDINGS(wasmMain)
    {
        emscripten::function("createEngine", &_createEngine);
        // emscripten::function("loadOsgb_t", &_loadOsgb_t);
    }

// #endif


// # Ref：
// web端 子线程调用opengl es：https://blog.csdn.net/qq_34754747/article/details/108150004
// 使用C、C++ 和Rust中的WebAssembly线程,以及报"Tried to spawn a new thread, but the thread pool is exhausted"错误的原因和解决方案: https://web.dev/articles/webassembly-threads?hl=zh-cn