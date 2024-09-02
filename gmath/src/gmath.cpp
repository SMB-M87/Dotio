#include "gmath.h"

#include <bitset>
#include <iostream>

namespace gmath {
    Vec2::Vec2(const float x, const float y) : X(x), Y(y) {}

    Vec2::Vec2(const float s) : Vec2(s, s) {}

    Vec2::Vec2() : Vec2(0.0f) {}

    void Vec2::Set(const float x, const float y) {
        X = x;
        Y = y;
    }
    void Vec2::Set(const Vec2&& vector) { Set(vector.X, vector.Y); }

    void Vec2::Set(const Vec2& vector) { Set(vector.X, vector.Y); }

    float Vec2::DotProduct(const Vec2& other) const { return X * other.X + Y * other.Y; }

    float Vec2::Determinant(const Vec2& other) const { return X * other.Y - Y * other.X; }

    float Vec2::SignedAngleDifference(const Vec2& other) const { return atan2f(Determinant(other), DotProduct(other)); }

    float Vec2::SignedAngleDifference(const Vec2& heading, const Vec2& other) const {
        return heading.SignedAngleDifference(other - *this);
    }

    float Vec2::Intercept(const float velocityA, const float velocityB, const float angleDifference) {
        float a = sinf(angleDifference) * velocityA / velocityB;

        if (a > 1)
            a = 1;

        else if (a < -1)
            a = -1;

        return asinf(a);
    }

    Vec2 Vec2::GetDifference(const Vec2& other) const { return (*this - other).GetAbs(); }

    Vec2 Vec2::GetAbs() const { return { std::abs(X), std::abs(Y) }; }

    Vec2 Vec2::operator-(const float n) const { return { X - n, Y - n }; }

    Vec2 Vec2::operator+(const float n) const { return { X + n, Y + n }; }

    Vec2 Vec2::operator+(const Vec2& other) const { return { X + other.X, Y + other.Y }; }

    Vec2 Vec2::operator-(const Vec2& other) const { return { X - other.X, Y - other.Y }; }

    bool Vec2::operator==(const Vec2& other) const { return X == other.X && Y == other.Y; }

    Vec2& Vec2::operator+=(float n) {
        X += n;
        Y += n;
        return *this;
    }

    Vec2& Vec2::operator+=(const Vec2& vec2) {
        X += vec2.X;
        Y += vec2.Y;
        return *this;
    }

    Vec2& Vec2::operator-=(float n) {
        X -= n;
        Y -= n;
        return *this;
    }

    Vec2& Vec2::operator-=(const Vec2& vec2) {
        X -= vec2.X;
        Y -= vec2.Y;
        return *this;
    }

    Vec2& Vec2::operator*=(float n) {
        X *= n;
        Y *= n;
        return *this;
    }

    bool Vec2::operator<(float value) const { return (GetLength() < value); }

    bool Vec2::operator>(float value) const { return (GetLength() > value); }

    bool Vec2::operator<=(float value) const { return (GetLength() <= value); }

    bool Vec2::operator>=(float value) const { return (GetLength() >= value); }

    Vec2 Vec2::operator*(const Vec2& other) const { return { X * other.X, Y * other.Y }; }

    Vec2 Vec2::operator/(const Vec2& other) const { return { X / other.X, Y / other.Y }; }

    Vec2 operator*(const float n, const Vec2& vec2) { return { vec2.X * n, vec2.Y * n }; }

    Vec2 operator*(const Vec2& vec2, const float n) { return n * vec2; }

    Vec2 operator/(const float n, const Vec2& vec2) { return { n / vec2.X, n / vec2.Y }; }

    Vec2 operator/(const Vec2& vec2, const float n) { return { vec2.X / n, vec2.Y / n }; }

    Vec2 Vec2::operator-() const { return { -X, -Y }; }

    Vec2& Vec2::Round() {
        X = roundf(X);
        Y = roundf(Y);
        return *this;
    }

    Vec2 Vec2::GetRounded() const { return { roundf(X), roundf(Y) }; }

    Vec2& Vec2::Ceil() {
        X = ceilf(X);
        Y = ceilf(Y);
        return *this;
    }
    Vec2 Vec2::GetCeiled() const { return { ceilf(X), ceilf(Y) }; }

    Vec2& Vec2::Floor() {
        X = floorf(X);
        Y = floorf(Y);
        return *this;
    }
    Vec2 Vec2::GetFloored() const { return { floorf(X), floorf(Y) }; }

