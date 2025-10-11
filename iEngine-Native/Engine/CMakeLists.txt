# 收集所有源文件
set(SRC_FILES "")

# 递归收集 src 目录下的所有源文件
file(GLOB_RECURSE CORE_FILES "src/core/*.cpp" "src/core/*.h" "src/core/*.hpp")
file(GLOB_RECURSE GEOMETRIES_FILES "src/geometries/*.cpp" "src/geometries/*.h" "src/geometries/*.hpp")
file(GLOB_RECURSE LIGHTS_FILES "src/lights/*.cpp" "src/lights/*.h" "src/lights/*.hpp")
file(GLOB_RECURSE MATERIALS_FILES "src/materials/*.cpp" "src/materials/*.h" "src/materials/*.hpp")
file(GLOB_RECURSE MATH_FILES "src/math/*.cpp" "src/math/*.h" "src/math/*.hpp")
file(GLOB_RECURSE RENDERERS_FILES "src/renderers/*.cpp" "src/renderers/*.h" "src/renderers/*.hpp")
file(GLOB_RECURSE SCENES_FILES "src/scenes/*.cpp" "src/scenes/*.h" "src/scenes/*.hpp")
file(GLOB_RECURSE SHADERS_FILES "src/shaders/*.cpp" "src/shaders/*.h" "src/shaders/*.hpp")
file(GLOB_RECURSE TEXTURES_FILES "src/textures/*.cpp" "src/textures/*.h" "src/textures/*.hpp")
file(GLOB_RECURSE VIEWS_FILES "src/views/*.cpp" "src/views/*.h" "src/views/*.hpp")

# 合并所有源文件
list(APPEND SRC_FILES 
    ${CORE_FILES}
    ${GEOMETRIES_FILES}
    ${LIGHTS_FILES}
    ${MATERIALS_FILES}
    ${MATH_FILES}
    ${RENDERERS_FILES}
    ${SCENES_FILES}
    ${SHADERS_FILES}
    ${TEXTURES_FILES}
    ${VIEWS_FILES}
)

# 创建静态库
add_library(iengine STATIC ${SRC_FILES})

# 设置库的属性
set_target_properties(iengine PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# glm
target_link_libraries(iengine PRIVATE glm::glm-header-only)

# mapbox/earcut.hpp
target_include_directories(iengine PRIVATE ${EARCUT_HPP_INCLUDE_DIRS})

# glad
target_link_libraries(iengine PRIVATE glad::glad)

# spdlog - 每个用到spdlog的库都需要再次链接spdlog库
target_link_libraries(iengine PRIVATE spdlog::spdlog)

# 设置包含目录
target_include_directories(iengine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 设置编译选项
target_compile_options(iengine PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

# 安装规则
install(TARGETS iengine
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY include/ DESTINATION include)