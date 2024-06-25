cmake_minimum_required(VERSION 3.10...3.27 FATAL_ERROR)

project(iEngineCore VERSION 0.1.0 LANGUAGES CXX C)

include(utils.cmake)

# 设置编译选项
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自定义变量，设置三方库的路径
set(THIRD_PARTY_DIR C:/env/libc++)

# 添加 Emscripten 目标
if(EMSCRIPTEN)
    option(IE_ONLY_EMSCRIPTEN "Set up development helper settings" ON)
    option(IE_WGPU_EMSCRIPTEN "Set up development helper settings" OFF)

    # 添加头文件搜索目录
    INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}/src
        ${THIRD_PARTY_DIR}
        ${THIRD_PARTY_DIR}/glfw/include
        # ${CMAKE_SOURCE_DIR}/thirdParty/wgpu-native/webgpu-cpp/emscripten/include
        ${CMAKE_SOURCE_DIR}/dist/_deps/webgpu-backend-emscripten-src/include
    )

    # 添加源文件
    if(IE_WGPU_EMSCRIPTEN)
        message(STATUS "Using WebGPU impls provided by wgpu-native in an webassembly environment.")
        message(STATUS "Downloading deps: webgpu...")
        add_subdirectory(thirdParty/wgpu-native/webgpu)
    else()
        message(STATUS "Using WebGPU impls provided by emscripten in an webassembly environment.")
    endif(IE_WGPU_EMSCRIPTEN)

    # 删除不需要编译的源文件
    FILE(GLOB_RECURSE SRC_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
    )

    # 删除不需要编译的源文件
    list(FILTER SRC_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/example.*\.cpp) # 注意正则匹配中的转移字符
    list(FILTER SRC_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/renderers/webgpu/DepthTexture.cpp)
    list(FILTER SRC_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/windows/GlfwWindow.cpp)

    #
    add_executable(${PROJECT_NAME} ${SRC_FILES})

    #
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/examples/web/lib)
    set(CMAKE_EXECUTABLE_SUFFIX ".js") # Enable js output (no html).

    #
    if(IE_WGPU_EMSCRIPTEN)
        target_compile_definitions(${PROJECT_NAME} PRIVATE IE_WGPU_EMSCRIPTEN)
    else()
        # IE_ONLY_ESCRIPTEN == ON (default)
        target_compile_definitions(${PROJECT_NAME} PRIVATE IE_ONLY_EMSCRIPTEN)
    endif()

    #
    target_compile_definitions(${PROJECT_NAME} PRIVATE IE_HTML_WINDOW)

    # 报大量的警告信息，类型“em++: warning: linker setting ignored during compilation:......”的警告，这是因为将本应传递给链接器的设置错误地传递给了编译器
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -sPTHREAD_POOL_SIZE=1 -sALLOW_MEMORY_GROWTH=0 --bind")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sOFFSCREENCANVAS_SUPPORT=1 -sOFFSCREEN_FRAMEBUFFER=1")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sEXPORT_ES6=1 -sMODULARIZE=1 -sEXPORT_NAME=\"iEngineCore\"")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sGL_DEBUG=1")


    # 设置编译参数
    target_compile_options(${PROJECT_NAME} PRIVATE -pthread)    # -pthread在编译阶段和链接阶段都要使用，另外，注意：“-s USE_PTHREADS”已经弃用，应使用“-pthread”取代

    # 设置链接参数
    target_link_options(${PROJECT_NAME} PRIVATE -sENVIRONMENT=web,worker)
    target_link_options(${PROJECT_NAME} PRIVATE -pthread -sPTHREAD_POOL_SIZE_STRICT=0 -sALLOW_MEMORY_GROWTH=0 --bind)
    target_link_options(${PROJECT_NAME} PRIVATE -sMIN_WEBGL_VERSION=2 -sUSE_WEBGPU -sOFFSCREENCANVAS_SUPPORT=1 -sOFFSCREEN_FRAMEBUFFER=1)
    target_link_options(${PROJECT_NAME} PRIVATE -sASYNCIFY -sEXPORT_ES6=1 -sMODULARIZE=1 -sEXPORT_NAME="iEngineCore")
    target_link_options(${PROJECT_NAME} PRIVATE -sMIN_SAFARI_VERSION=-1) # 移除对Safari的支持，因为开启“离屏画布支持”后，如果调用时不适用离屏画布，会报错：canvas.getContextSafariWebGL2Fixed is not a function
    # target_link_options(${PROJECT_NAME} PRIVATE -Os)    # Release模式进行优化
    target_link_options(${PROJECT_NAME} PRIVATE -gsource-map -sASSERTIONS=2) # Debug模式使用，报错时，将会打印C++中真实的函数名，可读性较高，而不会是混淆的函数名
    # target_link_options(${PROJECT_NAME} PRIVATE -sGL_DEBUG=1)

