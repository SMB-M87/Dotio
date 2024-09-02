#include "graph.h"

namespace logic {
    Graph::Graph() : vertexMap() {}

    void Graph::ClearAll() {
        for (auto& pair: vertexMap) {
            Vertex& vertex = pair.second;
            vertex.Reset();
        }
    }

    void Graph::AddVertex(const gmath::GridEntry& cell) {
        if (vertexMap.find(cell) == vertexMap.end()) {
            vertexMap[cell] = Vertex(cell);
        }
    }

    Vertex& Graph::GetVertex(const gmath::GridEntry& cell) {
        if (vertexMap.find(cell) != vertexMap.end())
            return vertexMap[cell];
        else
            return logic::Vertex::NullVertex;
    }

    bool Graph::AStar(const gmath::GridEntry& start, const gmath::GridEntry& goal, Vertex& output) {
        ClearAll();
        logic::PriorityQueue q(goal);

        Vertex* vertex = &GetVertex(start);
        vertex->G = 0;
        vertex->Calculate(goal);
        vertex->steps = 0;
        q.push(*vertex);

        while (!q.empty()) {
            vertex = &q.pop();

            if (vertex->cell == goal) {
                output = *vertex;
                return true;
            }

            for (int i = 0; i < vertex->straightIndex; i++) {
                Vertex& straightVertex = GetVertex(vertex->straight[i]);
                float G = vertex->G + 1.0f;
                if (G < straightVertex.G) {
                    straightVertex.prev = vertex->cell;
                    straightVertex.G = G;
                    straightVertex.Calculate(goal);
                    straightVertex.steps = vertex->steps + 1;
                    q.push(straightVertex);
                }
            }

            for (int i = 0; i < vertex->diagonalIndex; i++) {
                Vertex& diagonalVertex = GetVertex(vertex->diagonal[i]);
                float G = vertex->G + std::numbers::sqrt2_v<float>;
                if (G < diagonalVertex.G) {
                    diagonalVertex.prev = vertex->cell;
                    diagonalVertex.G = G;
                    diagonalVertex.Calculate(goal);
                    diagonalVertex.steps = vertex->steps + 1;
                    q.push(diagonalVertex);
                }
            }
        }
        return false;
    }

    bool Graph::HideStar(const gmath::GridEntry& start, const gmath::Position& player, const float fear, Vertex& output) {
        ClearAll();
        logic::PriorityQueue q(start);

        gmath::GridEntry player_output;
        gmath::GridEntry::FromWorld(player, logic::Grid::Origin, logic::Grid::CellSize, logic::Grid::GridSize, player_output);
        int row = start.Row - player_output.Row;
        int column = start.Column - player_output.Column;
        bool up = row == 0 ? true : row < 0 ? true : false;
        bool down = row == 0 ? true : row > 0 ? true : false;
        bool left = column == 0 ? true : column < 0 ? true : false;
        bool right = column == 0 ? true : column > 0 ? true : false;

        Vertex* vertex = &GetVertex(start);
        vertex->G = 0;
        vertex->steps = 0;
        q.push(*vertex);
        int count = 0;

        while (!q.empty()) {
            vertex = &q.pop();
            gmath::Point2 intersection{ gmath::GridEntry::ToWorld(
                vertex->cell.Row, vertex->cell.Column, logic::Grid::CellSize, logic::Grid::Origin) };
            intersection.X += logic::Grid::CellSize / 2;
            intersection.Y += logic::Grid::CellSize / 2;

            if (logic::Grid::IsIntersectingAnyCell(
                    { intersection, player }, reinterpret_cast<const int*>(logic::Grid::Array), intersection)) {
                count++;
                if (count > fear) {
                    output = *vertex;
                    return true;
                }
            }

            for (int i = 0; i < vertex->straightIndex; i++) {
                Vertex& straightVertex = GetVertex(vertex->straight[i]);

                float G = vertex->G + 1.0f;
                if (G < straightVertex.G && 
                    ((((up && straightVertex.cell.Row - player_output.Row <= 0) &&
                      ((left && straightVertex.cell.Column - player_output.Column < 0) || (right && straightVertex.cell.Column - player_output.Column > 0))) ||
                      ((down && straightVertex.cell.Row - player_output.Row >= 0) &&
                      ((left && straightVertex.cell.Column - player_output.Column < 0) || (right && straightVertex.cell.Column - player_output.Column > 0))) ||
                      ((left && straightVertex.cell.Column - player_output.Column <= 0) &&
                      ((up && straightVertex.cell.Row - player_output.Row < 0) || (down && straightVertex.cell.Row - player_output.Row > 0))) ||
                      ((right && straightVertex.cell.Column - player_output.Column >= 0) &&
                      ((up && straightVertex.cell.Row - player_output.Row < 0) || (down && straightVertex.cell.Row - player_output.Row > 0)))) &&
                     (abs(straightVertex.cell.Row - player_output.Row) > 1 ||
                     abs(straightVertex.cell.Column - player_output.Column) > 1))) {
                    straightVertex.prev = vertex->cell;
                    straightVertex.G = G;
                    straightVertex.steps = vertex->steps + 1;
                    q.push(straightVertex);
                }
            }

            for (int i = 0; i < vertex->diagonalIndex; i++) {
                Vertex& diagonalVertex = GetVertex(vertex->diagonal[i]);

                float G = vertex->G + std::numbers::sqrt2_v<float>;
                if (G < diagonalVertex.G &&
                    ((((up && diagonalVertex.cell.Row - player_output.Row <= 0) &&
                      ((left && diagonalVertex.cell.Column - player_output.Column < 0) || (right && diagonalVertex.cell.Column - player_output.Column > 0))) ||
                      ((down && diagonalVertex.cell.Row - player_output.Row >= 0) &&
                      ((left && diagonalVertex.cell.Column - player_output.Column < 0) || (right && diagonalVertex.cell.Column - player_output.Column > 0))) ||
                      ((left && diagonalVertex.cell.Column - player_output.Column <= 0) &&
                      ((up && diagonalVertex.cell.Row - player_output.Row < 0) || (down && diagonalVertex.cell.Row - player_output.Row > 0))) ||
                      ((right && diagonalVertex.cell.Column - player_output.Column >= 0) &&
                      ((up && diagonalVertex.cell.Row - player_output.Row < 0) || (down && diagonalVertex.cell.Row - player_output.Row > 0)))) &&
                     (abs(diagonalVertex.cell.Row - player_output.Row) > 1 ||
                     abs(diagonalVertex.cell.Column - player_output.Column) > 1))) {
                    diagonalVertex.prev = vertex->cell;
                    diagonalVertex.G = G;
                    diagonalVertex.steps = vertex->steps + 1;
                    q.push(diagonalVertex);
                }
            }
        }
        return false;
    }

    logic::Stack Graph::GetPath(const gmath::GridEntry& start, const Vertex& goal) {
        Vertex vertex = goal;
        logic::Stack path;
        int end = goal.steps;
        bool smaller = end <= path.capacity() - 1;
        int boundary = smaller ? end : end - path.capacity();
        int index = 0;

        if (!smaller)
            while (index++ < boundary) vertex = GetVertex(vertex.prev);

        while (index++ <= end) {
            path.push(vertex.cell);
            vertex = GetVertex(vertex.prev);
        }

        return path;
    }
}  // namespace logic
