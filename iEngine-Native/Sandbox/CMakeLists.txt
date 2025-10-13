# ===========================
# Sandbox CMakeLists.txt
# 包含多个示例和测试程序
# ===========================

# 设置通用的包含目录
set(SANDBOX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Engine/include)

# ===========================
# 1. 引擎核心测试程序 (EngineCoreTest)
# ===========================

# 引擎核心测试源文件
set(CORE_TEST_SOURCES
    src/EngineCoreTest.cpp
)

# 创建引擎核心测试可执行文件
add_executable(EngineCoreTest ${CORE_TEST_SOURCES})

# 链接 iengine 库
target_link_libraries(EngineCoreTest 
    iengine
)

# 设置包含目录
target_include_directories(EngineCoreTest PRIVATE 
    ${SANDBOX_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}
)

# ===========================
# 2. GLFW 应用程序示例 (GlfwAppExample)
# ===========================

# 查找GLFW、OpenGL和GLAD
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)

# GLFW应用程序示例源文件
set(GLFW_EXAMPLE_SOURCES
    src/GlfwAppExample.cpp
    src/GLFWWindow.cpp
)

# GLFW应用程序示例头文件
set(GLFW_EXAMPLE_HEADERS
    src/GLFWWindow.h
)

# 创建GLFW应用程序示例可执行文件
add_executable(GlfwAppExample ${GLFW_EXAMPLE_SOURCES} ${GLFW_EXAMPLE_HEADERS})

# 链接 iengine 库、GLFW、OpenGL和GLAD
target_link_libraries(GlfwAppExample 
    iengine
    glfw
    OpenGL::GL
    glad::glad
)

# 设置包含目录
target_include_directories(GlfwAppExample PRIVATE 
    ${SANDBOX_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}
)

# ===========================
# 3. Qt 应用程序 (QtAppExample)
# ===========================

# 启用 Qt 自动处理 MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找 Qt6 组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

# Qt应用程序源文件
set(QT_APP_SOURCES
    src/QtAppExample.cpp
    src/QtWindow.cpp
    src/MainWindow.cpp
)

# Qt应用程序头文件（需要MOC处理）
set(QT_APP_HEADERS
    src/QtWindow.h
    src/MainWindow.h
)

# 创建Qt可执行文件
add_executable(QtAppExample ${QT_APP_SOURCES} ${QT_APP_HEADERS})

# 链接 iengine 库、Qt 组件和GLAD
target_link_libraries(QtAppExample 
    iengine
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGLWidgets
    glad::glad
    glfw  # QtAppExample也需要GLFW，因为iengine中的OpenGLContext使用了GLFW
)

# 设置包含目录
target_include_directories(QtAppExample PRIVATE 
    ${SANDBOX_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}
)

# ===========================
# 通用设置
# ===========================

# 为所有目标设置编译选项
foreach(target EngineCoreTest QtAppExample GlfwAppExample)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
    )
    

endforeach()

# ===========================
# Windows Qt 部署和资源拷贝
# ===========================

    # 为每个可执行程序拷贝资源文件
    foreach(target EngineCoreTest QtAppExample GlfwAppExample)
        # 设置编译器的警告级别，启用高级别的警告（Level 4），帮助发现潜在的代码问题
        target_compile_options(${target} PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
        )

        # 设置输出目录
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )

        # 设置 VS 的工作目录为整个项目的根构建目录，如：build/local-windows
        # 但是这里操作具体的项目的构建目录更合理，如：build/local-windows/bin/Debug
        # if(MSVC)
        #     set_target_properties(${target} PROPERTIES
        #         VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        #     )
        # endif()

        # -----------------------------
        # POST_BUILD: 拷贝到构建目录(VS中启动的时候，工作目录就是这个构建目录)
        # -----------------------------
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/../assets"
                # "${CMAKE_BINARY_DIR}/assets"  # 整个项目的根构建目录
                "${CMAKE_CURRENT_BINARY_DIR}/assets"    # 当前正在处理的项目的构建目录
            COMMENT "Copying assets to build directory..."
        )

        # -----------------------------
        # POST_BUILD: 拷贝到 exe 所在的目录
        # -----------------------------
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/../assets"
                "$<TARGET_FILE_DIR:${target}>/assets"
            COMMENT "Copying assets to ${target} exe directory..."
        )
    endforeach()

if(WIN32 AND MSVC)

    if(NOT DEFINED Qt6_DIR)
        message(FATAL_ERROR "Qt6_DIR is not set. Please set CMAKE_PREFIX_PATH or Qt6_DIR")
    endif()

    # 查找 windeployqt 工具
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt)

    # 使用"windeployqt"程序检测并拷贝需要的QT动态库到可执行程序所在的目录
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET QtAppExample POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                $<TARGET_FILE:QtAppExample>       # 可执行文件的完整路径
                --no-compiler-runtime               # 不拷贝编译器运行时
                --no-system-d3d-compiler            # 不拷贝系统 D3D 编译器
                --no-translations                   # 禁用多语言支持
            COMMENT "Running windeployqt to copy Qt libraries..."
        )
    endif()
endif()

# ===========================
# 添加自定义目标用于构建所有示例
# ===========================
add_custom_target(all_examples
    DEPENDS EngineCoreTest QtAppExample GlfwAppExample
    COMMENT "Building all sandbox examples"
)