# Project Description

 - [English](README.md)
 - [简体中文](README.zh.md)

 This is a real-time rendering engine, which uses the WASM ( emscripten tool chain ), based on webgl, webgpu, emscripten, OffscreenCanvas(or calle it Offscreen-Canvas) technology, through C++ code, you can deploy the engine to the desktop, mobile and HTML5 web.

- **Known issues:**
  - Based on the WebGPU implementation provided by Emscripten, it can be compiled, but it runs abnormally; 
  - Based on the WebGPU implementation provided by WGPU-Native, it cannot be compiled on emsdk-v3.1.60; it can be compiled on emsdk-v3.1.47, but it is also an exception.

 ## 1. Compile and Build

### 1.1 For Native

- **Using Dawn backend**

```
git submodule update --init
mkdir build
cd build
cmake ..
cmake --build .
```

- **Using WGPU-Native backend**

```ps
mkdir build
cd build
cmake ..
cmake --build .
```

### 1.3 For Web

- First, make a build directory

```ps
mkdir dist
cd dist
```

- Second, Generate the Ninja project file

**Replace** the value of the parameter **"- DCMAKE-TOOLCHAIN-FILE"** with the absolute path of your Emscripten.cmake file.

- **Note**:
  - If **emscripten tool chain**(or call it **"emsdk"**) is not installed, you need to first download and install it according to this instructions: [https://emscripten.org/docs/getting_started/downloads.html](https://emscripten.org/docs/getting_started/downloads.html);
  - If there is no **Ninja** in your system env, you need to first download a from [https://github.com/ninja-build/ninja/releases](https://github.com/ninja-build/ninja/releases) and configure system environment variables for it!
  - The default configuration parameter **"-DIE_ONLY_EMSCRIPTEN=ON"** means using WebGPU implementation provided by emscripten(emsdk). If you want to use the WebGPU implementation provided by wgpu-native, you need to manually specify specified **"-DIE_WGPU_EMSCRIPTEN=ON"** configuration parameters. However with the 3.1.60 version of emsdk, it doesn 't work yet, you can track this [#issue16](https://github.com/eliemichel/WebGPU-Cpp/issues/16) for checking more details.

```ps
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/env/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -G "Ninja"
```

- Last, Build

```ps
ninja
```

## 2. Run

If you want to use the native executable application, it is located in the **"examples/native/lib/Debug”** directory. Double click it to run. If you want to use the web application, you can install the following steps:

- First, Install dependencies

```ps
pmpm i
```

- Second, use the app

After executing the command bellow, you can use the app by opening the url [http://localhost:3070/](http://localhost:3070/) in browser.

```ps
npm start
```