#ifndef RENDERER_H
#define RENDERER_H

namespace gamef {
    struct Screen final {
    private:
        float _virtualWidth;

        /**
         * \brief Ranges from viewport width for horizontal and viewport height for vertical.
         */
        gmath::Rectangle _actual{};

        /**
         * \brief Ranges from configuted virtual width for horizontal and viewport height for vertical.
         */
        gmath::Rectangle _virtual{};

        /**
         * \brief Ranges from -1.0f to 1.0f for both horizontal and vertical.
         */
        gmath::Rectangle _normalRange{ gmath::Size2{ -1.0f }, gmath::Size2{ 1.0f } };

        gmath::Size2 _actualPixel{ 1.0f };
        gmath::Size2 _virtualPixel{};  // y pixel is always 1.0f
        gmath::Size2 _normalPixel{};

        gmath::Size2 _actualRatio{};
        gmath::Size2 _virtualRatio{};
        gmath::Size2 _normalRatio{ 1.0f };

    public:
        explicit Screen(float virtualWidth);

        void Update(const gmath::Size2& viewport);

        [[nodiscard]] gmath::Rectangle GetNormal()const;
        [[nodiscard]] gmath::Rectangle GetVirtual() const;
        [[nodiscard]] gmath::Rectangle GetActual()const;

        [[nodiscard]] gmath::Size2 GetNormalPixel() const;
        [[nodiscard]] gmath::Size2 GetVirtualPixel() const;
        [[nodiscard]] gmath::Size2 GetActualPixel() const;

        [[nodiscard]] gmath::Size2 GetNormalRatio() const;
        [[nodiscard]] gmath::Size2 GetVirtualRatio() const;
        [[nodiscard]] gmath::Size2 GetActualRatio() const;
    };

    class Renderer {
    public:
        struct Mode {
            enum Enum {
                World,
                FloatingUI,
                UI,
                Debug,
            };
        };

        gmath::Vec2 Camera{};
        float Zoom{1.0f};

        using RendererEvent = std::function<void(Renderer& renderer)>;

        using RendererUIEvent = std::function<void(Renderer& renderer, const Screen& screen)>;

        std::function<void(Renderer& renderer)> OnRenderWorld;

        std::function<void(Renderer& renderer)> OnRenderFloatingUI;

        std::function<void(Renderer& renderer)> OnRenderUI;

        std::function<void(Renderer& renderer)> OnRenderDebug;

        [[nodiscard]] Screen GetScreen() const;

        [[nodiscard]] Mode::Enum GetRenderMode() const;

        void SetRenderMode(Mode::Enum renderMode);

        [[nodiscard]] bool UpdateViewport();

        [[nodiscard]] gmath::Size2 GetViewport() const;

        [[nodiscard]] gmath::Size2 GetHalfViewport() const;

        [[nodiscard]] gmath::Size2 GetScreenSize();

        void ToggleDebug();

        bool DebugEnabled() const;

        explicit Renderer(HWND hWnd, float virtualWidth);

        virtual bool Initialize();

        virtual ~Renderer() = default;

        [[nodiscard]] gmath::Vec2 ConvertToWorldPoint(const gmath::Vec2& point) const;

        [[nodiscard]] gmath::Vec2 ConvertToFloatingUIPoint(const gmath::Vec2& point) const;

        [[nodiscard]] gmath::Vec2 ConvertToUIPoint(const gmath::Vec2& point) const;

        [[nodiscard]] gmath::Vec2 ConvertPoint(const gmath::Vec2& point) const;

        void ConvertPoints(const gmath::Vec2 from[], gmath::Vec2 to[], unsigned int count) const;

        virtual void Begin() = 0;

        virtual void End() = 0;

        [[nodiscard]] virtual gmath::RGBA GetPrimaryColor() const = 0;

        virtual void SetPrimaryColor(const gmath::RGBA& color) = 0;

        [[nodiscard]] virtual float GetPrimaryAlpha() const = 0;

        virtual void SetPrimaryAlpha(float alpha) = 0;

        virtual void Clear(const gmath::RGBA& color) const = 0;

        virtual void DrawSegment(const gmath::Segment& segment, const gmath::RGBA& color, float thickness) const = 0;

        virtual void DrawSegment(const gmath::Segment& segment, const gmath::RGBA& color) const = 0;

        virtual void DrawSegment(const gmath::Segment& segment, float thickness) const = 0;

        virtual void DrawSegment(const gmath::Segment& start) const = 0;

        virtual void DrawTriangle(const gmath::Point2 points[3], const gmath::RGBA& color) const = 0;

        virtual void DrawTriangle(const gmath::Point2 points[3]) const = 0;

        virtual void DrawRectangle(const gmath::Rectangle& rectangle, const gmath::RGBA& color) const = 0;

        virtual void DrawRectangle(const gmath::Rectangle& rectangle) const = 0;

        virtual void DrawCircle(const gmath::Position& position, float radius, const gmath::RGBA& color) const = 0;

        virtual void DrawCircle(const gmath::Position& position, float radius) const = 0;

        virtual void DrawString(const gmath::Rectangle& rectangle, const std::wstring& text, const gmath::RGBA& color) const = 0;

        virtual void DrawString(const gmath::Rectangle& rectangle, const std::wstring& text) const = 0;

    protected:
        HWND _hWnd;
        gmath::Size2 _viewport{};
        gmath::Size2 _halfViewport{};
        Screen _screen;
        bool _renderDebug{ false };
        Mode::Enum _renderMode{ Mode::Enum::World };
    };
}  // namespace game

#endif  // RENDERER_H
