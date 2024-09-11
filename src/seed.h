#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Seed {
    struct grid_t {
        uint32_t* tiles;
        int width;
        int height;
    };

    grid_t load(std::string filepath);
}
