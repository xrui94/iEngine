#pragma once


#include "ShaderProgram.h"

#include <vector>
#include <memory>

enum ShaderType : uint8_t
{
    POINT,
    LINE,
    FACE
};

struct ShaderCode
{
    std::string vertCode;
    std::string fragCode;
};

struct Shader
{
    ShaderCode code;
    std::shared_ptr<ShaderProgram> program;

    Shader() : code(), program(nullptr) {}
};

struct ShaderLib
{
    std::vector<Shader> pointShader;
    std::vector<Shader> lineShader;
    std::vector<Shader> faceShader;

    ShaderLib() : pointShader(), lineShader(), faceShader() {}
};