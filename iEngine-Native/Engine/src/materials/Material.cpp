#include "../../include/iengine/materials/Material.h"

namespace iengine {
    Material::Material(const std::string& name, const std::string& shaderName)
        : name(name), shaderName(shaderName) {}
}