#ifndef UTILITY_H
#define UTILITY_H

#include <Windows.h>
#include <iostream>
#include <numbers>

#include "enemy.h"
#include "arrow.h"
#include "game.h"
#include "gmath.h"
#include "graph.h"
#include "grid.h"
#include "player.h"
#include "renderer.h"
#include "stack.h"
#include "world.h"

namespace app {
    class Utility {
    public:
        static void DrawGrid(gamef::Renderer &renderer);
        static void DrawRaster(gamef::Renderer &renderer);
        static void DrawPlayerIndicators(gamef::Renderer &renderer, logic::World& world);
        static void DrawShortestPath(gamef::Renderer &renderer, logic::Enemy& enemy);
        static void DrawSteering(gamef::Renderer &renderer, logic::Enemy& enemy);
        static void DrawInterception(gamef::Renderer &renderer, logic::Enemy& enemy);
        static void DrawDebug(gamef::Game &game, gamef::Renderer &renderer, logic::World &world, const bool path,
            const bool wander, const bool pursue, const bool recover, const bool kamikaze, const bool intercept);
    };
}
#endif
