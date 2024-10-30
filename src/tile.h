#pragma once

#include "common.h"

typedef uint32_t tile_t;
namespace Tile {
    tile_t encode(const std::string& str);
    std::string decode(tile_t value);

    void lock(tile_t& tile);
    void unlock(tile_t& tile);
    bool is_locked(tile_t tile);
} // namespace Tile
