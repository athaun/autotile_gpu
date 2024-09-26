#pragma once

#include "common.h"

namespace Seed {
    struct grid_t {
        tile_t* tiles;
        int width;
        int height;
    };

    grid_t load(std::string filepath);
}
