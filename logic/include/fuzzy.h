#ifndef FUZZY_H
#define FUZZY_H

namespace logic {
    enum class Variable { Distance, Fear, Ammo };

    enum class Distance { Close, Average, Far };
    enum class Fear { Anxious, Neutral, Warrior };
    enum class Ammo { Full, Midway, Empty };

    class Fuzzy {
    public:
        static bool Evaluate(float distance, float fear, float ammo);

    private:
        static float Triangle(float x, float a, float b, float c);
        static float Trapezoid(float x, float a, float b, float c, float d);
        static float Distance(float distance, Distance& result);
        static float Fear(float fear, Fear& result);
        static float Ammo(int ammo, Ammo& result);
    };
}
#endif