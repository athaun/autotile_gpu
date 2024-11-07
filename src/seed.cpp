#include "common.h"

namespace Seed {

grid_t load(std::string filepath) {
	grid_t grid;
	std::ifstream file(filepath);

	if (!file.is_open()) {
		std::cerr << "Error: Could not open the file " << filepath << std::endl;
		throw std::runtime_error("Failed to open file");
	}

	int min_x = std::numeric_limits<int>::max();
	int min_y = std::numeric_limits<int>::max();
	int max_x = 0;
	int max_y = 0;
	std::vector<std::tuple<int, int, std::string>> seed_data;

	std::string line;

	// Read file line by line to find bounds
	while (std::getline(file, line)) {
		std::istringstream iss(line);

		std::string tile;
		int x, y;

		// Parse the line
		std::getline(iss, tile, ',');

		if (tile[0] == '#' || tile.empty()) continue;

		char comma;
		iss >> x >> comma >> y;

		seed_data.emplace_back(x, y, tile);

		// Track bounds
		if (x < min_x) min_x = x;
		if (y < min_y) min_y = y;
		if (x > max_x) max_x = x;
		if (y > max_y) max_y = y;
	}

	file.close();

	int original_width = max_x - min_x + 1;
	int original_height = max_y - min_y + 1;
	int padding = 2 * std::max(original_width, original_height);
	int padded_width = original_width + 2 * padding;
	int padded_height = original_height + 2 * padding;

	// Allocate the new grid with padding
	grid.width = padded_width;
	grid.height = padded_height;
	grid.tiles = new tile_t[grid.width * grid.height]();
	// Initialize the grid with a specific tile value (e.g., 0 for empty)
	std::fill(grid.tiles, grid.tiles + grid.width * grid.height, Rules::EMPTY_TILE);

	// Copy the seed data into the new grid with padding
	for (const auto& [x, y, tile] : seed_data) {
		grid.tiles[(y - min_y + padding) * grid.width + (x - min_x + padding)] = Tile::encode(tile);
		std::cout << "Seed Tile: " << tile << "\t" << Tile::encode(tile) << "\tx: " << x << "\ty: " << y << std::endl;
	}

	return grid;
}

// Optionally, add a function to free the allocated memory
void free_grid(grid_t& grid) {
    delete[] grid.tiles;
}
} // namespace Seed
