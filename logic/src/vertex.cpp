#include "vertex.h"

namespace logic {
    Vertex::Vertex() : prev(gmath::GridEntry::NullGridEntry), G(FLT_MAX), H(0), F(FLT_MAX), steps(0) {}

    Vertex::Vertex(const gmath::GridEntry& cell) :
        cell(cell), prev(gmath::GridEntry::NullGridEntry), G(FLT_MAX), H(0), F(FLT_MAX), steps(0) {}

    Vertex::Vertex(const Vertex& other) : cell(other.cell), prev(other.prev), G(other.G), H(0), F(FLT_MAX), steps(0) {
        for (int i = 0; i < other.straightIndex; i++) {
            straight[i] = other.straight[i];
        }

        straightIndex = other.straightIndex;

        for (int i = 0; i < other.diagonalIndex; i++) {
            diagonal[i] = other.diagonal[i];
        }

        diagonalIndex = other.diagonalIndex;
    }

    Vertex& Vertex::operator=(const Vertex& other) {
        if (this != &other) {
            cell = other.cell;
            prev = other.prev;
            G = other.G;
            H = other.H;
            F = other.F;
            steps = other.steps;

            for (int i = 0; i < other.straightIndex; i++) {
                straight[i] = other.straight[i];
            }
            straightIndex = other.straightIndex;

            for (int i = 0; i < other.diagonalIndex; i++) {
                diagonal[i] = other.diagonal[i];
            }
            diagonalIndex = other.diagonalIndex;
        }
        return *this;
    }

    void Vertex::Reset() {
        prev = gmath::GridEntry::NullGridEntry;
        G = FLT_MAX;
        H = 0;
        F = FLT_MAX;
        steps = 0;
    }

    void Vertex::AddAdjacent(logic::Vertex& source, const gmath::GridEntry& dest) {
        if (source.cell.Row != dest.Row && source.cell.Column != dest.Column)
            source.diagonal[source.diagonalIndex++] = { dest.Row, dest.Column };
        else
            source.straight[source.straightIndex++] = { dest.Row, dest.Column };
    }

    float Vertex::Calculate(const gmath::GridEntry& goal) {
        int row = fabsf(cell.Column - goal.Column);
        int column = fabsf(cell.Column - goal.Column);

        H = 1.41f * (row + column) + (1.0f - (2 * 1.41f)) * min(cell.Row, goal.Row);
        F = G + H;
        return F;
    }

    Vertex Vertex::NullVertex = Vertex();
}  // namespace logic
