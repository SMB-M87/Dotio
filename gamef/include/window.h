#ifndef WINDOW_H
#define WINDOW_H

#include <functional>
#include <memory>
#include <string>

#include "pch.h"

namespace gamef {
    class Window {
        HINSTANCE _hInstance;
        HWND _hWnd{};
        RECT _clientRect{};
        std::wstring _className{};
        std::wstring _windowName{};
        float _virtualWidth;

        bool _keepRendering{ false };
        float _fps{};

        void RenderTask();

        void Draw() const;

#pragma region Window message handlers

        void WmCreateHandler();

        void WmSizeHandler();

#pragma endregion

    public:
        struct EventArgs {
            unsigned int WParam;
            unsigned long long LParam;

            explicit EventArgs(unsigned int wParam, long long lParam);
        };

        std::unique_ptr<Renderer> Renderer;

        explicit Window(HINSTANCE hInstance, float virtualWidth, std::wstring className, std::wstring windowName);

        static unsigned long GetRefreshRate();

        void UpdateClientRect();

        void Show();

        void Quit();

        float GetFrameRate() const;

        LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        ~Window();

#pragma region Events

        using WindowEvent = std::function<void(Window &window)>;

        using ParameterlessWindowMessageEvent = std::function<void(Window &window)>;

        using WordWindowMessageEvent = std::function<void(Window &window, unsigned int wParam)>;

        using LongWindowMessageEvent = std::function<void(Window &window, long long lParam)>;

        using WindowMessageEvent = std::function<void(Window &window, unsigned int wParam, long long lParam)>;

        WindowEvent OnCreate;

        WindowEvent OnQuit;

        WindowMessageEvent OnKeyDown;

        WindowMessageEvent OnKeyUp;

        WindowMessageEvent OnMouseMove;

        WindowMessageEvent OnMouseLeftButtonDown;

        WindowMessageEvent OnMouseLeftButtonUp;

#pragma endregion
    };
}  // namespace game

#endif  // WINDOW_H
