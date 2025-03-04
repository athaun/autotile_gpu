#include "common.h"

namespace Tile {
std::unordered_map<tile_t, std::string> name_keys = {};

uint32_t encode(const std::string& str) {
	uint32_t result = 0;
	for (size_t i = 0; i < str.length() && i < sizeof(uint32_t); i++) {
		result |= (uint8_t)str[i] << (i * 8); // Pack each character
	}
	return result;
}

std::string name(tile_t tile) {
    if (name_keys.find(tile) != name_keys.end()) {
        return name_keys[tile];
    }
    return "[______]";
}

std::string decode(uint32_t value) {
	if (value == Rules::EMPTY_TILE) return "[__]";
	std::string result;
	for (size_t i = 0; i < sizeof(uint32_t); i++) {
		result += (char)(value >> (i * 8)); // Unpack each character
	}
	while (result.length() < 4) {
        result.push_back(' '); // Pad with spaces to ensure 4 characters
    }
	return result;
}

void lock(uint32_t& tile) {
    tile |= 0x80000000; // Set the MSB to 1 to mark as locked
}

void unlock(uint32_t& tile) {
    tile &= ~0x80000000; // Set the MSB to 0 to mark as unlocked
}

bool is_locked(uint32_t tile) {
    return (tile & 0x80000000) != 0;
}
} // namespace Tile
