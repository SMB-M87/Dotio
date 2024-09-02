#include "utility.h"

namespace app {
    void Utility::DrawRaster(gamef::Renderer &renderer) {
        gamef::Renderer::Mode::Enum previousMode = renderer.GetRenderMode();
        renderer.SetRenderMode(gamef::Renderer::Mode::Enum::FloatingUI);
        const float temp = renderer.GetPrimaryAlpha();
        renderer.SetPrimaryAlpha(0.5f);

        gmath::Size2 half{ renderer.GetScreen().GetVirtual().GetHalfSize() / renderer.Zoom };
        half += logic::Grid::CellSize;
        const gmath::Size2 offset{ fmodf(logic::Grid::Origin.X + half.Width, logic::Grid::CellSize),
            fmodf(logic::Grid::Origin.Y + half.Height, logic::Grid::CellSize) };

        // clang-format off
    const unsigned int widthCount = 1 + static_cast<unsigned int>((renderer.GetScreen().GetVirtual().GetWidth() / renderer.Zoom + logic::Grid::CellSize) / logic::Grid::CellSize);
    for (int i = 0; i <= widthCount; i++)
        renderer.DrawSegment({
            {
                -half.Width + (logic::Grid::CellSize * i) + renderer.Camera.X - fmod(renderer.Camera.X, logic::Grid::CellSize) + offset.X,
                renderer.Camera.Y - half.Height,
            },
            {
                -half.Width + (logic::Grid::CellSize * i) + renderer.Camera.X - fmod(renderer.Camera.X, logic::Grid::CellSize) + offset.X,
                renderer.Camera.Y + half.Height
            }
        }, { 0.25f, 0.0f, 0.25f, 1.0f });

    const unsigned int heightCount = 1 + static_cast<unsigned int>((renderer.GetScreen().GetVirtual().GetHeight() / renderer.Zoom + logic::Grid::CellSize) / logic::Grid::CellSize);
    for (int i = 0; i <= heightCount; i++)
        renderer.DrawSegment({
            {
                renderer.Camera.X - half.Width,
                -half.Height + (logic::Grid::CellSize * i) + renderer.Camera.Y - fmod(renderer.Camera.Y, logic::Grid::CellSize) + offset.Y,
            },
            {
                renderer.Camera.X + half.Width,
                -half.Height + (logic::Grid::CellSize * i) + renderer.Camera.Y - fmod(renderer.Camera.Y, logic::Grid::CellSize) + offset.Y,
            }
        }, { 0.25f, 0.0f, 0.25f, 1.0f });
        // clang-format on

        renderer.SetPrimaryAlpha(temp);
        renderer.SetRenderMode(previousMode);
    }

    void Utility::DrawGrid(gamef::Renderer &renderer) {
        gamef::Renderer::Mode::Enum previousMode = renderer.GetRenderMode();
        renderer.SetRenderMode(gamef::Renderer::Mode::Enum::FloatingUI);
        const float temp = renderer.GetPrimaryAlpha();
        renderer.SetPrimaryAlpha(1.0f);

        for (size_t row = 0; row < logic::Grid::GridSize.Rows; row++) {
            for (size_t column = 0; column < logic::Grid::GridSize.Columns; column++) {
                gmath::Coordinates position =
                    gmath::GridEntry::ToWorld(row, column, logic::Grid::CellSize, logic::Grid::Origin);
                gmath::Rectangle wall = { position, position + logic::Grid::CellSize };
                if (logic::Grid::Array[row][column] == 1)
                    renderer.DrawRectangle(wall,
                        { (float) row * 0.00632f, (float) column * 0.00632f, (float) (row + column) * 0.00319f, 1.0f });
                else if (logic::Grid::Array[row][column] == 2)
                    renderer.DrawRectangle(wall, { 1.0, 0.0f, (float) (row + column) * 0.00319f, 1.0f });
            }
        }
        renderer.SetPrimaryAlpha(temp);
        renderer.SetRenderMode(previousMode);
    }

