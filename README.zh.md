# 项目说明

基于纯OpenGL和WebGPU标准的实现——“WGPU”（这里是C++项目，因此使用WGPY的C++绑定，即WGPU-Native），实现了一套代码，多端运行。在桌面端，可以选择OpenGL、Vulakn（仅限于基于WGPU）、DirectX（仅限于基于WGPU）、Metal（仅限于基于WGPU）图形驱动框架，在桌面端，可以选择WebGL2、WebGPU图形驱动框架。

- **已知问题:**
  - 基于Emscripten提供的WebGPU实现，可以编译通过，但运行异常；
  - 基于WGPU-Native提供的WebGPU实现，无法在emsdk-v3.1.60上编译通过；在emsdk-v3.1.47上可以编译通过，但同样是运行异常。

## 2. 编译运行

- 首先，创建一个build目录

```ps
mkdir build
cd build
```

- 然后，生成Ninja工程文件

将参数 ** -DCMAKE-TOOLCHAIN-FILE ** 的值替换为你自己电脑上“Emscripten.cmake file”的绝对路径。

- **Note**:
  - 如果电脑上还没有安装 **emscripten tool chain**(或者叫它 **"emsdk"**) , 则需要先根据这个教程先下载安装: [https://emscripten.org/docs/getting_started/downloads.html](https://emscripten.org/docs/getting_started/downloads.html);
  - 如果电脑系统环境中没有安装**Ninja**, 则需要先从网站：[https://github.com/ninja-build/ninja/releases](https://github.com/ninja-build/ninja/releases) 下载安装包，并将可执行程序所在的目录配置到系统的环境变量中!

```ps
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/env/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -G "Ninja"
```

- 编译为windows桌面程序

```ps
cmake ..
cmake --build .
```

- 编译为Webassembly程序

```ps
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/env/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -G "Ninja"
ninja
```

## 3. 运行

- 进入到“./examples/web”目录，然后安装有依赖项：

```ps
cd ./examples/web
pnpm i
```

- 执行如下命令，然后在电脑中打开地址：[http://localhost:3070/](http://localhost:3070/)

```ps
npm start
```

## 多线程渲染相关问题

【ChatGPT 3.5】
- **问题**：在主线程中创建了swapchain，渲染子线程也是用了这个swapchain，当主线程窗口大小更新时，需要重新创建swapchain，子线程中如何处理这个swapchain？
- **答案**：
在这种情况下，主线程窗口大小更新后，主线程需要重新创建 swapchain。在重新创建 swapchain 之后，子线程也需要相应地更新以使用新的 swapchain。这可以通过一些同步机制和约定来实现，以下是一种可能的解决方案：

- 1. 使用标志或事件通知: 主线程在重新创建 swapchain 后，设置一个标志或触发一个事件，告知子线程 swapchain 已经更新。
- 2. 等待子线程完成当前渲染: 在主线程更新 swapchain 之后，主线程等待子线程完成当前的渲染任务，然后再更新 swapchain。
- 3. 同步帧更新: 在主线程更新 swapchain 之后，等待子线程完成当前帧的渲染后，再更新 swapchain。这样可以确保子线程在更新之前使用旧的 swapchain 完成当前帧的渲染。

下面是一个简单的示例代码，演示了如何在主线程更新 swapchain 后通知子线程更新：

```C++
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool swapchainUpdated = false;

// Function for rendering thread
void renderingThread() {
    while (true) {
        // Wait until swapchain is updated
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{ return swapchainUpdated; });
        }
        
        // Use updated swapchain for rendering
        std::cout << "Rendering Thread: Swapchain Updated, Rendering with New Swapchain\n";

        // Reset flag
        swapchainUpdated = false;
    }
}

int main() {
    // Start rendering thread
    std::thread renderer(renderingThread);

    // Simulate main thread updating swapchain
    while (true) {
        // Update swapchain
        std::cout << "Main Thread: Updating Swapchain\n";

        // Simulate swapchain update process
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Notify rendering thread that swapchain is updated
        {
            std::lock_guard<std::mutex> lock(mtx);
            swapchainUpdated = true;
        }
        cv.notify_one();

        // Continue with other main thread tasks
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    renderer.join();

    return 0;
}
```

在这个示例中，主线程更新 swapchain 后，会设置 swapchainUpdated 标志，并使用条件变量 cv 通知渲染线程。渲染线程在等待到通知后，会使用新的 swapchain 进行渲染，并在完成后重置标志。

最新编译命令：emcc "C:\Users\xrui94\Desktop\test_offscreen_canvas\src\main.cpp" -o "C:\Users\xrui94\Desktop\test_offscreen_canvas\examples\index.js" -sASYNCIFY -sOFFSCREENCANVAS_SUPPORT -pthread -s PTHREAD_POOL_SIZE_STRICT=0 -sUSE_WEBGPU --bind -sEXPORT_ES6=1 -sMODULARIZE=1 -sEXPORT_NAME="iEngine"