#include "state.h"
#include "enemy.h"
#include "grid.h"
#include "world.h"
#include "fuzzy.h"

namespace logic {
    void WanderState::Execute(const float delta) {
        if (_enemy->Path.empty())
            return;

        gmath::GridEntry cell = _enemy->Path.top();
        gmath::Coordinates pos =
            gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
        pos.X += logic::Grid::CellSize / 2;
        pos.Y += logic::Grid::CellSize / 2;

        if ((pos - _enemy->Position) < logic::Grid::CellSize * 0.666) {
            _enemy->Path.pop();
            if (!_enemy->Path.empty()) {
                cell = _enemy->Path.top();
                gmath::Coordinates pos =
                    gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
                pos.X += logic::Grid::CellSize / 2;
                pos.Y += logic::Grid::CellSize / 2;
            }
        }

        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2 * std::numbers::pi_v<float>;
        gmath::Vec2 displacement(
            (logic::Grid::CellSize) *std::cos(angle), (logic::Grid::CellSize * 0.4f) * std::sin(angle));
        gmath::Vec2 target = pos + displacement;
        gmath::Vec2 force = (target - _enemy->Position).GetNormalized() * 0.75f;
        _enemy->ApplyForce(force * delta, 0.5f * _enemy->GetMass());
    }

    void WanderState::Update() {
        if (_enemy->LineOfSight) {
            _enemy->Path.clear();
            _enemy->SetState(_enemy->GetAttackState());
            return;
        }

        if (_enemy->Path.empty()) {
            gmath::GridEntry enemy_output;
            if (gmath::GridEntry::FromWorld(_enemy->Position,
                    logic::Grid::Origin,
                    logic::Grid::CellSize,
                    logic::Grid::GridSize,
                    enemy_output)) {
                gmath::GridEntry enemy_grid = { enemy_output.Row, enemy_output.Column };
                if (logic::Vertex _;
                    _enemy->GetWorld().GetGraph().AStar(enemy_grid, logic::Grid::GenerateGridEntry(enemy_grid), _))
                    _enemy->Path = _enemy->GetWorld().GetGraph().GetPath(enemy_grid, _);
            }
        }
    }

    void AttackState::Execute(const float delta) {
        bool boundary = sqrt(pow(_enemy->Position.X - _enemy->Target.X, 2) + pow(_enemy->Position.Y - _enemy->Target.Y, 2)) <
                        (logic::Grid::CellSize * (_enemy->Fear * (_enemy->AmmoUsedRatio())) + logic::Grid::CellSize * 0.5f);

        gmath::Vec2 force = {(boundary ? _enemy->Position - _enemy->Target : _enemy->Target - _enemy->Position).GetNormalized() * 0.85f };

        _enemy->ApplyForce(force * delta, 0.5f * _enemy->GetMass());
    }

    void AttackState::Update() {
        if (logic::Fuzzy::Evaluate(_enemy->Distance(), _enemy->Fear, _enemy->GetAmmo())) {
            _enemy->SetState(_enemy->GetKamikazeState());
            return;
        }
        if (_enemy->IsOutOfAmmo()) {
            _enemy->SetState(_enemy->GetRecoverState());
            return;
        }
        if (!_enemy->LineOfSight) {
            _enemy->ResetPursue();
            _enemy->SetState(_enemy->GetPursueState());
            return;
        }

        if (_enemy->GetCooldown() <= 0.0f) {
            _enemy->ResetCooldown();
            _enemy->AmmoFired();
            float arrowSpeed = 13.0f;
            gmath::Vector2 shoot = { arrowSpeed, 0.0f };
            shoot.Rotate(_enemy->Position.SignedAngleDifference(shoot, _enemy->Target) +
                gmath::Vec2::Intercept(_enemy->TargetVelocity.GetLength(), arrowSpeed, _enemy->Target.SignedAngleDifference(_enemy->TargetVelocity, _enemy->Position)));
            _enemy->GetWorld().AddArrow(_enemy->Position, shoot);
        }
    }

