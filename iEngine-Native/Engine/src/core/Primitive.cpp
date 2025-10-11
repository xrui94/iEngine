#include "../../include/iengine/core/Primitive.h"

namespace iengine {
    Primitive::Primitive(PrimitiveType type, std::shared_ptr<VertexLayout> layout)
        : type(type), vertexLayout(layout) {}
}