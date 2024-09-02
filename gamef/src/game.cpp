#include <thread>
#include <renderer.h>

#include "game.h"
#include "window.h"

namespace gamef {
    bool Game::VirtualKey::IsDown(const Enum virtualKey) { return GetKeyState(virtualKey) & 0x8000; }

    inline bool Game::VirtualKey::IsUp(const Enum virtualKey) { return !IsDown(virtualKey); }

    bool Game::VirtualKey::IsToggledOn(const Enum virtualKey) { return GetKeyState(virtualKey) & 0x1; }

    inline bool Game::VirtualKey::IsToggledOff(const Enum virtualKey) { return !IsToggledOn(virtualKey); }

    Game::EventArgs::EventArgs(Game& game, const unsigned int wParam, const long long lParam) :
        _game(game), Window::EventArgs(wParam, lParam) {}

    Game& Game::EventArgs::GetGame() const { return _game; }

    Game::KeyEventArgs::KeyEventArgs(Game& game, const unsigned int wParam, const long long lParam) :
        EventArgs(game, wParam, lParam), KeyFlags(HIWORD(LParam)) {}

    Game::VirtualKey::Enum Game::KeyEventArgs::GetVirtualKeyCode() const {
        return static_cast<VirtualKey::Enum>(LOWORD(WParam));
    }

    unsigned int Game::KeyEventArgs::GetRepeatCount() const { return LOWORD(LParam); }

    Game::ScanCode::Enum Game::KeyEventArgs::GetScanCode() const {
        return static_cast<ScanCode::Enum>(LOBYTE(KeyFlags));
    }

    bool Game::KeyEventArgs::IsExtendedKey() const { return (KeyFlags & KF_EXTENDED) == KF_EXTENDED; }

    bool Game::KeyEventArgs::WasKeyDown() const { return (KeyFlags & KF_REPEAT) == KF_REPEAT; }

    bool Game::KeyEventArgs::IsKeyReleased() const { return (KeyFlags & KF_UP) == KF_UP; }

    Game::MouseMoveEventArgs::MouseMoveEventArgs(Game& game, unsigned int wParam, long long lParam) :
        EventArgs(game, wParam, lParam) {}

    Game::MouseClickEventArgs::MouseClickEventArgs(Game& game, unsigned int wParam, long long lParam) :
        MouseMoveEventArgs(game, wParam, lParam) {}

    gmath::Coordinates Game::MouseMoveEventArgs::GetActualCoordinates() const {
        return { static_cast<float>(LOWORD(LParam)), static_cast<float>(HIWORD(LParam)) };
    }

    gmath::Coordinates Game::MouseMoveEventArgs::GetWorldSpaceCoordinates() const {
        const Renderer& renderer{ GetGame().GetRenderer() };
        return GetActualCoordinates() - renderer.GetHalfViewport() + renderer.Camera;
    }

    gmath::Coordinates Game::MouseMoveEventArgs::GetUISpaceCoordinates() const {
        return GetActualCoordinates() / GetGame().GetRenderer().GetHalfViewport() - gmath::Vec2(1.0f);
    }

