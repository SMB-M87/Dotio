#ifndef DIRECT2D_RENDERER_H
#define DIRECT2D_RENDERER_H

#include <d2d1_1.h>
#include <dwrite.h>
#include <wrl/client.h>

#include "renderer.h"

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite")

namespace gamef {
    class Direct2DRenderer final : public Renderer {
        ComPtr<ID2D1Factory> _factory;
        ComPtr<IDWriteFactory> _writeFactory;
        ComPtr<ID2D1HwndRenderTarget> _renderTarget;

        ComPtr<ID2D1SolidColorBrush> _primaryBrush;
        ComPtr<IDWriteTextFormat> _primaryTextFormat;

        [[nodiscard]] static D2D1_COLOR_F D2D1ColorFromRGBA(const gmath::RGBA &color);
        [[nodiscard]] static gmath::RGBA D2D1ColorToRGBA(const D2D1_COLOR_F &color);

        [[nodiscard]] static D2D1_POINT_2F D2D1PointFromVec2(const gmath::Point2 &point);
        [[nodiscard]] static gmath::Vec2 D2D1PointToVec2(const D2D1_POINT_2F &point);

        static void D2D1PointsFromVec2(const gmath::Point2 from[], D2D1_POINT_2F to[], unsigned int count);
        static void D2D1PointsToVec2(const D2D1_POINT_2F from[], gmath::Point2 to[], unsigned int count);

    public:
        explicit Direct2DRenderer(HWND hWnd, float virtualWidth);

        bool Initialize() override;

        void Begin() override;

        void End() override;

        [[nodiscard]] gmath::RGBA GetPrimaryColor() const override;

        void SetPrimaryColor(const gmath::RGBA &color) override;

        [[nodiscard]] float GetPrimaryAlpha() const override;

        void SetPrimaryAlpha(float alpha) override;

        void Clear(const gmath::RGBA &color) const override;

        void DrawSegment(const gmath::Segment &segment, const gmath::RGBA &color, float thickness) const override;
        void DrawSegment(const gmath::Segment &segment, const gmath::RGBA &color) const override;
        void DrawSegment(const gmath::Segment &segment, float thickness) const override;
        void DrawSegment(const gmath::Segment &segment) const override;

        void DrawTriangle(const gmath::Point2 points[3], const gmath::RGBA &color) const override;
        void DrawTriangle(const gmath::Point2 points[3]) const override;

        void DrawRectangle(const gmath::Rectangle &rectangle, const gmath::RGBA &color) const override;
        void DrawRectangle(const gmath::Rectangle &rectangle) const override;

        void DrawCircle(const gmath::Position& position, float radius, const gmath::RGBA& color) const override;
        void DrawCircle(const gmath::Position& position, float radius) const override;

        void DrawString(const gmath::Rectangle &rectangle, const std::wstring &text, const gmath::RGBA &color) const override;
        void DrawString(const gmath::Rectangle &rectangle, const std::wstring &text) const override;

        ~Direct2DRenderer() override;
    };
}  // namespace game

#endif  // DIRECT2D_RENDERER_H