    void Utility::DrawPlayerIndicators(gamef::Renderer &renderer, logic::World& world) {
        gamef::Screen screen{ renderer.GetScreen() };
        gmath::Point2 normalPixel{ screen.GetNormalPixel() };

        // Draw health bar
        gmath::Rectangle healthBar{ screen.GetNormal().GetBottomLeft() + gmath::Point2(0.0f, -0.05f),
            screen.GetNormal().GetBottomLeft() + gmath::Point2(0.3f, -0.1f) };

        healthBar += gmath::Point2(10.0f, -60.0f) * normalPixel;
        renderer.DrawRectangle(healthBar, { 0.1f, 0.0f, 0.0f, 1.0f });

        healthBar.SetTo(
            screen.GetNormal().GetBottomLeft() +
            gmath::Point2(world.GetPlayer().Health * 0.3f / world.GetPlayer().MaxHealth + 10.0f * normalPixel.X,
                -0.1f - 60.0f * normalPixel.Y));
        renderer.DrawRectangle(healthBar, { 0.5f, 0.0f, 0.0f, 1.0f });

        // Draw mana bar
        gmath::Rectangle manaBar{ screen.GetNormal().GetBottomLeft() + gmath::Point2(0.0f, -0.05f),
            screen.GetNormal().GetBottomLeft() + gmath::Point2(0.3f, -0.1f) };

        manaBar += gmath::Point2(10.0f, -20.0f) * normalPixel;
        renderer.DrawRectangle(manaBar, { 0.0f, 0.0f, 0.1f, 1.0f });

        manaBar.SetTo(screen.GetNormal().GetBottomLeft() +
                      gmath::Point2(world.GetPlayer().Mana * 0.3f / world.GetPlayer().MaxMana + 10.0f * normalPixel.X,
                          -0.1f - 20.0f * normalPixel.Y));
        renderer.DrawRectangle(manaBar, { 0.0f, 0.0f, 1.0f, 1.0f });

        // Bullet speed bar
        gmath::Rectangle bulletBar{ screen.GetNormal().GetBottomLeft(),
            screen.GetNormal().GetBottomLeft() + gmath::Point2(0.3f, -0.05f) };

        bulletBar += gmath::Point2(10.0f, -10.0f) * normalPixel;
        renderer.DrawRectangle(bulletBar, { 0.1f, 0.1f, 0.0f, 1.0f });

        bulletBar.SetTo(screen.GetNormal().GetBottomLeft() +
                        gmath::Point2((world.GetPlayer().Kills - 1) * 0.3f / (world.GetPlayer().MaxBulletKills) +
                                          10.0f * normalPixel.X,
                            -0.05f - 10.0f * normalPixel.Y));
        renderer.DrawRectangle(bulletBar, { 0.25f, 0.75f, 0.0f, 1.0f });
    }

    void Utility::DrawShortestPath(gamef::Renderer &renderer, logic::Enemy& enemy) {
        if (enemy.Path.empty())
            return;

        gmath::RGBA orange = { 1.0f, 0.65f, 0.0f, 1.0f };
        gmath::RGBA blue = { 0.0f, 0.5f, 1.0f, 1.0f };
        gmath::RGBA purple = { 0.4f, 0.0f, 1.0f, 1.0f };
        gmath::RGBA green = { 0.0f, 1.0f, 0.0f, 1.0f };
        gmath::RGBA kamikaze = { 1.0f, 0.3f, 0.3f, 1.0f };
        gamef::Renderer::Mode::Enum previousMode = renderer.GetRenderMode();
        renderer.SetRenderMode(gamef::Renderer::Mode::Enum::FloatingUI);
        const float temp = renderer.GetPrimaryAlpha();
        renderer.SetPrimaryAlpha(1.0f);

        for (int i = 0; i < enemy.Path.size(); i++) {
            const gmath::GridEntry &cell = enemy.Path.get(i);
            gmath::Coordinates position =
                gmath::GridEntry::ToWorld(cell.Row, cell.Column, logic::Grid::CellSize, logic::Grid::Origin);
            float format = logic::Grid::CellSize * (i == 0 ? 0.2f : 0.15f);
            position += (logic::Grid::CellSize - format) / 2.0f;
            gmath::Rectangle wall = { position, position + format };
            renderer.DrawRectangle(wall,
                i == 0 ? (enemy.IsPursueing() ? purple
                       : enemy.IsRecovering() ? green
                       : enemy.IsKamikazing() ? kamikaze
                                              : blue)
                       : (enemy.IsPursueing() ? orange
                       : enemy.IsRecovering() ? green
                       : enemy.IsKamikazing() ? kamikaze
                                              : blue));
        }
        renderer.SetPrimaryAlpha(temp);
        renderer.SetRenderMode(previousMode);
    }

