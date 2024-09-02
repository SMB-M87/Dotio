#include "physics_object.h"

namespace logic {
    PhysicsObject::~PhysicsObject() = default;

    PhysicsObject::PhysicsObject() {}

    PhysicsObject::PhysicsObject(const float mass, const float radius, const float friction,
        const float rotationalFriction, const gmath::Position position, const float direction, bool active) :
        MovingObject(position, direction, active), _mass(mass), Radius(radius), _friction(friction),
        _angularFriction(rotationalFriction) {}

    void PhysicsObject::ApplyForce(const gmath::Vector2& force, const float mass) { Velocity += force; }

    void PhysicsObject::ApplyAngularForce(const float force, const float mass) {
        AngularVelocity += (_mass - mass) * force / (_mass + mass);
    }

    void PhysicsObject::Update(const float delta) {
        gmath::Vector2 force{ _friction * delta * -Velocity };

        ApplyForce(force, GetMass());

        MovingObject::Update(delta);
    }

    void PhysicsObject::Tick() { MovingObject::Tick(); }

    float PhysicsObject::GetDiameter() const { return 2.0f * Radius; }

    float PhysicsObject::GetFriction() const { return _friction; }
    float PhysicsObject::GetMass() const { return _mass; }
}  // namespace logic
