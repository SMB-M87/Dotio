#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include <gmath.h>

#include "moving_object.h"

namespace logic {
    class PhysicsObject : public MovingObject {
    protected:
        float _mass;
        float _friction;
        float _angularFriction;

    public:
        float Radius;
        gmath::GridEntry LastPartition[4]{ { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } };

        PhysicsObject();

        explicit PhysicsObject(float mass, float radius, float friction, float rotationalFriction,
            gmath::Position position, float direction, bool active);

        virtual ~PhysicsObject();

        void ApplyForce(const gmath::Vector2& force, float mass);

        void ApplyAngularForce(float force, float mass);

        void Update(float delta) override;

        void Tick() override;

        float GetDiameter() const;

        [[nodiscard]] float GetFriction() const;

        [[nodiscard]] float GetMass() const;
    };
}  // namespace logic

#endif  // PHYSICS_OBJECT_H
