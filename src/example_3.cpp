#include <stdio.h>
#include <pthread.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/threading.h>

#include <string>


const char* g_canvasId = nullptr;

void* threadFunc(void* arg)
{

	const EmscriptenWebGLContextAttributes attr = {
		.alpha = 1,
		.depth = 1,
		.stencil = 1,
		.antialias = 1,
		.explicitSwapControl = 1,
		.majorVersion = 2,
		.minorVersion = 0,
	};

	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(g_canvasId, &attr);
	if (context == 0) {
		printf ("error can't create context\n");
		return NULL;
	}
	if (emscripten_webgl_make_context_current(context) != EMSCRIPTEN_RESULT_SUCCESS) {
		printf("error can't make context current\n");
		return NULL;
	}

	EM_ASM("console.log(GL.currentContext);");
	emscripten_webgl_commit_frame();

	emscripten_webgl_make_context_current(0);
	emscripten_webgl_destroy_context(context);

	return NULL;
}

int init(const std::string& containerId, const std::string& canvasId = "t-canvas")
{
    g_canvasId = canvasId.c_str();
    EM_ASM({
        const containerId = UTF8ToString($0);
        const canvasId = UTF8ToString($1);

        const canvas = document.createElement('canvas');
        canvas.id = canvasId;
        canvas.width = 400;
        canvas.height = 300;
        // const offscreen = canvas.transferControlToOffscreen();
        // Module.offscreenCanvas = offscreen;
        const parentElement = document.getElementById(containerId);
        if (parentElement?.tagName === 'DIV') {
            parentElement.appendChild(canvas);
        }

        Module['canvas'] = canvas;
    }, containerId.c_str(), g_canvasId);

	pthread_t pth;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	emscripten_pthread_attr_settransferredcanvases(&attr, g_canvasId);
	pthread_create(&pth, &attr, threadFunc, NULL);

	return 0;
}


// EMSCRIPTEN_KEEPALIVE
// bool_createEngine(const std::string& containerId) {
// 	bool res = init(containerId);
// 	return res;
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