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

    // Define new, larger grid dimensions
    int expanded_size = 20 * std::max(original_width, original_height); // Expand by 4x
    int new_width = expanded_size;
    int new_height = expanded_size;

    // Compute centering offsets
    int offset_x = (new_width - original_width) / 2;
    int offset_y = (new_height - original_height) / 2;

    // Allocate the new grid
    grid.width = new_width;
    grid.height = new_height;
    grid.tiles = new tile_t[grid.width * grid.height]();
    std::fill(grid.tiles, grid.tiles + grid.width * grid.height, Rules::EMPTY_TILE);

	// Copy the seed data into the new grid with centering
    for (const auto& [x, y, tile] : seed_data) {
        int new_x = (x - min_x) + offset_x;
        int new_y = (y - min_y) + offset_y;
        grid.tiles[new_y * grid.width + new_x] = Tile::encode(tile);
        std::cout << "Seed Tile: " << tile << "\tEncoded: " << Tile::encode(tile) 
                  << "\tNew Pos: (" << new_x << ", " << new_y << ")" << std::endl;
    }

	return grid;
}

// Optionally, add a function to free the allocated memory
void free_grid(grid_t& grid) {
    delete[] grid.tiles;
}
} // namespace Seed