    void Utility::DrawSteering(gamef::Renderer &renderer, logic::Enemy& enemy) {
        gmath::RGBA red = { 1.0f, 0.0f, 0.0f, 1.0f };
        gmath::RGBA orange = { 1.0f, 0.65f, 0.0f, 1.0f };
        gmath::RGBA green = { 0.0f, 1.0f, 0.0f, 1.0f };
        gmath::RGBA blue = { 0.0f, 0.5f, 1.0f, 1.0f };
        gmath::RGBA yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
        gmath::RGBA kamikaze = { 1.0f, 0.3f, 0.3f, 1.0f };

        gamef::Renderer::Mode::Enum previousMode = renderer.GetRenderMode();
        renderer.SetRenderMode(gamef::Renderer::Mode::Enum::FloatingUI);
        const float temp = renderer.GetPrimaryAlpha();
        renderer.SetPrimaryAlpha(1.0f);

        gmath::Vec2 direction_front = { enemy.GetNextPosition() - enemy.Position };
        float segmentLengthFront = enemy.Radius * 0.7f;
        float segmentLengthRear = enemy.Radius * 0.5f;
        gmath::Vec2 frontEndPosition = enemy.Position + direction_front * segmentLengthFront;

        float angle90 = 90.0f * (static_cast<float>(std::numbers::pi_v<float>) / 180.0f);
        float angle135 = 135.0f * (static_cast<float>(std::numbers::pi_v<float>) / 180.0f);
        gmath::Vec2 direction_front135 = direction_front.Rotate(angle135);
        gmath::Vec2 direction_front225 = direction_front.Rotate(angle90);
        gmath::Vec2 frontEndPosition135 = enemy.Position + direction_front135 * segmentLengthRear;
        gmath::Vec2 frontEndPosition225 = enemy.Position + direction_front225 * segmentLengthRear;

        renderer.DrawSegment({ enemy.Position, frontEndPosition },
             enemy.IsRecovering()  ? green
            : enemy.IsAttacking()  ? red
            : enemy.IsPursueing()  ? orange
            : enemy.IsKamikazing() ? kamikaze
                                   : blue);
        renderer.DrawSegment({ enemy.Position, frontEndPosition135 },
             enemy.IsRecovering()  ? green
            : enemy.IsAttacking()  ? red
            : enemy.IsPursueing()  ? orange
            : enemy.IsKamikazing() ? kamikaze
                                   : blue);
        renderer.DrawSegment({ enemy.Position, frontEndPosition225 },
             enemy.IsRecovering()  ? green
            : enemy.IsAttacking()  ? red
            : enemy.IsPursueing()  ? orange
            : enemy.IsKamikazing() ? kamikaze
                                   : blue);

        gmath::Point2 intersectionFront{ enemy.Position };
        bool isIntersectingFront = logic::Grid::IsIntersectingAnyCell(
            { enemy.Position, frontEndPosition }, reinterpret_cast<const int *>(logic::Grid::Array), intersectionFront);

        gmath::Point2 intersectionFront135{ enemy.Position };
        bool isIntersectingFront135 = logic::Grid::IsIntersectingAnyCell(
            { enemy.Position, frontEndPosition135 }, reinterpret_cast<const int *>(logic::Grid::Array), intersectionFront135);

        gmath::Point2 intersectionFront225{ enemy.Position };
        bool isIntersectingFront225 = logic::Grid::IsIntersectingAnyCell(
            { enemy.Position, frontEndPosition225 }, reinterpret_cast<const int *>(logic::Grid::Array), intersectionFront225);

        float format = logic::Grid::CellSize * 0.05f;

        if (isIntersectingFront) {
            renderer.DrawCircle(intersectionFront, format, yellow);
        }

        if (isIntersectingFront135) {
            renderer.DrawCircle(intersectionFront135, format, yellow);
        }

        if (isIntersectingFront225) {
            renderer.DrawCircle(intersectionFront225, format, yellow);
        }

        renderer.SetPrimaryAlpha(temp);
        renderer.SetRenderMode(previousMode);
    }

