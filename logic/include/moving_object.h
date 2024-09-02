#ifndef MOVING_OBJECT_H
#define MOVING_OBJECT_H

#include <static_object.h>

namespace logic {
    class MovingObject : public StaticObject {
    public:
        gmath::Vector2 Velocity{};
        float AngularVelocity{};

        explicit MovingObject(gmath::Position position, float direction, bool active);

        MovingObject();

        virtual void Update(float delta);

        virtual void Tick();

        [[nodiscard]] gmath::Vector2 GetVelocity() const;
        void SetVelocity(gmath::Vec2 velocity);

        [[nodiscard]] gmath::Vector2 GetNextPosition() const;
    };
}  // namespace logic

#endif  // MOVING_OBJECT_H
