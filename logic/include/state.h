#ifndef STATE_H
#define STATE_H

namespace logic {
    class Enemy;

    class State {
    protected:
        Enemy *_enemy{};

    public:
        void SetEnemy(Enemy &enemy) { _enemy = &enemy; }
        virtual void Execute(const float delta) = 0;
        virtual void Update(){};
    };

    struct AttackState : State {
        void Execute(const float delta) override;
        void Update() override;
    };

    struct WanderState : State {
        void Execute(const float delta) override;
        void Update() override;
    };

    struct PursueState : State {
        void Execute(const float delta) override;
        void Update() override;
    };

    struct RecoverState : State {
        void Execute(const float delta) override;
        void Update() override;
    };

    struct KamikazeState : State {
        void Execute(const float delta) override;
        void Update() override;
    };
}  // namespace logic

#endif  // STATE_H