    void Utility::DrawInterception(gamef::Renderer &renderer, logic::Enemy& enemy) {
        if (enemy.IsRecovering()) {
            gmath::RGBA green = { 0.0f, 1.0f, 0.0f, 1.0f };
            gmath::RGBA yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

            gamef::Renderer::Mode::Enum previousMode = renderer.GetRenderMode();
            renderer.SetRenderMode(gamef::Renderer::Mode::Enum::FloatingUI);
            const float temp = renderer.GetPrimaryAlpha();
            renderer.SetPrimaryAlpha(1.0f);

            gmath::Point2 intersection{ enemy.Position };

            float format = logic::Grid::CellSize * 0.05f;

            gmath::GridEntry enemy_output;
            gmath::GridEntry target_output;
            if (gmath::GridEntry::FromWorld(
                    enemy.Position, logic::Grid::Origin, logic::Grid::CellSize, logic::Grid::GridSize, enemy_output) &&
                gmath::GridEntry::FromWorld(
                    enemy.Target, logic::Grid::Origin, logic::Grid::CellSize, logic::Grid::GridSize, target_output)) {
                gmath::GridEntry enemy_grid = { enemy_output.Row, enemy_output.Column };
                gmath::GridEntry target_grid = { target_output.Row, target_output.Column };
                int row = abs(enemy_grid.Row - target_grid.Row);
                int column = abs(enemy_grid.Column - target_grid.Column);

                bool targetIsIntersecting = logic::Grid::IsIntersectingAnyCell({ enemy.Position, enemy.Target },
                    reinterpret_cast<const int *>(logic::Grid::Array),
                    intersection);

                if (targetIsIntersecting) {
                    renderer.DrawSegment({ enemy.Position, enemy.Target }, green);               
                    renderer.DrawCircle(intersection, format, yellow);
                }
            }
            renderer.SetPrimaryAlpha(temp);
            renderer.SetRenderMode(previousMode);
        }
    }

    void Utility::DrawDebug(gamef::Game &game, gamef::Renderer &renderer, logic::World &world, const bool path,
        const bool wander, const bool pursue, const bool recover, const bool kamikaze, const bool intercept) {
        gamef::Screen screen{ renderer.GetScreen() };
        renderer.DrawRectangle({ { -1.0f, -1.0f }, { -0.885, -0.85 } }, { 0.0f, 0.0f, 0.0f, 0.5f });
        renderer.DrawString({ { -0.98f, -0.98f }, { -0.9, -0.9 } }, std::to_wstring(game.GetFrameRate()));
        renderer.DrawString(
            { gmath::Point2(-0.98f, -0.98f) + gmath::Point2(0.0f, 24.0f) * screen.GetNormalPixel(), { -0.9, -0.9 } },
            std::to_wstring(game.GetUpdateRate()));
        renderer.DrawString(
            { gmath::Point2(-0.98f, -0.98f) + gmath::Point2(0.0f, 48.0f) * screen.GetNormalPixel(), { -0.9, -0.9 } },
            std::to_wstring(game.GetTickRate()));

        for (auto &enemy: world.GetEnemies()) {
            if (path)
                app::Utility::DrawShortestPath(renderer, enemy);
            if (wander && enemy.IsWandering())
                app::Utility::DrawShortestPath(renderer, enemy);
            if (pursue && enemy.IsPursueing())
                app::Utility::DrawShortestPath(renderer, enemy);
            if (recover && enemy.IsRecovering())
                app::Utility::DrawShortestPath(renderer, enemy);
            if (kamikaze && enemy.IsKamikazing())
                app::Utility::DrawShortestPath(renderer, enemy);
            if (intercept)
                app::Utility::DrawInterception(renderer, enemy);
            app::Utility::DrawSteering(renderer, enemy);
        }

        renderer.SetRenderMode(gamef::Renderer::Mode::Enum::World);
        renderer.SetRenderMode(gamef::Renderer::Mode::Enum::Debug);
    }
}