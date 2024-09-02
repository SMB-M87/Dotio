#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <memory>

#include "mouse.h"
#include "window.h"

namespace gamef {
    class Game {
        std::unique_ptr<Window> _window;
        float _logicalFps;
        float _delta{};
        float _minDelta;
        float _maxDelta;

        float _ups{};
        float _updateRate;
        std::chrono::time_point<std::chrono::steady_clock> _updateStart{};
        long long _updateDeltaMax{};
        bool _keepUpdating{ false };
        std::thread _updateThread;

        float _tps{};
        float _tickRate;
        std::chrono::time_point<std::chrono::steady_clock> _tickStart{};
        long long _tickDeltaMax{};
        bool _keepTicking{ false };
        std::thread _tickThread;
        Mouse _mouse;

        bool _leftMouseButton{false};

    public:
        struct VirtualKey {
            enum Enum : short {
                Escape = VK_ESCAPE,
                Tilde = VK_OEM_3,
                Shift = VK_SHIFT,
                ArrowUp = VK_UP,
                ArrowDown = VK_DOWN,
                ArrowLeft = VK_LEFT,
                ArrowRight = VK_RIGHT,
                Space = ' ',
                One = '1',
                Two = '2',
                Three = '3',
                Four = '4',
                Five = '5',
                Six = '6',
                Q = 'Q',
                W = 'W',
                E = 'E',
                S = 'S',
                D = 'D',
                A = 'A',
                R = 'R',
                F = 'F',
                G = 'G',
                U = 'U',
                I = 'I',
                O = 'O',
                P = 'P',
                H = 'H',
                J = 'J',
                K = 'K',
                L = 'L',
                V = 'V',
            };

            static bool IsDown(Enum virtualKey);
            inline static bool IsUp(Enum virtualKey);
            static bool IsToggledOn(Enum virtualKey);
            inline static bool IsToggledOff(Enum virtualKey);
        };

        struct ScanCode {
            enum Enum : unsigned char {
                Esc = 0x01,
                Tilde = 0x29,
                LShift = 0x2A,
                W = 0x11,
                S = 0x1F,
                A = 0x1E,
                D = 0x20,
            };
        };

        struct EventArgs : Window::EventArgs {
        protected:
            Game& _game;

        public:
            explicit EventArgs(Game& game, unsigned int wParam, long long lParam);

            [[nodiscard]] Game& GetGame() const;
        };

        struct KeyEventArgs : EventArgs {
            unsigned short KeyFlags;

            KeyEventArgs(Game& game, unsigned int wParam, long long lParam);
            [[nodiscard]] VirtualKey::Enum GetVirtualKeyCode() const;
            [[nodiscard]] unsigned int GetRepeatCount() const;
            [[nodiscard]] ScanCode::Enum GetScanCode() const;
            [[nodiscard]] bool IsExtendedKey() const;
            [[nodiscard]] bool WasKeyDown() const;
            [[nodiscard]] bool IsKeyReleased() const;
        };

        struct MouseMoveEventArgs : EventArgs {
            MouseMoveEventArgs(Game& game, unsigned int wParam, long long lParam);
            [[nodiscard]] gmath::Coordinates GetActualCoordinates() const;
            [[nodiscard]] gmath::Coordinates GetWorldSpaceCoordinates() const;
            [[nodiscard]] gmath::Coordinates GetUISpaceCoordinates() const;
        };

        struct MouseClickEventArgs : MouseMoveEventArgs {
            MouseClickEventArgs(Game& game, unsigned int wParam, long long lParam);
        };

        static HINSTANCE hInstance;

        using GameEvent = std::function<void(Game& game)>;

        using RenderEvent = std::function<void(Game& game, Renderer& renderer)>;

        using WindowKeyEvent = std::function<void(KeyEventArgs args)>;

        using WindowMouseMoveEvent = std::function<void(MouseMoveEventArgs args)>;

        using WindowMouseClickEvent = std::function<void(MouseClickEventArgs args)>;

        GameEvent OnCreate;

        GameEvent OnUpdate;

        GameEvent OnTick;

        RenderEvent OnRenderWorld;

        RenderEvent OnRenderFloatingUI;

        RenderEvent OnRenderUI;

        RenderEvent OnRenderDebug;

        WindowKeyEvent OnKeyDown;

        WindowKeyEvent OnKeyUp;

        WindowMouseMoveEvent OnMouseMove;

        WindowMouseClickEvent OnLeftButtonDown;

        WindowMouseClickEvent OnLeftButtonUp;

        GameEvent OnLeftButtonHold;

        explicit Game(HINSTANCE hInstance, float tickRate, float logicalFrameRate, float virtualWidth);

        [[nodiscard]] Renderer& GetRenderer() const;

        [[nodiscard]] Window& GetWindow() const;

        void Start();

        void Update();

        void Tick();

        void Quit();

        [[nodiscard]] float GetDelta() const;

        [[nodiscard]] float GetUpdateRate() const;

        [[nodiscard]] float GetTickRate() const;

        [[nodiscard]] float GetFrameRate() const;

        [[nodiscard]] Mouse& GetMouse();
    };
}  // namespace gamef

#endif  // GAME_H