    float Vec2::GetLength() const { return sqrtf(X * X + Y * Y); }

    float Vec2::GetDirection() const { return atan2f(Y, X); }

    Vec2& Vec2::Translate(const Vec2& vector) {
        Set(*this + vector);
        return *this;
    }

    Vec2 Vec2::GetSigned() const { return { fabsf(X), fabsf(Y) }; }

    Vec2 Vec2::GetModulo(const float divisor) const { return { fmodf(X, divisor), fmodf(Y, divisor) }; }

    Vec2& Vec2::Modulo(const float divisor) {
        X = fmodf(X, divisor);
        Y = fmodf(Y, divisor);
        return *this;
    }

    Vec2 Vec2::GetNormalized() const { return *this / GetLength(); }

    Vec2& Vec2::Normalize() {
        const float length = GetLength();
        if (length > 0.0f)
            Set(*this / length);
        return *this;
    }

    Vec2& Vec2::Rotate(const float radians) {
        float x = X;
        float y = Y;
        X = x * cos(radians) - y * sin(radians);
        Y = x * sin(radians) + y * cos(radians);
        return *this;
    }

    float Vec2::GetDistance(const Vec2& other) const { return GetDifference(other).GetLength(); }

    Segment::Segment() : From({}), To({}) {}
    Segment::Segment(Point2 from, Point2 to) : From(from), To(to) {}
    float Segment::GetSlope() const { return (End.Y - Start.Y) / (End.X - Start.X); }

    Rect::Rect(const Size2 start, const Size2 end) : Segment(start, end) { Update(); }
    Rect::Rect(float startX, float startY, float endX, float endY) : Rect({ startX, startY }, { endX, endY }) {}
    Rect::Rect() : Rect({}, {}) {}

    void Rect::Set(const Segment& segment) {
        From = segment.From;
        To = segment.To;
        Update();
    }
    inline void Rect::Set(const Point2& from, const Point2& to) { Set({ from, to }); }

    void Rect::Update() {
        _size = From.GetDifference(To);
        _halfSize = _size * 0.5f;
        _center = From + _halfSize;
        _topRight = { To.X, From.Y };
        _bottomLeft = { From.X, To.Y };
    }

    void Rect::SetFrom(const Point2& from) {
        From = from;
        Update();
    }
    void Rect::SetTo(const Point2& to) {
        To = to;
        Update();
    }

    float Rect::GetLeft() const { return From.X; }
    float Rect::GetRight() const { return To.X; }
    float Rect::GetTop() const { return From.Y; }
    float Rect::GetBottom() const { return To.Y; }

    Point2 Rect::GetTopLeft() const { return From; }
    Point2 Rect::GetTopRight() const { return _topRight; }
    Point2 Rect::GetBottomLeft() const { return _bottomLeft; }
    Point2 Rect::GetBottomRight() const { return To; }

    Size2 Rect::GetSize() const { return _size; }
    float Rect::GetWidth() const { return _size.Width; }
    float Rect::GetHeight() const { return _size.Height; }
    Size2 Rect::GetHalfSize() const { return _halfSize; }
    Point2 Rect::GetCenter() const { return _center; }

    Rect& Rect::operator+=(const Vec2& vec2) { this->Start += vec2; this->End += vec2; return *this; }

    float Angle::Get() const { return _radians; }
    void Angle::Set(const float radians) { _radians = radians; }
    void Angle::SetWrapped(const float radians) {
        Set(radians);
        Wrap();
    }
    float Angle::FromWrapped(const float radians) {
        return fmodf(std::numbers::pi_v<float> * 2.0f + fmodf(radians, 2.0f * std::numbers::pi_v<float>),
            2.0f * std::numbers::pi_v<float>);
    }
    void Angle::Wrap(float& radians) { radians = FromWrapped(radians); }
    inline void Angle::Wrap() { Wrap(_radians); }

    GridEntry::GridEntry() : GridEntry(0) {}
    GridEntry::GridEntry(const int row, const int column) : Row(row), Column(column) {}
    GridEntry::GridEntry(const int size) : GridEntry(size, size) {}
    Coordinates GridEntry::ToWorld(const int row, const int column, const float cellSize, const Coordinates& origin) {
        return { (static_cast<float>(column) * cellSize) + origin.X, (static_cast<float>(row) * cellSize) + origin.Y };
    }

    bool GridEntry::InBounds(const GridEntry& entry, const GridEntry& size) {
        return !(entry.Row >= size.Rows || entry.Column >= size.Columns || entry.Row < 0 || entry.Column < 0);
    }


