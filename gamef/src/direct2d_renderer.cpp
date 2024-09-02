#include "direct2d_renderer.h"

#include <gtest/gtest.h>

#include "renderer.h"

namespace gamef {
    Direct2DRenderer::Direct2DRenderer(HWND hWnd, const float virtualWidth) : Renderer(hWnd, virtualWidth) {}

    bool Direct2DRenderer::Initialize() {
        Renderer::Initialize();

        HRESULT result{};

        // Initialize COM
        result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

        if (FAILED(result)) {
            log_error(std::string("Failed to initialize COM: ").append(std::to_string(result)));
            return false;
        }

        // Create Direct2D facotry
        result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, _factory.GetAddressOf());

        if (FAILED(result)) {
            log_error(std::string("Failed to create D2D1 factory: ").append(std::to_string(result)));
            return false;
        }

        // Create Direct2D write factory
        result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(_writeFactory.GetAddressOf()));

        if (FAILED(result)) {
            log_error(std::string("Failed to create D2D1 factory: ").append(std::to_string(result)));
            return false;
        }

        if (SUCCEEDED(result)) {
            // Create primary text format
            result = _writeFactory->CreateTextFormat(L"",
                nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                12,
                L"",
                _primaryTextFormat.GetAddressOf());

            if (FAILED(result)) {
                log_error(std::string("Failed to create D2D1 text format: ").append(std::to_string(result)));
                return false;
            }

            if (SUCCEEDED(result)) {
                _primaryTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                _primaryTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
        }

        // Create Direct2D render target
        gmath::Size2 viewport = GetViewport();

        result = _factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                _hWnd, D2D1::SizeU(static_cast<UINT32>(viewport.Width), static_cast<UINT32>(viewport.Height))),
            _renderTarget.GetAddressOf());

        if (FAILED(result)) {
            log_error(std::string("Failed to create D2D1 render target: ").append(std::to_string(result)));
            return false;
        }

        if (SUCCEEDED(result)) {
            // Create primary brush
            result =
                _renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), _primaryBrush.GetAddressOf());

            if (FAILED(result)) {
                log_error(std::string("Failed to create D2D1 brush: ").append(std::to_string(result)));
                return false;
            }
        }

        return true;
    }

    void Direct2DRenderer::Begin() {
        if (!_renderTarget)
            return;

        _renderTarget->BeginDraw();
    }

    void Direct2DRenderer::End() {
        if (!_renderTarget)
            return;

        if (const HRESULT result = _renderTarget->EndDraw(); FAILED(result))
            log_error(std::string("Failed to create D2D1 render target: ").append(std::to_string(result)));
    }

    gmath::RGBA Direct2DRenderer::GetPrimaryColor() const { return D2D1ColorToRGBA(_primaryBrush->GetColor()); }

    void Direct2DRenderer::SetPrimaryColor(const gmath::RGBA& color) {
        _primaryBrush->SetColor(D2D1ColorFromRGBA(color));
    }

    float Direct2DRenderer::GetPrimaryAlpha() const { return GetPrimaryColor().Alpha; }

    void Direct2DRenderer::SetPrimaryAlpha(float alpha) {
        gmath::RGBA color{ GetPrimaryColor() };
        color.Alpha = alpha;
        SetPrimaryColor(color);
    }

    Direct2DRenderer::~Direct2DRenderer() { CoUninitialize(); };

    void Direct2DRenderer::Clear(const gmath::RGBA& color) const {
        if (!_renderTarget)
            return;

        _renderTarget->Clear(D2D1ColorFromRGBA(color));
    }

    void Direct2DRenderer::DrawSegment(
        const gmath::Segment& segment, const gmath::RGBA& color, const float thickness) const {
        if (!_renderTarget)
            return;

        const gmath::Vec2 start{ ConvertPoint(segment.From) };
        const gmath::Vec2 end{ ConvertPoint(segment.To) };
        const D2D1_COLOR_F tempColor{ _primaryBrush->GetColor() };

        _primaryBrush->SetColor(D2D1ColorFromRGBA(color));
        _renderTarget->DrawLine(D2D1PointFromVec2(start), D2D1PointFromVec2(end), _primaryBrush.Get(), thickness);
        _primaryBrush->SetColor(tempColor);
    }

    void Direct2DRenderer::DrawSegment(const gmath::Segment& segment, const gmath::RGBA& color) const {
        DrawSegment(segment, color, 1.0f);
    }

    void Direct2DRenderer::DrawSegment(const gmath::Segment& segment, float thickness) const {
        DrawSegment(segment, D2D1ColorToRGBA(_primaryBrush->GetColor()), thickness);
    }

    void Direct2DRenderer::DrawSegment(const gmath::Segment& segment) const {
        DrawSegment(segment, D2D1ColorToRGBA(_primaryBrush->GetColor()), 1.0f);
    }

    void Direct2DRenderer::DrawTriangle(const gmath::Point2 points[3], const gmath::RGBA& color) const {
        if (!_renderTarget)
            return;

        ID2D1PathGeometry* geometry;

        HRESULT result{ _factory->CreatePathGeometry(&geometry) };

        if (FAILED(result)) {
            log_error(std::string("Failed to create D2D1 path geometry: ").append(std::to_string(result)));
            return;
        }

        ComPtr<ID2D1GeometrySink> sink;
        result = geometry->Open(sink.GetAddressOf());

        if (FAILED(result)) {
            log_error(std::string("Failed to open D2D1 geometry sink: ").append(std::to_string(result)));
            return;
        }

        sink->BeginFigure(D2D1PointFromVec2(ConvertPoint(points[0])), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1PointFromVec2(ConvertPoint(points[1])));
        sink->AddLine(D2D1PointFromVec2(ConvertPoint(points[2])));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);

        result = sink->Close();

        if (FAILED(result)) {
            log_error(std::string("Failed to close D2D1 geometry sink: ").append(std::to_string(result)));
            return;
        }

        const D2D1_COLOR_F tempColor{ _primaryBrush->GetColor() };
        _renderTarget->FillGeometry(geometry, _primaryBrush.Get());
        _primaryBrush->SetColor(tempColor);
    }

    void Direct2DRenderer::DrawTriangle(const gmath::Point2 points[3]) const {
        DrawTriangle(points, D2D1ColorToRGBA(_primaryBrush->GetColor()));
    }

    void Direct2DRenderer::DrawRectangle(const gmath::Rectangle& rectangle, const gmath::RGBA& color) const {
        if (!_renderTarget)
            return;

        const D2D1_POINT_2F start = { D2D1PointFromVec2(ConvertPoint(rectangle.GetTopLeft())) };
        const D2D1_POINT_2F end = { D2D1PointFromVec2(ConvertPoint(rectangle.GetBottomRight())) };
        const D2D1_RECT_F rect{ start.x, start.y, end.x, end.y };

        const D2D1_COLOR_F tempColor{ _primaryBrush->GetColor() };
        _primaryBrush->SetColor(D2D1ColorFromRGBA(color));
        _renderTarget->FillRectangle(&rect, _primaryBrush.Get());
        _primaryBrush->SetColor(tempColor);
    }

    void Direct2DRenderer::DrawRectangle(const gmath::Rectangle& rectangle) const {
        DrawRectangle(rectangle, D2D1ColorToRGBA(_primaryBrush->GetColor()));
    }

    void Direct2DRenderer::DrawCircle(
        const gmath::Position& position, const float radius, const gmath::RGBA& color) const {
        if (!_renderTarget)
            return;

        float ratio = GetScreen().GetVirtualRatio().Width;
        const D2D1_POINT_2F point = { D2D1PointFromVec2(ConvertPoint(position)) };
        const D2D1_ELLIPSE ellipse{ point, radius * Zoom * ratio, radius * Zoom * ratio };

        const D2D1_COLOR_F tempColor{ _primaryBrush->GetColor() };
        _primaryBrush->SetColor(D2D1ColorFromRGBA(color));
        _renderTarget->FillEllipse(&ellipse, _primaryBrush.Get());
        _primaryBrush->SetColor(tempColor);
    }

    void Direct2DRenderer::DrawCircle(const gmath::Position& position, const float radius) const {
        DrawCircle(position, radius, D2D1ColorToRGBA(_primaryBrush->GetColor()));
    }

    void Direct2DRenderer::DrawString(
        const gmath::Rectangle& rectangle, const std::wstring& text, const gmath::RGBA& color) const {
        if (!_renderTarget)
            return;

        const D2D1_POINT_2F start = { D2D1PointFromVec2(ConvertPoint(rectangle.GetTopLeft())) };
        const D2D1_POINT_2F end = { D2D1PointFromVec2(ConvertPoint(rectangle.GetBottomRight())) };
        const D2D1_RECT_F rect{ start.x, start.y, end.x, end.y };

        const D2D1_COLOR_F tempColor{ _primaryBrush->GetColor() };
        _primaryBrush->SetColor(D2D1ColorFromRGBA(color));
        _renderTarget->DrawTextW(text.c_str(), text.length(), _primaryTextFormat.Get(), &rect, _primaryBrush.Get());
        _primaryBrush->SetColor(tempColor);
    }

    void Direct2DRenderer::DrawString(const gmath::Rectangle& rectangle, const std::wstring& text) const {
        DrawString(rectangle, text, D2D1ColorToRGBA(_primaryBrush->GetColor()));
    }

    D2D1_COLOR_F Direct2DRenderer::D2D1ColorFromRGBA(const gmath::RGBA& color) {
        return D2D1_COLOR_F(color.R, color.G, color.B, color.A);
    }

    gmath::RGBA Direct2DRenderer::D2D1ColorToRGBA(const D2D1_COLOR_F& color) {
        return { color.r, color.g, color.b, color.a };
    }

    D2D1_POINT_2F Direct2DRenderer::D2D1PointFromVec2(const gmath::Point2& point) {
        return D2D1_POINT_2F(point.X, point.Y);
    }

    gmath::Vec2 Direct2DRenderer::D2D1PointToVec2(const D2D1_POINT_2F& point) { return { point.x, point.y }; }

    void Direct2DRenderer::D2D1PointsFromVec2(
        const gmath::Point2 from[], D2D1_POINT_2F to[], const unsigned int count) {
        for (unsigned int i = 0; i < count; i++) to[i] = D2D1PointFromVec2(from[i]);
    }

    void Direct2DRenderer::D2D1PointsToVec2(const D2D1_POINT_2F from[], gmath::Point2 to[], const unsigned int count) {
        for (unsigned int i = 0; i < count; i++) to[i] = D2D1PointToVec2(from[i]);
    }
}  // namespace gamef