    Game::Game(HINSTANCE hInstance, const float tickRate, const float logicalFrameRate, const float virtualWidth) :
        _tickRate(tickRate), _logicalFps(logicalFrameRate), _minDelta(0.42f), _maxDelta(6.0f) {
        _updateRate = static_cast<float>(Window::GetRefreshRate());
        _window = std::make_unique<Window>(hInstance, virtualWidth, L"GameWindowCLass", L"My Game");

        _window->OnCreate = [this](Window& window) {
            OnCreate(*this);

            Renderer& renderer{ GetRenderer() };
            renderer.OnRenderWorld = [this](Renderer& r) { OnRenderWorld(*this, r); };
            renderer.OnRenderFloatingUI = [this](Renderer& r) { OnRenderFloatingUI(*this, r); };
            renderer.OnRenderUI = [this](Renderer& r) { OnRenderUI(*this, r); };
            renderer.OnRenderDebug = [this](Renderer& r) { OnRenderDebug(*this, r); };

            _keepUpdating = true;
            _keepTicking = true;
            _updateThread = std::thread([this] { Update(); });
            _tickThread = std::thread([this] { Tick(); });
        };

        _window->OnQuit = [this](Window& window) {
            _updateThread.join();
            _tickThread.join();
        };

        _window->OnKeyDown = [this](Window& window, const unsigned int wParam, const long long lParam) {
            this->OnKeyDown(KeyEventArgs(*this, wParam, lParam));
        };

        _window->OnKeyUp = [this](Window& window, const unsigned int wParam, const long long lParam) {
            this->OnKeyUp(KeyEventArgs(*this, wParam, lParam));
        };

        _window->OnMouseMove = [this](Window& window, const unsigned int wParam, const long long lParam) {
            this->OnMouseMove(MouseMoveEventArgs(*this, wParam, lParam));
        };

        _window->OnMouseLeftButtonDown = [this](Window& window, const unsigned int wParam, const long long lParam) {
            this->OnLeftButtonDown(MouseClickEventArgs(*this, wParam, lParam));
            _leftMouseButton = true;
        };

        _window->OnMouseLeftButtonUp = [this](Window& window, const unsigned int wParam, const long long lParam) {
            this->OnLeftButtonUp(MouseClickEventArgs(*this, wParam, lParam));
            _leftMouseButton = false;
        };
    }

    Renderer& Game::GetRenderer() const { return *_window->Renderer; }

    Window& Game::GetWindow() const { return *_window; }

    void Game::Start() {
        _updateStart = std::chrono::high_resolution_clock::now();
        _tickStart = _updateStart;

        if (_tickRate > 0.0f)
            _tickDeltaMax = static_cast<long long>(1000000.0f / _tickRate);

        if (_updateRate > 0.0f)
            _updateDeltaMax = static_cast<long long>(1000000.0f / _updateRate);

        _window->Show();
    }

    void Game::Update() {
        while (_keepUpdating) {
            auto now{ std::chrono::high_resolution_clock::now() };
            const auto updateDelta{ std::chrono::duration_cast<std::chrono::microseconds>(now - _updateStart).count() };
            _delta =
                static_cast<float>(updateDelta) * _logicalFps / (static_cast<float>(_updateDeltaMax) * _updateRate);

            if (_delta > _maxDelta)
                _delta = _maxDelta;

            if (_delta > _minDelta) {
                _mouse.Update(GetRenderer(), _mouse.GetActualCoordinates());
                OnUpdate(*this);

                if (_leftMouseButton)
                    OnLeftButtonHold(*this);

                if (updateDelta)
                    _ups = 1000000.0f / static_cast<float>(updateDelta);

                _updateStart = now;
            }
        }
    }

    void Game::Tick() {
        while (_keepTicking) {
            auto now{ std::chrono::high_resolution_clock::now() };

            if (const auto tickDelta = std::chrono::duration_cast<std::chrono::microseconds>(now - _tickStart).count();
                _tickRate == 0.0f || (tickDelta >= _tickDeltaMax)) {
                OnTick(*this);

                if (tickDelta)
                    _tps = 1000000.0f / static_cast<float>(tickDelta);

                _tickStart = now;
            }
        }
    };

    void Game::Quit() {
        _window->Quit();
        _keepUpdating = false;
        _keepTicking = false;
        PostQuitMessage(0);
    }

    float Game::GetDelta() const { return _delta; }
    float Game::GetUpdateRate() const { return _ups; }
    float Game::GetTickRate() const { return _tps; }
    float Game::GetFrameRate() const { return GetWindow().GetFrameRate(); }
    Mouse& Game::GetMouse() { return _mouse; }
}  // namespace game
