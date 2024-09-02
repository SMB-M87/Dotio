#include "renderer.h"

namespace gamef {
    Screen::Screen(const float virtualWidth) : _virtualWidth(virtualWidth) {}

    void Screen::Update(const gmath::Size2& viewport) {
        _actual.SetTo(viewport);
        _virtual.SetTo({ _virtualWidth, (_actual.GetHeight() / _actual.GetWidth()) * _virtualWidth });

        _virtualPixel = { _actual.GetWidth() / _virtual.GetWidth(), 1.0f };
        _normalPixel = 1.0 / _actual.GetHalfSize();

        _actualRatio = { _actual.GetWidth() / _actual.GetHeight(), _actual.GetHeight() / _actual.GetWidth() };
        _virtualRatio = { _actual.GetWidth() / _virtual.GetWidth(), _actual.GetHeight() / _virtual.GetHeight() };
    }
    gmath::Rectangle Screen::GetNormal() const { return _normalRange; }
    gmath::Rectangle Screen::GetVirtual() const { return _virtual; }
    gmath::Rectangle Screen::GetActual() const { return _actual; }

    gmath::Size2 Screen::GetNormalPixel() const { return _normalPixel; }
    gmath::Size2 Screen::GetVirtualPixel() const { return _virtualPixel; }
    gmath::Size2 Screen::GetActualPixel() const { return _virtualPixel; }

    gmath::Size2 Screen::GetNormalRatio() const { return _normalRatio; }
    gmath::Size2 Screen::GetVirtualRatio() const { return _virtualRatio; }
    gmath::Size2 Screen::GetActualRatio() const { return _virtualRatio; }

    Renderer::Renderer(HWND hWnd, const float virtualWidth) : _hWnd(hWnd), _screen(virtualWidth) {}

    bool Renderer::Initialize() {
        if (!UpdateViewport()) {
            log_error("Could not update the viewport");
            return false;
        }

        return true;
    }

    gmath::Vec2 Renderer::ConvertToWorldPoint(const gmath::Vec2& point) const {
        return (point - Camera + _screen.GetVirtual().GetHalfSize() / Zoom) * _screen.GetVirtualRatio() * Zoom;
    }

    gmath::Vec2 Renderer::ConvertToFloatingUIPoint(const gmath::Vec2& point) const { return ConvertToWorldPoint(point); }

    gmath::Vec2 Renderer::ConvertToUIPoint(const gmath::Vec2& point) const { return point * _halfViewport + _halfViewport; }

    gmath::Vec2 Renderer::ConvertPoint(const gmath::Vec2& point) const {
        switch (_renderMode) {
            case Mode::Enum::World:
                return ConvertToWorldPoint(point);

            case Mode::Enum::FloatingUI:
                return ConvertToFloatingUIPoint(point);

            case Mode::Enum::UI:
            case Mode::Enum::Debug:
                return ConvertToUIPoint(point);

            default:
                log_error("Invalid render mode");
                return point;
        }
    }

    void Renderer::ConvertPoints(const gmath::Vec2 from[], gmath::Vec2 to[], const unsigned int count) const {
        for (unsigned int i = 0; i < count; i++) to[i] = ConvertPoint(from[i]);
    }

    Screen Renderer::GetScreen() const { return _screen; }

    Renderer::Mode::Enum Renderer::GetRenderMode() const { return _renderMode; }

    void Renderer::SetRenderMode(const Mode::Enum renderMode) { _renderMode = renderMode; }

    bool Renderer::UpdateViewport() {
        const gmath::Size2 screenSize = GetScreenSize();

        if (screenSize.Width == 0.0L || screenSize.Height == 0.0L) {
            log_error("Could not retreive the screen size");
            return false;
        }

        _viewport = screenSize;
        _halfViewport = _viewport * 0.5f;
        _screen.Update(_viewport);

        return true;
    }

    gmath::Size2 Renderer::GetViewport() const { return _viewport; }

    gmath::Size2 Renderer::GetHalfViewport() const { return _halfViewport; }

    gmath::Size2 Renderer::GetScreenSize() {
        int xScreen{}, yScreen{};
        gmath::Size2 primaryMonitorSize{};

        xScreen = GetSystemMetrics(SM_CXSCREEN);
        yScreen = GetSystemMetrics(SM_CYSCREEN);

        if (!xScreen) {
            handle_last_error("Could not get the primary screen width from the system metrics");
            return primaryMonitorSize;
        }

        if (!yScreen) {
            handle_last_error("Could not get the primary screen width from the system metrics");
            return primaryMonitorSize;
        }

        primaryMonitorSize.Width = static_cast<float>(xScreen);
        primaryMonitorSize.Height = static_cast<float>(yScreen);

        return primaryMonitorSize;
    }
    void Renderer::ToggleDebug() { _renderDebug ^= true; }

    bool Renderer::DebugEnabled() const { return _renderDebug; }
}  // namespace game
