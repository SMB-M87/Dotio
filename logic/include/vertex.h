#ifndef VERTEX_H
#define VERTEX_H

#include <list>

#include "gmath.h"

namespace logic {
    class Vertex {
    public:
        gmath::GridEntry cell;
        float G, H, F;
        gmath::GridEntry prev;
        int steps;

        int straightIndex{};
        int diagonalIndex{};
        gmath::GridEntry straight[4]{};
        gmath::GridEntry diagonal[4]{};

        Vertex();
        Vertex(const gmath::GridEntry& cell);
        Vertex(const Vertex& other);
        Vertex& operator=(const Vertex& other);

        void Reset();
        void AddAdjacent(logic::Vertex& source, const gmath::GridEntry& dest);
        float Calculate(const gmath::GridEntry& goal);
        static Vertex NullVertex;
    };
}
#endif
