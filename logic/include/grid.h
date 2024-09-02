#ifndef GRID_H
#define GRID_H

#include <physics_object.h>

namespace logic {
    struct quadrantResult {
        int8_t x;
        int8_t y;
    };

    class Grid {
    public:
        static const int Rows = 155;
        static const int Columns = 155;
        static const gmath::GridSize GridSize;
        static gmath::Coordinates Origin;
        static float CellSize;
        static int Array[Rows][Columns];

        static void HandleGridCollisionCorners(logic::PhysicsObject &object, const gmath::Position &cellPosition, const gmath::Vec2 &sign);

        static void HandleGridCollision(logic::PhysicsObject &object, const float delta, const int *Array);

        static bool GridIntersection(const gmath::Segment &segment, const int *Array, gmath::Point2 &intersection);

        static bool SegmentIsIntersecingCircle(const gmath::Segment &segment, const gmath::Point2 c, const float r);

        static uint8_t SegementCircleIntersection(const gmath::Segment &segment, const gmath::Point2 c, const float r, gmath::Point2 &outIntersection);

        static bool IsIntersectingAnyCell(gmath::Segment segment, const int *Array, gmath::Point2 &intersection);

        static gmath::GridEntry GenerateGridEntry(gmath::GridEntry origin);

        static gmath::Position GeneratePosition();
    };
}  // namespace logic
#endif
