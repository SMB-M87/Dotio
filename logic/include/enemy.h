#ifndef ENEMY_H
#define ENEMY_H

#include "physics_object.h"
#include "stack.h"
#include "state.h"

namespace logic {
    class World;

    class Enemy final : public PhysicsObject {
        float _cooldown{};
        float _maxCooldown{ 1.0f };
        int _ammo{ 1 };
        int _countdown{ 0 };
        int _threshold{ 10 };
        World *_world{};
        State *_state{};
        AttackState _attack;
        WanderState _wander;
        PursueState _pursue;
        RecoverState _recover;
        KamikazeState _kamikaze;

        void PlayerBumpedIntoEnemy();
        void SpatialPartitioning(const float delta);

    public:
        gmath::Vec2 Target{};
        gmath::Vec2 TargetVelocity{};
        logic::Stack Path;
        float Fear{};
        bool LineOfSight{ false }, Debug{ false };

        Enemy();

        explicit Enemy(World *world, gmath::Position position, gmath::Position target, float direction, float fear);

        ~Enemy() override = default;

        void Update(float delta) override;

        void Tick() override;

        void Draw(gamef::Renderer &renderer) override;

        void Initialize();

        World& GetWorld();

        State &GetState();
        void SetState(State& state);

        State &GetWanderState();
        State &GetAttackState();
        State &GetPursueState();
        State &GetRecoverState();
        State &GetKamikazeState();

        bool IsAttacking();
        bool IsWandering();
        bool IsPursueing();
        bool IsRecovering();
        bool IsKamikazing();
        bool IsInLineOfSight();
        bool IsOutOfAmmo() const;
        bool HasReloadedAmmo() const;
        bool PursueCountdownPassed() const;

        int GetAmmo() const;
        float GetCooldown() const;
        float AmmoUsedRatio() const;
        float Distance() const;

        void AmmoFired();
        void AmmoRecovered();
        void PursueCountdown();
        void ResetCooldown();
        void ResetPursue();
        void Reset();
    };
}  // namespace logic

#endif  // ENEMY_H
