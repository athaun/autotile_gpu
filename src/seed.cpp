#include "common.h"

namespace Seed {

grid_t load(std::string filepath) {
	grid_t grid;
	std::ifstream file(filepath);

	if (!file.is_open()) {
		std::cerr << "Error: Could not open the file " << filepath << std::endl;
		throw std::runtime_error("Failed to open file");
	}

	std::vector<std::vector<uint32_t>> temp_grid;
	std::string line;
	int max_width = 0;

	// Read file line by line
	while (std::getline(file, line)) {
		std::istringstream iss(line);

		std::string tile;
		int x, y;

		// Parse the line
		std::getline(iss, tile, ',');

		// Skip empty lines
		if (tile.empty()) continue;

        // Skip lines beginning with #
        if (tile[0] == '#') continue;

		char comma;
        iss >> x >> comma >> y;

		// Ensure the grid is large enough
		if (y >= temp_grid.size()) {
			temp_grid.resize(y + 1);
			temp_grid.resize(y + 1, std::vector<tile_t>(max_width, Rules::EMPTY_TILE));
			
		}
		if (x >= temp_grid[y].size()) {
			temp_grid[y].resize(x + 1, Rules::EMPTY_TILE);
		}

		// Set the tile value
		temp_grid[y][x] = Tile::encode(tile);

		std::cout  << "Seed Tile: " << tile << "\t" << temp_grid[y][x] << "\tx: " << x << "\ty: " << y << std::endl;

		// Track maximum width
		if (x >= max_width) {
			max_width = x + 1;
		}
	}

	std::cout << "\nFinished loading seed, size: " << max_width << "x" << temp_grid.size() << "\n\n";

	file.close();

	int original_width = max_width;
    int original_height = temp_grid.size();
    int padding = 2 * std::max(original_width, original_height);
	int padded_width = original_width + 2 * padding;
	int padded_height = original_height + 2 * padding;

    // Allocate the new grid with padding
    grid.width = padded_width;
    grid.height = padded_height;
    grid.tiles = new tile_t[grid.width * grid.height]();
	// Initialize the grid with a specific tile value (e.g., 0 for empty)
	std::fill(grid.tiles, grid.tiles + grid.width * grid.height, Rules::EMPTY_TILE);

    // Copy the existing data into the new grid with padding
    for (int y = 0; y < original_height; y++) {
        for (int x = 0; x < original_width; x++) {
            grid.tiles[(y + padding) * grid.width + (x + padding)] = temp_grid[y][x];
        }
    }

	return grid;
}

// Optionally, add a function to free the allocated memory
void free_grid(grid_t& grid) {
    delete[] grid.tiles;
}
} // namespace Seed
