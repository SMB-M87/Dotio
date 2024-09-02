#ifndef OBJECT_H
#define OBJECT_H

#include "renderer.h"

namespace logic {
    class StaticObject {
    public:
        bool IsActive;
        gmath::Position Position{};
        float Direction{};

        explicit StaticObject(gmath::Position position, float direction, bool active);

        virtual ~StaticObject();

        virtual void Draw(gamef::Renderer &renderer);
    };
}  // namespace logic
#endif  // OBJECT_H