    bool GridEntry::FromWorld(const Position& position, const Coordinates& origin, const float cellSize,
        const GridEntry& gridSize, GridEntry& output) {
        // clang-format off
        const GridEntry temp = {
            static_cast<int>(floorf((position.Y - origin.Y) / cellSize)),
            static_cast<int>(floorf((position.X - origin.X) / cellSize))
        };
        // clang-format on

        output = temp;

        return InBounds(temp, gridSize);
    }

    Position GridEntry::GetCellPosition(const Position& position, const Coordinates& origin, const float cellSize) {
        return Vec2{ position - origin }.Modulo(cellSize);
    }

    bool GridEntry::ActivatePath(const gmath::GridEntry& origin, const gmath::GridEntry& target) {
        int far = 6;
        int row = abs(origin.Row - target.Row);
        int column = abs(origin.Column - target.Column);
        return row < far && column < far;
    }

    bool GridEntry::PathRefresh(const gmath::GridEntry& origin, const gmath::GridEntry& target)
    {
        int near = 1;
        int row = abs(origin.Row - target.Row);
        int column = abs(origin.Column - target.Column);
        return row >= near || column >= near;
    }

    uint8_t GridEntry::GetSquarePartition(
        const Position& position, const float semiWidth, const Coordinates& origin, float cellSize) {
        const Position cellPosition = GetCellPosition(position, origin, cellSize);
        uint8_t result{};

        if (cellPosition.X > cellSize - semiWidth) {
            result |= 0b00000001;
        }

        else if (cellPosition.X < semiWidth) {
            result |= 0b00000011;
        }

        if (cellPosition.Y > cellSize - semiWidth) {
            result |= 0b0000100;

            if (cellPosition.X >= cellSize - semiWidth) {
                result |= 0b00010000;
            }

            else if (cellPosition.X < semiWidth) {
                result |= 0b00110000;
            }
        }

        else if (cellPosition.Y < semiWidth) {
            result |= 0b00001100;

            if (cellPosition.X > cellSize - semiWidth) {
                result |= 0b01110000;
            }

            else if (cellPosition.X < semiWidth) {
                result |= 0b01010000;
            }
        }

        return result;
    }
    void GridEntry::GetPartitions(uint8_t partition, const Position& position, const Coordinates& origin,
        const float cellSize, const GridEntry& gridSize, GridEntry output[4]) {
        GridEntry cell;

        if (!FromWorld(position, origin, cellSize, gridSize, cell))
            return;

        output[0] = cell;

        if (partition & 0b00000001) {
            if ((partition & 0b00000010) == 0b00000010) {
                if (const GridEntry t{ cell.Row, cell.Column - 1 }; InBounds(t, gridSize))
                    output[1] = t;
            }

            else {
                if (const GridEntry t{ cell.Row, cell.Column + 1 }; InBounds(t, gridSize))
                    output[1] = t;
            }
        }

        if (partition & 0b00000100) {
            if ((partition & 0b00001000) == 0b00001000) {
                if (const GridEntry t{ cell.Row - 1, cell.Column }; InBounds(t, gridSize))
                    output[2] = t;
            }

            else {
                if (const GridEntry t{ cell.Row + 1, cell.Column }; InBounds(t, gridSize))
                    output[2] = t;
            }
        }

        if (partition & 0b00010000) {
            if ((partition & 0b01100000) == 0b01100000) {
                if (const GridEntry t{ cell.Row - 1, cell.Column + 1 }; InBounds(t, gridSize)) {
                    output[3] = t;
                }
            }

            else if ((partition & 0b01000000) == 0b01000000) {
                if (const GridEntry t{ cell.Row - 1, cell.Column - 1 }; InBounds(t, gridSize))
                    output[3] = t;

            }

            else if ((partition & 0b00100000) == 0b00100000) {
                if (const GridEntry t{ cell.Row + 1, cell.Column - 1 }; InBounds(t, gridSize))
                    output[3] = t;

            }

            else {
                if (const GridEntry t{ cell.Row + 1, cell.Column + 1 }; InBounds(t, gridSize))
                    output[3] = t;
            }
        }
    }

    bool GridEntry::operator==(const GridEntry& other) const {
        return this->Row == other.Row && this->Column == other.Column;
    }

    bool GridEntry::operator!=(const GridEntry& other) const {
        return this->Row != other.Row && this->Column != other.Column;
    }

    GridEntry GridEntry::NullGridEntry = { -1, -1 };
}  // namespace gmath
