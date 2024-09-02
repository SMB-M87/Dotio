#include "world.h"

namespace logic {
    float World::GetTickRate() const { return _tickRate; }

    void World::Initialize() {
        ExtractGraph();

        SetPlayer(logic::Player(this, logic::Grid::GeneratePosition(), 0.0f));
        SetGhost(logic::Player(this, logic::Grid::GeneratePosition(), 0.0f));

        for (int i = 0; i < GetEnemies().GetCapacity(); i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(2.0f, 10.0f);
            float fear = dis(gen);
            AddEnemy(logic::Grid::GeneratePosition(),
                GetPlayer().Position,
                fear,
                true);
            _enemies[i].Initialize();
        }
    }

    void World::Reset() {
        GetPlayer().Position = logic::Grid::GeneratePosition();
        GetPlayer().Health = GetPlayer().MaxHealth;
        GetPlayer().Mana = GetPlayer().MaxMana;
        GetPlayer().Kills = 1;
        for (auto& enemy: GetEnemies()) {
            enemy.Position = logic::Grid::GeneratePosition();
            enemy.Target = GetPlayer().Position;
            enemy.Reset();
        }
    }

    void World::ExtractGraph() {
        for (int row = 0; row < logic::Grid::GridSize.Rows; row++) {
            for (int column = 0; column < logic::Grid::GridSize.Columns; column++) {
                if (logic::Grid::Array[row][column] == 0) {
                    _graph.AddVertex({ row, column });
                    logic::Vertex& vertex = _graph.GetVertex({ row, column });

                    if (row > 0 && logic::Grid::Array[row - 1][column] == 0)
                        vertex.AddAdjacent(vertex, { row - 1, column });

                    if (row < logic::Grid::GridSize.Rows - 1 && logic::Grid::Array[row + 1][column] == 0)
                        vertex.AddAdjacent(vertex, { row + 1, column });

                    if (column > 0 && logic::Grid::Array[row][column - 1] == 0)
                        vertex.AddAdjacent(vertex, { row, column - 1 });

                    if (column < logic::Grid::GridSize.Columns - 1 && logic::Grid::Array[row][column + 1] == 0)
                        vertex.AddAdjacent(vertex, { row, column + 1 });

                    if (row > 0 && column > 0 && logic::Grid::Array[row - 1][column - 1] == 0 &&
                        logic::Grid::Array[row][column - 1] == 0 && logic::Grid::Array[row - 1][column] == 0)
                        vertex.AddAdjacent(vertex, { row - 1, column - 1 });

                    if (row > 0 && column < logic::Grid::GridSize.Columns - 1 &&
                        logic::Grid::Array[row - 1][column + 1] == 0 && logic::Grid::Array[row][column + 1] == 0 &&
                        logic::Grid::Array[row - 1][column] == 0)
                        vertex.AddAdjacent(vertex, { row - 1, column + 1 });

                    if (row < logic::Grid::GridSize.Rows - 1 && column > 0 &&
                        logic::Grid::Array[row + 1][column - 1] == 0 && logic::Grid::Array[row][column - 1] == 0 &&
                        logic::Grid::Array[row + 1][column] == 0)
                        vertex.AddAdjacent(vertex, { row + 1, column - 1 });

                    if (row < logic::Grid::GridSize.Rows - 1 && column < logic::Grid::GridSize.Columns - 1 &&
                        logic::Grid::Array[row + 1][column + 1] == 0 && logic::Grid::Array[row][column + 1] == 0 &&
                        logic::Grid::Array[row + 1][column] == 0)
                        vertex.AddAdjacent(vertex, { row + 1, column + 1 });
                }
            }
        }
    }

    void World::SetPlayer(logic::Player player) { _player = player; }

    logic::Player& World::GetPlayer() { return _player; }

    void World::SetGhost(logic::Player ghost) { _ghost = ghost; }

    logic::Player& World::GetGhost() { return _ghost; }

    void World::AddEnemy(const gmath::Position position, gmath::Position target, const float fear, bool active) {
        _enemies += Enemy(this, position, target, 0.0f, fear);
    }

