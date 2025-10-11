# 启用 Qt 自动处理 MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找 Qt6 组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

# 收集所有源文件
file(GLOB_RECURSE SANDBOX_SRC_FILES "src/*.cpp")

# 👇 关键：显式列出需要 MOC 的头文件（相对于 CMakeLists.txt 的路径）
set(SANDBOX_MOC_HEADERS
    include/sandbox/MainWindow.h
    include/sandbox/MainOpenGLWidget.h
)

# 创建可执行文件
add_executable(iengine_sandbox ${SANDBOX_SRC_FILES} ${SANDBOX_MOC_HEADERS})

# 链接 iengine 库和 Qt 组件
target_link_libraries(iengine_sandbox 
    iengine
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGLWidgets
)

# 设置包含目录
target_include_directories(iengine_sandbox PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Engine/include
)

# 设置编译选项
target_compile_options(iengine_sandbox PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

# 设置可执行文件的属性
set_target_properties(iengine_sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# -----------------------------
# Windows: 自动复制 assets & 部署 Qt DLL
# -----------------------------
if(WIN32 AND MSVC)
    if(NOT DEFINED Qt6_DIR)
        message(FATAL_ERROR "Qt6_DIR is not set. Please set CMAKE_PREFIX_PATH or Qt6_DIR")
    endif()

    # # -----------------------------
    # # POST_BUILD: 拷贝到构建目录(VS中启动的时候，工作目录就是这个构建目录)
    # # -----------------------------
    # add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory
    #         "${CMAKE_SOURCE_DIR}/assets"
    #         "${CMAKE_BINARY_DIR}/assets"
    #     COMMENT "Copying assets to build directory..."
    # )

    # # -----------------------------
    # # POST_BUILD: 拷贝到 exe 所在的目录
    # # -----------------------------
    # add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory
    #         "${CMAKE_SOURCE_DIR}/assets"
    #         "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    #     COMMENT "Copying assets to exe directory..."
    # )

    # 查找 windeployqt 工具
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt)

    # 使用“windeployqt”程序检测并拷贝需要的QT动态库到可执行程序所在的目录
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET iengine_sandbox POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                # $<TARGET_FILE_DIR:iengine_sandbox>  # 可执行文件的目录（如果是1个目录，windeployqt会扫描该目录下的所有可执行文件，并尝试为它们部署 Qt 依赖项）
                $<TARGET_FILE:iengine_sandbox>       # 可执行文件的完整路径
                --no-compiler-runtime               # 不拷贝编译器运行时
                --no-system-d3d-compiler            # 不拷贝系统 D3D 编译器
                --no-translations                   # 禁用多语言支持
            COMMENT "Running windeployqt to copy Qt libraries..."
        )
    endif()
endif()