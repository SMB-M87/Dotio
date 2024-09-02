#ifndef MOUSE_H
#define MOUSE_H

namespace gamef {
    class Mouse {
        gmath::Coordinates _actualCoordinates{};
        gmath::Coordinates _worldSpaceCoordinates{};
        gmath::Coordinates _uiSpaceCoordinates{};

    public:
        void Update(const Renderer& renderer, const gmath::Coordinates& actualCoordinates);

        [[nodiscard]] gmath::Coordinates GetActualCoordinates() const;
        [[nodiscard]] gmath::Coordinates GetWorldSpaceCoordinates() const;
        [[nodiscard]] gmath::Coordinates GetUISpaceCoordinates() const;
    };
}

#endif //MOUSE_H
