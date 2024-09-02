#include "window.h"

#include <iostream>
#include <thread>
#include <utility>

#include "direct2d_renderer.h"

namespace gamef {
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        LONG_PTR userData = GetWindowLongPtr(hwnd, GWLP_USERDATA);

        if (!userData) {
            // Note that some message will not trigger at Window::WndProc during the creation of the window.
            // Most notable messages are: WM_GETMINMAXINFO, WM_NCCREATE, WM_NCCALCSIZE, WM_CREATE, WM_WINDOWPOSCHANGING
            // WM_WINDOWPOSCHANGED, WM_MOVE, WM_SIZE, WM_GETICON.
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        auto *window = reinterpret_cast<Window *>(userData);

        return window->WndProc(hwnd, msg, wParam, lParam);
    }

    Window::EventArgs::EventArgs(const unsigned int wParam, const long long lParam) : WParam(wParam), LParam(lParam) {}

    Window::Window(HINSTANCE hInstance, const float virtualWidth, std::wstring className, std::wstring windowName) :
        _hInstance(hInstance), _virtualWidth(virtualWidth), _className(std::move(className)),
        _windowName(std::move(windowName)) {}

    void Window::Quit() { _keepRendering = false; }

    float Window::GetFrameRate() const { return _fps; }

    unsigned long Window::GetRefreshRate() {
        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm);
        return dm.dmDisplayFrequency;
    }

    void Window::Show() {
        const WNDCLASSEX wndClass = {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_OWNDC,
            .lpfnWndProc = gamef::WndProc,
            .cbClsExtra = NULL,
            .cbWndExtra = NULL,
            .hInstance = _hInstance,
            .hIcon = nullptr,
            .hCursor = LoadCursor(nullptr, IDC_ARROW),
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = _className.c_str(),
            .hIconSm = nullptr,
        };

        const ATOM atom = RegisterClassEx(&wndClass);

        if (!atom)
            handle_last_error("Registering window class");

        _hWnd = CreateWindowEx(0,
            MAKEINTATOM(atom),
            _windowName.c_str(),
            WS_POPUPWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            _hInstance,
            nullptr);

        // Set the window's custom user data to this instance,
        // so it can be used later in WndProcW
        if (SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)))
            handle_last_error("Setting the current window as the window's GWLP_USERDATA");

        ShowWindow(_hWnd, SW_MAXIMIZE);

        Renderer = std::make_unique<Direct2DRenderer>(_hWnd, _virtualWidth);

        if (!Renderer->Initialize()) {
            log_error("Could not initialize renderer.");
            return;
        }

        _keepRendering = true;
        std::thread renderThread([this] { RenderTask(); });

        MSG msg = {};

        OnCreate(*this);

        while (_keepRendering) {
            if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                // Respond to PostQuitMessage
                if (msg.message == WM_QUIT)
                    _keepRendering = false;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        renderThread.join();
        OnQuit(*this);
    }

    void Window::UpdateClientRect() {
        if (!GetClientRect(_hWnd, &_clientRect))
            handle_last_error("Getting the client rectangle of the window");
    }

    void Window::RenderTask() {
        std::chrono::time_point<std::chrono::steady_clock> start{};
        std::chrono::time_point<std::chrono::steady_clock> now{};
        long long timeDelta{};

        while (_keepRendering) {
            Renderer->Begin();
            start = std::chrono::high_resolution_clock::now();
            Draw();
            now = std::chrono::high_resolution_clock::now();
            timeDelta = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
            Renderer->End();
            _fps = 1000000.0f / static_cast<float>(timeDelta);
        }
    }

    void Window::Draw() const {
        Renderer->SetRenderMode(Renderer::Mode::Enum::World);
        Renderer->OnRenderWorld(*Renderer);

        Renderer->SetRenderMode(Renderer::Mode::Enum::FloatingUI);
        Renderer->OnRenderFloatingUI(*Renderer);

        Renderer->SetRenderMode(Renderer::Mode::Enum::UI);
        Renderer->OnRenderUI(*Renderer);

        Renderer->SetRenderMode(Renderer::Mode::Enum::Debug);
        if (Renderer->DebugEnabled())
            Renderer->OnRenderDebug(*Renderer);
    }

    void Window::WmCreateHandler() { UpdateClientRect(); }

    void Window::WmSizeHandler() { UpdateClientRect(); }

    LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        LRESULT result{ 0 };

        switch (msg) {
            case WM_DESTROY: {
                _keepRendering = false;
                PostQuitMessage(0);
                break;
            }
            case WM_SIZE:
                WmSizeHandler();
                break;
            case WM_MOUSEMOVE:
                OnMouseMove(*this, wParam, lParam);
                break;
            case WM_LBUTTONDOWN:
                OnMouseLeftButtonDown(*this, wParam, lParam);
                break;
            case WM_LBUTTONUP:
                OnMouseLeftButtonUp(*this, wParam, lParam);
                break;
            case WM_KEYDOWN:
                OnKeyDown(*this, wParam, lParam);
                break;
            case WM_KEYUP:
                OnKeyUp(*this, wParam, lParam);
                break;
            case WM_CLOSE:
                PostQuitMessage(0);
            default:
                result = DefWindowProc(hWnd, msg, wParam, lParam);
                break;
        }

        return result;
    }

    Window::~Window() = default;
}  // namespace gamef
