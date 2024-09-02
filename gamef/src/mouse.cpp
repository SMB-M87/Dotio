#include "mouse.h"
#include "renderer.h"

namespace gamef {
    void Mouse::Update(const Renderer& renderer, const gmath::Coordinates& actualCoordinates) {
        _actualCoordinates = actualCoordinates;
        _worldSpaceCoordinates = _actualCoordinates / renderer.GetScreen().GetVirtualRatio() -
                                 renderer.GetScreen().GetVirtual().GetCenter() + renderer.Camera;
        _uiSpaceCoordinates = _actualCoordinates / renderer.GetHalfViewport() - gmath::Vec2(1.0f);
    }

    gmath::Coordinates Mouse::GetActualCoordinates() const { return _actualCoordinates; }
    gmath::Coordinates Mouse::GetWorldSpaceCoordinates() const { return _worldSpaceCoordinates; }
    gmath::Coordinates Mouse::GetUISpaceCoordinates() const { return _uiSpaceCoordinates; }
}  // namespace game
