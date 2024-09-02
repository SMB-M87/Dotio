#ifndef WORLD_H
#define WORLD_H

#include "arrow.h"
#include "bolt.h"
#include "enemy.h"
#include "player.h"
#include "tracker.h"
#include "graph.h"

#include <unordered_set>

namespace logic {
    class World {
        float _tickRate{ 1.0f / 20.0f };
        logic::Player _player;
        logic::Player _ghost;
        TrackerData<Enemy, 666> _enemies;
        TrackerData<Arrow, 666> _arrows;
        TrackerData<Bolt, 20> _bolts;
        logic::Graph _graph{};

    public:
        std::unordered_map<gmath::GridEntry, std::unordered_set<logic::PhysicsObject *>> EnemyPartitionMap;

        void Initialize();

        void Reset();

        float GetTickRate() const;

        void ExtractGraph();

        void SetPlayer(logic::Player player);

        logic::Player& GetPlayer();

        void SetGhost(logic::Player ghost);

        logic::Player& GetGhost();

        void AddEnemy(gmath::Position position, gmath::Position target, const float fear, bool active);

        void AddArrow(gmath::Position position, gmath::Vector2 velocity);

        void AddBolt(const gmath::Position position, const gmath::Vector2 velocity);

        void UpdateEnemies(const float delta, const bool debug);

        void UpdateArrows(const float delta, const bool debug);

        void UpdateBolts(const float delta);

        void Draw(gamef::Renderer& renderer);

        [[nodiscard]] Tracker<Enemy>& GetEnemies();

        [[nodiscard]] Tracker<Arrow>& GetArrows();

        [[nodiscard]] Tracker<Bolt>& GetBolts();

        [[nodiscard]] logic::Graph& GetGraph();
    };
}  // namespace logic

#endif
