#include "static_object.h"

namespace logic {
    StaticObject::~StaticObject() = default;
    StaticObject::StaticObject(const gmath::Position position, const float direction, bool active) :
        Position(position), Direction(direction), IsActive(active) {}

    void StaticObject::Draw(gamef::Renderer& renderer) {}
}  // namespace logic
