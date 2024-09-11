#include <cstdint>
#include <string>

namespace Tile {
    uint32_t encode(const std::string& str);
    std::string decode(uint32_t value);
} // namespace Tile
