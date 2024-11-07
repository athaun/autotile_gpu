#include "common.h"

namespace Tile {
uint32_t encode(const std::string& str) {
	uint32_t result = 0;
	for (size_t i = 0; i < str.length() && i < sizeof(uint32_t); i++) {
		result |= (uint8_t)str[i] << (i * 8); // Pack each character
	}
	return result;
}

std::string decode(uint32_t value) {
	if (value == Rules::EMPTY_TILE) return "[__]";
	std::string result;
	for (size_t i = 0; i < sizeof(uint32_t); i++) {
		result += (char)(value >> (i * 8)); // Unpack each character
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
