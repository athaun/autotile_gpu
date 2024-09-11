#include "tile.h"

namespace Tile {
uint32_t encode(const std::string& str) {
	uint32_t result = 0;
	for (size_t i = 0; i < str.length() && i < sizeof(uint32_t); i++) {
		result |= (uint8_t)str[i] << (i * 8); // Pack each character
	}
	return result;
}

std::string decode(uint32_t value) {
	std::string result;
	for (size_t i = 0; i < sizeof(uint32_t); i++) {
		result += (char)(value >> (i * 8)); // Unpack each character
	}
	return result;
}
} // namespace Tile
