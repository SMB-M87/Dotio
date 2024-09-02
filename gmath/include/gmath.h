#ifndef GMATH_H
#define GMATH_H

namespace gmath {
    typedef struct Angle {
    private:
        float _radians{};

    public:
        [[nodiscard]] float Get() const;
        void Set(float radians);
        void SetWrapped(float radians);
        [[nodiscard]] static float FromWrapped(float radians);
        static void Wrap(float &radians);
        inline void Wrap();
    } Dir, Direction;

    typedef struct Vec1 {
        union {
            float X;
            float Length;
            float Distance;
            float Magnitude;
        };
    };

    typedef struct Vec2 {
        union {
            float X;
            float Width;
            float U;

            float Left;
            float Top;
            float Start;
        };

        union {
            float Y;
            float Height;
            float V;

            float Right;
            float Bottom;
            float End;
        };

        Vec2(float x, float y);

        explicit Vec2(float s);

        Vec2();

        void Set(float x, float y);
        void Set(const Vec2 &&vector);
        void Set(const Vec2 &vector);

        [[nodiscard]] float DotProduct(const Vec2 &other) const;

        [[nodiscard]] float Determinant(const Vec2 &other) const;

        [[nodiscard]] float SignedAngleDifference(const Vec2 &other) const;

        [[nodiscard]] float SignedAngleDifference(const Vec2 &heading, const Vec2 &other) const;

        static float Intercept(float velocityA, float velocityB, float angleDifference);

        [[nodiscard]] Vec2 GetDifference(const Vec2 &other) const;

        [[nodiscard]] float GetDistance(const Vec2 &other) const;

        [[nodiscard]] Vec2 GetAbs() const;

        Vec2 operator-(float n) const;

        Vec2 operator+(float n) const;

        Vec2 operator+(const Vec2 &other) const;

        Vec2 operator-(const Vec2 &other) const;

        bool operator==(const Vec2 &other) const;

        Vec2 &operator+=(float n);

        Vec2 &operator+=(const Vec2 &vec2);

        Vec2 &operator-=(float n);

        Vec2 &operator-=(const Vec2 &vec2);

        Vec2 &operator*=(float n);

        Vec2 operator*(const Vec2 &other) const;

        Vec2 operator/(const Vec2 &other) const;

        bool operator<(float value) const;
        bool operator>(float value) const;
        bool operator<=(float value) const;
        bool operator>=(float value) const;

        friend Vec2 operator*(float n, const Vec2 &vec2);

        friend Vec2 operator*(const Vec2 &vec2, float n);

        friend Vec2 operator/(float n, const Vec2 &vec2);

        friend Vec2 operator/(const Vec2 &vec2, float n);


        Vec2 operator-() const;

        Vec2 &Round();
        [[nodiscard]] Vec2 GetRounded() const;

        Vec2 &Ceil();
        [[nodiscard]] Vec2 GetCeiled() const;

        Vec2 &Floor();
        [[nodiscard]] Vec2 GetFloored() const;

        [[nodiscard]] float GetLength() const;

        [[nodiscard]] float GetDirection() const;

        Vec2 &Translate(const Vec2 &vector);

        [[nodiscard]] Vec2 GetSigned() const;

        [[nodiscard]] Vec2 GetModulo(float divisor) const;
        Vec2 &Modulo(float divisor);
        [[nodiscard]] Vec2 GetNormalized() const;
        Vec2 &Normalize();

        Vec2 &Rotate(float radians);
    } Vector2, Size2, Point2, Coord, Coordinates, Pos, Position;

    typedef struct Vec3 {
        union {
            float X;
            float R;
            float Red;
            float Width;
        };

        union {
            float Y;
            float G;
            float Green;
            float Height;
        };

        union {
            float Z;
            float B;
            float Blue;
            float Depth;
        };
    } Vector3, RGB;

    typedef struct Vec4 {
        union {
            float X;
            float R;
            float Red;
        };

        union {
            float Y;
            float G;
            float Green;
        };

        union {
            float Z;
            float B;
            float Blue;
        };

        union {
            float W;
            float A;
            float Alpha;
        };
    } Vector4, RGBA;

    struct Segment {
        union {
            Point2 From;
            Point2 Start;
        };

        union {
            Point2 To;
            Point2 End;
        };

        Segment();
        Segment(Point2 from, Point2 to);

        float GetSlope() const;
    };

    typedef struct Rect : protected Segment {
        virtual ~Rect() = default;

        Rect(Point2 start, Point2 end);
        Rect(float startX, float startY, float endX, float endY);
        Rect();

        void Set(const Segment &segment);
        inline void Set(const Point2 &from, const Point2 &to);
        void SetFrom(const Point2 &from);
        void SetTo(const Point2 &to);

        [[nodiscard]] float GetLeft() const;
        [[nodiscard]] float GetRight() const;
        [[nodiscard]] float GetTop() const;
        [[nodiscard]] float GetBottom() const;

        [[nodiscard]] Point2 GetTopLeft() const;
        [[nodiscard]] Point2 GetTopRight() const;
        [[nodiscard]] Point2 GetBottomLeft() const;
        [[nodiscard]] Point2 GetBottomRight() const;

        [[nodiscard]] Size2 GetSize() const;
        [[nodiscard]] float GetWidth() const;
        [[nodiscard]] float GetHeight() const;
        [[nodiscard]] Size2 GetHalfSize() const;
        [[nodiscard]] Point2 GetCenter() const;

        Rect &operator+=(const Vec2 &vec2);

    private:
        Point2 _topRight{};
        Point2 _bottomLeft{};
        Point2 _center{};
        Size2 _size{};
        Size2 _halfSize{};

        void Update();
    } Rectangle;

    typedef struct GridEntry {
    public:
        union {
            int Row;
            int Rows;
        };

        union {
            int Column;
            int Columns;
        };

        GridEntry();
        explicit GridEntry(const int size);
        GridEntry(const int row, const int column);
        static Coordinates ToWorld(const int row, const int column, const float cellSize, const Coordinates &origin);
        static bool InBounds(const GridEntry &entry, const GridEntry &size);
        static bool FromWorld(const Position &position, const Coordinates &origin, const float cellSize, const GridEntry &gridSize, GridEntry &output);
        static Position GetCellPosition(const Position &position, const Coordinates &origin, const float cellSize);
        static bool ActivatePath(const gmath::GridEntry &start, const gmath::GridEntry &candidate);
        static bool PathRefresh(const gmath::GridEntry &start, const gmath::GridEntry &candidate);
        static unsigned char GetSquarePartition(const Position &position, float semiWidth, const Coordinates &origin, float cellSize);
        static void GetPartitions(uint8_t partition, const Position &position, const Coordinates &origin, float cellSize, const GridEntry &gridSize, GridEntry output[4]);
        bool operator==(const GridEntry &other) const;
        bool operator!=(const GridEntry &other) const;
        static GridEntry NullGridEntry;
    } GridSize, GridPosition, GridCoordinates;
}  // namespace gmath

namespace std {
    template<>
    struct hash<gmath::GridEntry> {
        std::size_t operator()(const gmath::GridEntry &entry) const noexcept {
            std::hash<float> hasher;
            std::size_t hashValue = 0;
            hashValue ^= hasher(entry.Row) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            hashValue ^= hasher(entry.Column) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            return hashValue;
        }
    };
}  // namespace std

#endif  // GMATH_H
