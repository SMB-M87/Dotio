//
// Created by Priory on 3/8/2024.
//

#ifndef ARROW_H
#define ARROW_H

#include "moving_object.h"

namespace logic {
    struct Arrow : MovingObject {
        explicit Arrow(gmath::Position position, gmath::Vector2 velocity);

        Arrow();

        void Update(float delta) override;

        void Draw(gamef::Renderer &renderer) override;
    };
}  // namespace logic

#endif  // ARROW_H
