#include "moving_object.h"

namespace logic {
    MovingObject::MovingObject(const gmath::Position position, const float direction, bool active) :
        StaticObject(position, direction, active) {}

    MovingObject::MovingObject() : MovingObject({}, 0.0f, false) {}

    void MovingObject::Update(const float delta) {
        Position += Velocity * delta;
        Direction += AngularVelocity * delta;
    }

    void MovingObject::Tick() {}

    gmath::Vector2 MovingObject::GetVelocity() const { return Velocity; }
    void MovingObject::SetVelocity(const gmath::Vec2 velocity) { Velocity = velocity; }

    gmath::Vector2 MovingObject::GetNextPosition() const { return Position + Velocity; }
}  // namespace logic
