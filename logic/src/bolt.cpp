//
// Created by Priory on 3/8/2024.
//

#include "bolt.h"

namespace logic {
    Bolt::Bolt(gmath::Position position, gmath::Vector2 velocity) : MovingObject(position, 0, false) { Velocity = velocity; }

    Bolt::Bolt() : Bolt({}, {}) {}

    void Bolt::Update(float delta) { MovingObject::Update(delta); }

    void Bolt::Draw(gamef::Renderer& renderer) {
        renderer.DrawCircle({ Position }, 5.0f, { 1.0f, 0.3f, 0.0f, 1.0f });
    }
}  // namespace logic