    void World::AddArrow(const gmath::Position position, const gmath::Vector2 velocity) {
        _arrows += Arrow(position, velocity);
    }

    void World::AddBolt(const gmath::Position position, const gmath::Vector2 velocity) {
        _bolts += Bolt(position, velocity);
    }

    void World::UpdateEnemies(const float delta, const bool debug) {
        for (auto& enemy: GetEnemies()) {
            if (!enemy.IsActive)
                continue;
            enemy.Target = GetPlayer().Position;
            enemy.TargetVelocity = GetPlayer().Velocity;
            enemy.Update(delta);
            enemy.Debug = debug;
            logic::Grid::HandleGridCollision(enemy, delta, reinterpret_cast<const int*>(logic::Grid::Array));
        }
    }

    void World::UpdateArrows(const float delta, const bool debug) {
        for (auto& arrow: GetArrows()) {
            if (!arrow.IsActive)
                continue;

            arrow.Update(delta);

            gmath::Point2 intersection;
            if (logic::Grid::IsIntersectingAnyCell({ arrow.Position, arrow.Position + arrow.Velocity * delta },
                    reinterpret_cast<const int*>(logic::Grid::Array),
                    intersection)) {
                arrow.IsActive = false;
            }

            if (logic::Grid::SegementCircleIntersection({ arrow.Position, arrow.Position + arrow.Velocity * delta },
                    GetPlayer().Position,
                    GetPlayer().Radius,
                    intersection) &
                0b1) {
                arrow.IsActive = false;
                if (!debug)
                    GetPlayer().Health -= 10.0f;

                if (GetPlayer().Health <= 0.0f) {
                    Reset();
                }
            }
        }
    }

    void World::UpdateBolts(const float delta) {
        for (auto& bolt: GetBolts()) {
            if (!bolt.IsActive)
                continue;

            bolt.Update(delta);

            gmath::Point2 intersection;
            if (logic::Grid::IsIntersectingAnyCell({ bolt.Position, bolt.Position + bolt.Velocity * delta },
                    reinterpret_cast<const int*>(logic::Grid::Array),
                    intersection)) {
                bolt.IsActive = false;
            }

            for (auto& enemy: GetEnemies()) {
                if (!enemy.IsActive)
                    continue;

                if (logic::Grid::SegementCircleIntersection(
                        { bolt.Position, bolt.Position - bolt.Velocity }, enemy.Position, enemy.Radius, intersection) &
                    0b1) {
                    if (GetPlayer().Health < GetPlayer().MaxHealth)
                        GetPlayer().Health += enemy.IsRecovering() ? 40.0f : enemy.IsKamikazing() ? 20.0f : 10.0f;
                    if ((enemy.IsRecovering() || enemy.IsKamikazing()) && GetPlayer().Mana < GetPlayer().MaxMana)
                        GetPlayer().Mana += enemy.IsRecovering() ? 40.0f : 20.0f;
                    enemy.IsActive = false;
                    bolt.IsActive = false;
                    if (GetPlayer().Kills < GetPlayer().MaxBulletKills + 1)
                        GetPlayer().Kills++;
                }
            }
        }
    }

    void World::Draw(gamef::Renderer& renderer) {
        for (auto& enemy: GetEnemies()) {
            if (!enemy.IsActive)
                continue;
            enemy.Draw(renderer);
        }

        for (auto& arrow: GetArrows()) {
            if (!arrow.IsActive)
                continue;
            arrow.Draw(renderer);
        }

        for (auto& bolt: GetBolts()) {
            if (!bolt.IsActive)
                continue;
            bolt.Draw(renderer);
        }

        GetPlayer().Draw(renderer);
    }

    Tracker<Enemy>& World::GetEnemies() { return _enemies.GetTracker(); }

    Tracker<Arrow>& World::GetArrows() { return _arrows.GetTracker(); }

    Tracker<Bolt>& World::GetBolts() { return _bolts.GetTracker(); }

    logic::Graph& World::GetGraph() { return _graph; }
}  // namespace logic
