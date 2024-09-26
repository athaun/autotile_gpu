#pragma once

#include "common.h"

typedef uint32_t tile_t;
namespace Tile {
    tile_t encode(const std::string& str);
    std::string decode(uint32_t value);

    void lock(uint32_t& tile);
    void unlock(uint32_t& tile);
    bool is_locked(uint32_t tile);
} // namespace Tile
