#pragma once

#include <cstdint>
#include "glad/glad.h"
#include <glm/glm.hpp>

#include <string>

namespace n_GIECore
{
    struct ShaderCom
    {
        uint32_t VertShader;
        uint32_t FragShader;
        uint32_t GeomShader;
    };

    class ShaderProgram
    {
    public:
        // constructor generates the shader program on the fly
        ShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

        ShaderProgram(const std::string &vertexCode, const std::string &fragmentCode, const std::string &geometryCode = "");

        static std::string ShaderProgram::ReadCode(const std::string url);

        // activate the shader program
        void Use();

        uint32_t GetProgram();

        // utility uniform functions
        void SetBool(const std::string &name, bool value) const;

        void SetInt(const std::string &name, int value) const;

        void SetFloat(const std::string &name, float value) const;

        void SetVec2(const std::string &name, const glm::vec2 &value) const;

        void SetVec2(const std::string &name, float x, float y) const;

        void SetVec3(const std::string &name, const glm::vec3 &value) const;

        void SetVec3(const std::string &name, float x, float y, float z) const;

        void SetVec4(const std::string &name, const glm::vec4 &value) const;

        void SetVec4(const std::string &name, float x, float y, float z, float w);

        void SetMat2(const std::string &name, const glm::mat2 &mat) const;

        void SetMat3(const std::string &name, const glm::mat3 &mat) const;

        void SetMat4(const std::string &name, const glm::mat4 &mat) const;

    private:
        // utility function for checking shader program compilation/linking errors.
        void CheckCompileErrors(GLuint shaderProgram, std::string type);

    private:
        uint32_t mProgram;
    };

}
