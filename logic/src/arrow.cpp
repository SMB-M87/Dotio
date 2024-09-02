//
// Created by Priory on 3/8/2024.
//

#include "arrow.h"

namespace logic {
    Arrow::Arrow(gmath::Position position, gmath::Vector2 velocity) : MovingObject(position, 0, false) { Velocity = velocity; }

    Arrow::Arrow() : Arrow({}, {}) {}

    void Arrow::Update(float delta) { MovingObject::Update(delta); }

    void Arrow::Draw(gamef::Renderer& renderer) {
        renderer.DrawSegment({ Position, Position - (Velocity).GetNormalized() * 30.0f }, { 1.0f, 0.8f, 0.8f, 1.0f });
    }
}  // namespace logic
