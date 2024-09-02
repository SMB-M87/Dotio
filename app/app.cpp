#include "utility.h"

auto world = std::make_unique<logic::World>();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
    world->Initialize();
    gmath::Point2 worldReticle{};
    gmath::Coordinates cursor{};
    gamef::Game instance(hInstance, 20.0f, 60.0f, 1920.0f);
    bool debug = false, path = false, wander = false, pursue = false, recover = false, kamikaze = false, intercept = false, ghost = false, pauze = false;

    instance.OnCreate = [&](gamef::Game &game) {};

    instance.OnUpdate = [&](gamef::Game &game) {
        const float delta = game.GetDelta();

        if (!ghost && !pauze) {
            world->GetPlayer().Update(delta);
            logic::Grid::HandleGridCollision(world->GetPlayer(), delta, reinterpret_cast<const int*>(logic::Grid::Array));
            world->GetGhost().Position = world->GetPlayer().Position;
        }
        else
            world->GetGhost().Update(delta);

        game.GetRenderer().Camera = ghost ? world->GetGhost().Position : world->GetPlayer().Position;

        if (!pauze) {
            world->UpdateEnemies(delta, debug);
            world->UpdateBolts(delta);
            world->UpdateArrows(delta, debug);
        }
    };

    instance.OnTick = [&](gamef::Game &game) {
        if (!pauze) {
            for (auto &enemy: world->GetEnemies()) {
                enemy.Tick();
            }
        }
    };

    instance.OnRenderWorld = [&worldReticle, &cursor, &ghost](gamef::Game &game, gamef::Renderer &renderer) {
        worldReticle = game.GetMouse().GetWorldSpaceCoordinates();
        renderer.Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
        app::Utility::DrawGrid(renderer);
        cursor = game.GetMouse().GetWorldSpaceCoordinates();
        const gmath::Size2 offset{ fmodf(logic::Grid::Origin.X, logic::Grid::CellSize), fmodf(logic::Grid::Origin.Y, logic::Grid::CellSize) };
        cursor.X = floorf((cursor.X - offset.X) / logic::Grid::CellSize) * logic::Grid::CellSize + offset.X;
        cursor.Y = floorf((cursor.Y - offset.Y) / logic::Grid::CellSize) * logic::Grid::CellSize + offset.Y;

        world->Draw(renderer);

        if (ghost)
            world->GetGhost().Draw(renderer);
    };

    instance.OnRenderFloatingUI = [](gamef::Game &game, gamef::Renderer &renderer) {};

    instance.OnRenderUI = [](gamef::Game &game, gamef::Renderer &renderer) {
        app::Utility::DrawPlayerIndicators(renderer, *world);
    };

    instance.OnRenderDebug = [&path, &wander, &pursue, &recover, &kamikaze, &intercept](gamef::Game &game, gamef::Renderer &renderer) {
        app::Utility::DrawRaster(renderer);
        app::Utility::DrawDebug(game, renderer, *world, path, wander, pursue, recover, kamikaze, intercept);
    };

    instance.OnKeyDown = [&cursor, &debug, &path, &wander, &pursue, &recover, &kamikaze, &intercept, &ghost, &pauze](const gamef::Game::KeyEventArgs &args) {
        gamef::Game &game{ args.GetGame() };
        gamef::Renderer &renderer{ game.GetRenderer() };

        switch (args.GetVirtualKeyCode()) {
            case gamef::Game::VirtualKey::Escape: {
                game.Quit();
                break;
            }

            case gamef::Game::VirtualKey::Tilde: {
                if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::Shift)) {
                    debug = !debug;
                    renderer.ToggleDebug();
                }
                break;
            }

            case gamef::Game::VirtualKey::Space: {
                if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::Shift)) {
                    world->Reset();
                } else {
                    pauze = !pauze;
                }
                break;
            }

            case gamef::Game::VirtualKey::One: {
                if (debug) {
                    path = !path;
                    wander = false;
                    pursue = false;
                    recover = false;
                }
                break;
            }

            case gamef::Game::VirtualKey::Two: {
                if (debug) {
                    wander = !wander;
                    path = false;
                }
                break;
            }   

            case gamef::Game::VirtualKey::Three: {
                if (debug) {
                    pursue = !pursue;
                    path = false;
                }
                break;
            }

            case gamef::Game::VirtualKey::Four: {
                if (debug) {
                    kamikaze = !kamikaze;
                    path = false;
                }
                break;
            }

            case gamef::Game::VirtualKey::Five: {
                if (debug) {
                    recover = !recover;
                    path = false;
                }
                break;
            }

            case gamef::Game::VirtualKey::Six: {
                if (debug) {
                    intercept = !intercept;
                }
                break;
            }

            case gamef::Game::VirtualKey::G: {
                if (debug && gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::Shift)) {
                    ghost = !ghost;
                    world->GetPlayer().Position = world->GetGhost().Position;
                } else if (debug) {
                    ghost = !ghost;
                    world->GetGhost().Position = world->GetPlayer().Position;
                }
                break;
            }

            case gamef::Game::VirtualKey::V: {
                if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::Shift)) {
                    float &zoom = game.GetRenderer().Zoom;
                    zoom -= 0.1f;

                    if (zoom < 0.1f)
                        zoom = 0.1f;

                    world->GetGhost().Speed = 1.0f / zoom;
                } else {
                    float &zoom = game.GetRenderer().Zoom;
                    zoom += 0.1f;

                    if (zoom > 2.0f)
                        zoom = 2.0f;

                    world->GetGhost().Speed = 1.0f / zoom;
                }
                break;
            }

            case gamef::Game::VirtualKey::W:
            case gamef::Game::VirtualKey::S:
            case gamef::Game::VirtualKey::ArrowUp:
            case gamef::Game::VirtualKey::ArrowDown: {
                if (!args.WasKeyDown()) {
                    world->GetPlayer().Heading.Y = 0.0f;
                    if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::S) ||
                        gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowDown))
                        world->GetPlayer().Heading.Y += 1.0f;

                    if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::W) ||
                        gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowUp))
                        world->GetPlayer().Heading.Y -= 1.0f;
                }
                if (ghost)
                    world->GetGhost().Heading = world->GetPlayer().Heading;
                break;
            }

            case gamef::Game::VirtualKey::A:
            case gamef::Game::VirtualKey::D:
            case gamef::Game::VirtualKey::ArrowLeft:
            case gamef::Game::VirtualKey::ArrowRight: {
                if (!args.WasKeyDown()) {
                    world->GetPlayer().Heading.X = 0.0f;
                    if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::D) ||
                        gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowRight))
                        world->GetPlayer().Heading.X += 1.0f;

                    if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::A) ||
                        gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowLeft))
                        world->GetPlayer().Heading.X -= 1.0f;
                }
                if (ghost)
                    world->GetGhost().Heading = world->GetPlayer().Heading;
                break;
            }

            default:
                break;
        }
    };

    instance.OnKeyUp = [&ghost](const gamef::Game::KeyEventArgs &args) {
        gamef::Game &game{ args.GetGame() };
        gamef::Renderer &renderer{ game.GetRenderer() };

        switch (args.GetVirtualKeyCode()) {
            case gamef::Game::VirtualKey::W:
            case gamef::Game::VirtualKey::S:
            case gamef::Game::VirtualKey::ArrowUp:
            case gamef::Game::VirtualKey::ArrowDown: {
                world->GetPlayer().Heading.Y = 0.0f;
                if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::S) ||
                    gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowDown))
                    world->GetPlayer().Heading.Y += 1.0f;

                if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::W) ||
                    gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowUp))
                    world->GetPlayer().Heading.Y -= 1.0f;

                if (ghost)
                    world->GetGhost().Heading = world->GetPlayer().Heading;
                break;
            }

            case gamef::Game::VirtualKey::A:
            case gamef::Game::VirtualKey::D:
            case gamef::Game::VirtualKey::ArrowLeft:
            case gamef::Game::VirtualKey::ArrowRight: {
                world->GetPlayer().Heading.X = 0.0f;
                if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::D) ||
                    gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowRight))
                    world->GetPlayer().Heading.X += 1.0f;

                if (gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::A) ||
                    gamef::Game::VirtualKey::IsDown(gamef::Game::VirtualKey::ArrowLeft))
                    world->GetPlayer().Heading.X -= 1.0f;

                if (ghost)
                    world->GetGhost().Heading = world->GetPlayer().Heading;
                break;
            }

            default:
                break;
        }
    };

    instance.OnMouseMove = [&worldReticle](const gamef::Game::MouseMoveEventArgs &args) {
        gamef::Game &game{ args.GetGame() };
        game.GetMouse().Update(game.GetRenderer(), args.GetActualCoordinates());
    };

    instance.OnLeftButtonHold = [&worldReticle, &debug](gamef::Game &game) {
        world->GetPlayer().Shoot(game.GetMouse().GetWorldSpaceCoordinates(), debug);
    };

    instance.OnLeftButtonDown = [&worldReticle](const gamef::Game::MouseClickEventArgs &args) {
        gamef::Game &game{ args.GetGame() };
    };

    instance.OnLeftButtonUp = [&worldReticle](const gamef::Game::MouseClickEventArgs &args) {
        gamef::Game &game{ args.GetGame() };
    };

    instance.Start();

    return 0;
}