else()
    option(DEBUG_MODE "Set up development helper settings" ON)
    option(IE_DAWN_NATIVE "Set up development helper settings" ON)
    option(IE_WGPU_NATIVE "Set up development helper settings" OFF)

    # set(WEBGPU_BACKEND "DAWN" CACHE STRING "Using dawn native backend for WebGPU." FORCE)
    # if(IE_WGPU_NATIVE)
    #     set(WEBGPU_BACKEND "WGPU" CACHE STRING "Using wgpu-native backend for WebGPU." FORCE)
    #     message(STATUS "Using WebGPU impls provided by wgpu-native.")
    # else()
    #     set(WEBGPU_BACKEND "DAWN" CACHE STRING "Using dawn native backend for WebGPU." FORCE)
    #     message(STATUS "Using WebGPU impls provided by dawn native.")
    # endif()

    if (MSVC)
	    #忽略保存为unicode警告
		add_compile_options(/wd4819)

		# 设置MSVC使用UTF-8编码
        add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")  # 处理“warning C4819: 该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失”警告
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>") # 处理“warning C4819: 该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失”警告
    endif()

    # 添加头文件搜索目录
    if(IE_WGPU_NATIVE)
        INCLUDE_DIRECTORIES(
            ${CMAKE_SOURCE_DIR}/src
            ${THIRD_PARTY_DIR}
            ${THIRD_PARTY_DIR}/glfw/include
        )

        message(STATUS "Downloading deps for wgpu-native: glfw, webgpu and glfw3webgpu ...")
        add_subdirectory(thirdParty/wgpu-native/glfw)
        add_subdirectory(thirdParty/wgpu-native/webgpu)
        add_subdirectory(thirdParty/wgpu-native/glfw3webgpu)
    else()
        INCLUDE_DIRECTORIES(
            ${CMAKE_SOURCE_DIR}/src
            ${THIRD_PARTY_DIR}
        )

        message(STATUS "Downloading deps for dawn: glfw, webgpu and webgpu_glfw ...")
        # 首先找到并包含子模块目录
        # find_package(Git QUIET)
        # if(GIT_FOUND)
        #     option(GIT_SUBMODULE "Check submodules during build" ON)
        #     message(STATUS "Submodule update, downloading deps for dawn: glfw, webgpu and webgpu_glfw ...")
        #     execute_process(
        #         COMMAND ${GIT_EXECUTABLE} submodule update --init
        #         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} RESULT_VARIABLE GIT_SUBMOD_RESULT
        #     )
        #     if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        #         message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        #     endif()
        # endif()
        
        # 然后包含子模块
        set(DAWN_FETCH_DEPENDENCIES ON)
        add_subdirectory(${CMAKE_SOURCE_DIR}/thirdParty/dawn EXCLUDE_FROM_ALL)
    endif()

    # 添加源文件
    # FILE(GLOB SRC_FILES
    #     ${CMAKE_SOURCE_DIR}/src/renderer/webgpu/ShaderModule.cpp
    #     ${CMAKE_SOURCE_DIR}/src/renderer/webgpu/WebGPUDevice.cpp
    #     ${CMAKE_SOURCE_DIR}/src/VertexAttrBuffer.cpp
    #     ${CMAKE_SOURCE_DIR}/src/Mesh.cpp
    #     ${CMAKE_SOURCE_DIR}/src/Scene.cpp
    #     ${CMAKE_SOURCE_DIR}/src/Camera.cpp
    #     ${CMAKE_SOURCE_DIR}/src/PerspectiveCamera.cpp
    #     ${CMAKE_SOURCE_DIR}/src/Renderer.cpp
    #     ${CMAKE_SOURCE_DIR}/src/RenderThread.cpp
    #     ${CMAKE_SOURCE_DIR}/src/Engine.cpp
    #     ${CMAKE_SOURCE_DIR}/src/Window.cpp
    #     ${CMAKE_SOURCE_DIR}/src/GlfwWindow.cpp
    #     ${CMAKE_SOURCE_DIR}/src/Impls.cpp
    #     ${CMAKE_SOURCE_DIR}/src/main.cpp
    # )

    # 删除不需要编译的源文件
     FILE(GLOB_RECURSE SRC_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
    )

    # 删除不需要编译的源文件
    # 通过LIST的REMOVE_ITEM方法，并不管用，似乎要这么用：https://stackoverflow.com/questions/36134129/listremove-item-not-working-in-cmake
    # list(REMOVE_ITEM ${SRC_FILES} 
    #     ${CMAKE_SOURCE_DIR}/src/example_1.cpp
    #     ${CMAKE_SOURCE_DIR}/src/example_1_copy.cpp
    #     ${CMAKE_SOURCE_DIR}/src/example_2.cpp
    #     ${CMAKE_SOURCE_DIR}/src/example_3.cpp
    #     ${CMAKE_SOURCE_DIR}/src/renderers/webgpu/DepthTexture.cpp
    #     ${CMAKE_SOURCE_DIR}/src/windows/HtmlWindow.cpp
    #     ${CMAKE_SOURCE_DIR}/src/renderers/webgpu/OpenGLRenderer.cpp
    #     ${CMAKE_SOURCE_DIR}/src/renderers/webgpu/WebGPURenderer.cpp
    # )

    list(FILTER SRC_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/example.*\.cpp) # 注意正则表达式的转义字符
    list(FILTER SRC_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/renderers/webgpu/DepthTexture.cpp)
    list(FILTER SRC_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/windows/HtmlWindow.cpp)

    add_executable(${PROJECT_NAME} ${SRC_FILES})

    #
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/examples/native/lib)
    set(CMAKE_EXECUTABLE_SUFFIX ".exe")

    #
    if(IE_WGPU_NATIVE)
        target_compile_definitions(${PROJECT_NAME} PRIVATE IE_WGPU_NATIVE)
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE IE_DAWN_NATIVE)
    endif(IE_WGPU_NATIVE)

    target_compile_definitions(${PROJECT_NAME} PRIVATE IE_GLFW_WINDOW)

    #
    if(DEBUG_MODE)
        # In dev mode, we load resources from the source tree, so that when we
        # dynamically edit resources (like shaders), these are correctly
        # versionned.
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
        )
    else()
        # In release mode, we just load resources relatively to wherever the
        # executable is launched from, so that the binary is portable
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            RESOURCE_DIR="./resources"
        )
    endif()

    if(MSVC)
        # 自定义编译规则
        set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:MSVCRT")

        target_compile_options(${PROJECT_NAME} PRIVATE /wd4201)	# Disable warning C4201: nonstandard extension used: nameless struct/union
    endif()

    #
    target_treat_all_warnings_as_errors(${PROJECT_NAME})

    # find_library(LIB_GLFW glfw3 ${THIRD_PARTY_DIR}/glfw/lib-vc2022)
    # target_link_libraries(${PROJECT_NAME} ${LIB_GLFW})

    #
    if(IE_WGPU_NATIVE)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 17
            # VS_DEBUGGER_ENVIRONMENT "DAWN_DEBUG_BREAK_ON_ERROR=1"
            VS_DEBUGGER_ENVIRONMENT "RUST_BACKTRACE=full"
        )

        target_include_directories(${PROJECT_NAME} PRIVATE ./thirdParty)
        target_link_libraries(${PROJECT_NAME} glfw webgpu glfw3webgpu)

        #
        target_copy_webgpu_binaries(${PROJECT_NAME})
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE webgpu_cpp webgpu_dawn glfw webgpu_glfw)
    endif(IE_WGPU_NATIVE)
endif()