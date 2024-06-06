#pragma once

#include <glm/mat4x4.hpp>

#include <array>

/**
 * The same structure as in the shader, replicated in C++
 */
struct BaseUniforms {
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    std::array<float, 4> color;
    float time;
    float _pad[3];
};

// 函数接受结构体类型和字段名称作为参数，并返回字段的偏移量
template<typename T, typename = std::enable_if_t<std::is_class_v<T>>>
size_t getFieldOffset(const char* fieldName) {
    return offsetof(T, fieldName);
}

template<typename T, typename = std::enable_if_t<std::is_class_v<T>>>
size_t getFieldSize(const char* fieldName) {
    size_t nextFieldOffset;
    if (fieldName[0] == '\0') {
        // 如果字段名称为空，则返回整个结构体的大小
        nextFieldOffset = sizeof(T);
    } else {
        // 否则，计算下一个字段的偏移量
        nextFieldOffset = getFieldOffset<T>(fieldName) + getFieldSize<T>(fieldName + 1);
    }
    // 返回字段所占的字节数
    return nextFieldOffset - getFieldOffset<T>(fieldName);
}

template<typename T, typename Field = void>
size_t getFieldSize(Field T::* field = nullptr) {
    if(T().*Field == nullptr) return sizeofStruct<T>();
    else {
        return sizeof(T().*field);
    }
}

// 函数接受结构体类型作为参数
template<typename T, typename = std::enable_if_t<std::is_class_v<T>>>
void processStruct(const T& obj) {
    // 在这里处理结构体对象
    std::cout << "Processing struct..." << std::endl;
}