    void PursueState::Execute(const float delta) {
        if (_enemy->Path.empty())
            return;

        gmath::GridEntry cell = _enemy->Path.top();
        gmath::Coordinates pos =
            gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
        pos.X += logic::Grid::CellSize / 2;
        pos.Y += logic::Grid::CellSize / 2;

        if ((pos - _enemy->Position) < logic::Grid::CellSize * 0.666) {
            _enemy->Path.pop();
            if (!_enemy->Path.empty()) {
                cell = _enemy->Path.top();
                gmath::Coordinates pos =
                    gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
                pos.X += logic::Grid::CellSize / 2;
                pos.Y += logic::Grid::CellSize / 2;
            }
        }

        if (pos != _enemy->Position) {
            gmath::Vec2 force = { (pos - _enemy->Position).GetNormalized() * 0.85f };
            _enemy->ApplyForce(force * delta, 0.5f * _enemy->GetMass());
        }
    }

    void PursueState::Update() {
        if (_enemy->LineOfSight) {
            _enemy->Path.clear();
            _enemy->SetState(_enemy->GetAttackState());
            return;
        }
        if (_enemy->PursueCountdownPassed()) {
            _enemy->Path.clear();
            _enemy->SetState(_enemy->GetWanderState());
            return;
        }

        if (_enemy->GetCooldown() <= 0.0f) {
            _enemy->ResetCooldown();
            _enemy->PursueCountdown();
            if (_enemy->GetAmmo() > 6)
                _enemy->AmmoRecovered();
        }

        gmath::GridEntry target_output;
        if (_enemy->Path.empty() ||
            gmath::GridEntry::PathRefresh(_enemy->Path.get(0), { target_output.Row, target_output.Column })) {
            gmath::GridEntry enemy_output;
            if (gmath::GridEntry::FromWorld(_enemy->Target,logic::Grid::Origin,logic::Grid::CellSize,logic::Grid::GridSize,target_output) &&
                gmath::GridEntry::FromWorld(_enemy->Position,logic::Grid::Origin,logic::Grid::CellSize,logic::Grid::GridSize,enemy_output)) {
                if (logic::Vertex _; _enemy->GetWorld().GetGraph().AStar({ enemy_output.Row, enemy_output.Column }, { target_output.Row, target_output.Column }, _))
                    _enemy->Path = _enemy->GetWorld().GetGraph().GetPath({ enemy_output.Row, enemy_output.Column }, _);
            }
        }
    }

    void RecoverState::Execute(const float delta) {
        if (_enemy->Path.empty())
            return;

        gmath::GridEntry cell = _enemy->Path.top();
        gmath::Coordinates pos = gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
        pos.X += logic::Grid::CellSize / 2;
        pos.Y += logic::Grid::CellSize / 2;

        if ((pos - _enemy->Position) < logic::Grid::CellSize * (_enemy->Path.size() > 1 ? 0.666 : 0.1)) {
            _enemy->Path.pop();
            if (!_enemy->Path.empty()) {
                cell = _enemy->Path.top();
                gmath::Coordinates pos =
                    gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
                pos.X += logic::Grid::CellSize / 2;
                pos.Y += logic::Grid::CellSize / 2;
            }
        }

        if (pos != _enemy->Position) {
            gmath::Vec2 force = { (pos - _enemy->Position).GetNormalized() * 1.1f };
            _enemy->ApplyForce(force * delta, 0.5f * _enemy->GetMass());
        }
    }

