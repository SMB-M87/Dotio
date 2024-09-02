#include "fuzzy.h"
#include "grid.h"

namespace logic {
    float Fuzzy::Triangle(float x, float a, float b, float c) {
        if (x <= a || x >= c)
            return 0.0;
        if (x == b)
            return 1.0;
        if (x < b)
            return (x - a) / (b - a);
        return (c - x) / (c - b);
    }

    float Fuzzy::Trapezoid(float x, float a, float b, float c, float d) {
        if (x <= a || x >= d)
            return 0.0;
        if (x >= b && x <= c)
            return 1.0;
        if (x < b)
            return (x - a) / (b - a);
        return (d - x) / (d - c);
    }

    float Fuzzy::Distance(float distance, logic::Distance& result) {
        float close = Triangle(distance, 0, logic::Grid::CellSize * 4.0f, logic::Grid::CellSize * 5.0f);
        float neutral = Trapezoid(distance,
            logic::Grid::CellSize * 3.0f,
            logic::Grid::CellSize * 5.0f,
            logic::Grid::CellSize * 8.0f,
            logic::Grid::CellSize * 10.0f);
        float farr = Triangle(distance, 
            logic::Grid::CellSize * 10.0f, 
            logic::Grid::CellSize * 20.0f, 
            logic::Grid::CellSize * 100.0f);

        if (close > neutral && close > farr) {
            result = Distance::Close;
            return close;
        } else if (neutral > farr) {
            result = Distance::Average;
            return neutral;
        } else {
            result = Distance::Far;
            return farr;
        }
    }

    float Fuzzy::Fear(float fear, logic::Fear& result) {
        float anxious = Triangle(fear, 6.5f, 10.0f, 10.0f);
        float neutral = Trapezoid(fear, 4.0f, 4.5f, 6.0f, 7.0f);
        float chad = Triangle(fear, 1.5f, 4.5f, 4.5f);

        if (anxious > neutral && anxious > chad) {
            result = Fear::Anxious;
            return anxious;
        } else if (neutral > chad) {
            result = Fear::Neutral;
            return neutral;
        } else {
            result = Fear::Warrior;
            return chad;
        }
    }

    float Fuzzy::Ammo(int ammo, logic::Ammo& result) {
        float full = Triangle(static_cast<float>(ammo), 0.0f, 1.0f, 2.0f);
        float midway = Trapezoid(static_cast<float>(ammo), 1.0f, 4.0f, 6.0f, 7.0f);
        float empty = Triangle(static_cast<float>(ammo), 6.0f, 10.0f, 10.0f);

        if (full > midway && full > empty) {
            result = Ammo::Full;
            return full;
        } else if (midway > empty) {
            result = Ammo::Midway;
            return midway;
        } else {
            result = Ammo::Empty;
            return empty;
        }
    }

    bool Fuzzy::Evaluate(float distance, float fear, float ammo) {
        logic::Distance distanceResult;
        logic::Fear fearResult;
        logic::Ammo ammoResult;

        float distanceDegree = Distance(distance, distanceResult);
        float fearDegree = Fear(fear, fearResult);
        float ammoDegree = Ammo(ammo, ammoResult);

        if (distanceResult == logic::Distance::Close && fearResult == Fear::Warrior && ammoResult == Ammo::Midway) {
            return true;
        }

        return false;
    }
}  // namespace logic