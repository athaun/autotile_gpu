#pragma once

#include "common.h"
#include <unordered_map>

typedef uint32_t tile_t;

namespace Tile {
    extern std::unordered_map<tile_t, std::string> name_keys;

    tile_t encode(const std::string& str);
    std::string decode(tile_t value); // Decode a tile_t value to a 4 character string
    std::string name(tile_t tile); // Get the original system name of a tile based on .key file

    void lock(tile_t& tile);
    void unlock(tile_t& tile);
    bool is_locked(tile_t tile);
} // namespace Tile