    void RecoverState::Update() {
        if (_enemy->HasReloadedAmmo()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(2.0f, 6.0f);
            _enemy->Fear = dis(gen);
            _enemy->Path.clear();
            _enemy->ResetPursue();
            _enemy->SetState(_enemy->GetPursueState());
            return;
        }

        if (!_enemy->LineOfSight && _enemy->GetCooldown() <= 0.0f) {
            _enemy->ResetCooldown();
            _enemy->AmmoRecovered();
        }

        if ((_enemy->LineOfSight && _enemy->Path.empty()) ||
            ((abs(_enemy->Position.X - _enemy->Target.X) <= logic::Grid::CellSize * 2.5f) &&
            ((abs(_enemy->Position.Y - _enemy->Target.Y) <= logic::Grid::CellSize * 2.5f)))) {
            gmath::GridEntry enemy_output;
            if (gmath::GridEntry::FromWorld(_enemy->Position,
                    logic::Grid::Origin,
                    logic::Grid::CellSize,
                    logic::Grid::GridSize,
                    enemy_output)) {
                if (logic::Vertex _; _enemy->GetWorld().GetGraph().HideStar({ enemy_output.Row, enemy_output.Column }, _enemy->Target, _enemy->Fear, _))
                    _enemy->Path = _enemy->GetWorld().GetGraph().GetPath({ enemy_output.Row, enemy_output.Column }, _);
                else
                    _enemy->SetState(_enemy->GetKamikazeState());
            }
        }
    }

    void KamikazeState::Execute(const float delta) {
        if (_enemy->LineOfSight) {
            gmath::Vec2 force = {(_enemy->Target - _enemy->Position).GetNormalized() * 1.3f };
            _enemy->ApplyForce(force * delta, 0.5f * _enemy->GetMass());      
            _enemy->Path.clear();
            return;
        }

        if (_enemy->Path.empty())
            return;

        gmath::GridEntry cell = _enemy->Path.top();
        gmath::Coordinates pos =
            gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
        pos.X += logic::Grid::CellSize / 2;
        pos.Y += logic::Grid::CellSize / 2;

        if ((pos - _enemy->Position) < logic::Grid::CellSize * 0.666) {
            _enemy->Path.pop();
            if (!_enemy->Path.empty()) {
                cell = _enemy->Path.top();
                gmath::Coordinates pos =
                    gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
                pos.X += logic::Grid::CellSize / 2;
                pos.Y += logic::Grid::CellSize / 2;
            }
        }

        if (pos != _enemy->Position) {
            gmath::Vec2 force = { (pos - _enemy->Position).GetNormalized() * 1.3f };
            _enemy->ApplyForce(force * delta, 0.5f * _enemy->GetMass());
        }
    }

    void KamikazeState::Update() {
        if (_enemy->LineOfSight && !_enemy->IsOutOfAmmo() && _enemy->GetCooldown() <= 0.0f) {
            _enemy->ResetCooldown();
            _enemy->AmmoFired();
            float arrowSpeed = 13.0f;
            gmath::Vector2 shoot = { arrowSpeed, 0.0f };
            shoot.Rotate(_enemy->Position.SignedAngleDifference(shoot, _enemy->Target) +
                         gmath::Vec2::Intercept(_enemy->TargetVelocity.GetLength(),
                             arrowSpeed,
                             _enemy->Target.SignedAngleDifference(_enemy->TargetVelocity, _enemy->Position)));
            _enemy->GetWorld().AddArrow(_enemy->Position, shoot);
        } else if (!_enemy->LineOfSight && _enemy->GetAmmo() > 5 && _enemy->GetCooldown() <= 0.0f) {
            _enemy->AmmoRecovered();       
        }

        gmath::GridEntry target_output;
        if (!_enemy->LineOfSight && (_enemy->Path.empty() || 
            gmath::GridEntry::PathRefresh(_enemy->Path.get(0), { target_output.Row, target_output.Column }))) {
            gmath::GridEntry enemy_output;
            if (gmath::GridEntry::FromWorld(_enemy->Target, logic::Grid::Origin, logic::Grid::CellSize, logic::Grid::GridSize, target_output) &&
                gmath::GridEntry::FromWorld(_enemy->Position,logic::Grid::Origin,logic::Grid::CellSize,logic::Grid::GridSize,enemy_output)) {
                if (logic::Vertex _; _enemy->GetWorld().GetGraph().AStar(
                        { enemy_output.Row, enemy_output.Column }, { target_output.Row, target_output.Column }, _))
                    _enemy->Path = _enemy->GetWorld().GetGraph().GetPath({ enemy_output.Row, enemy_output.Column }, _);
            }
        }
    }
}