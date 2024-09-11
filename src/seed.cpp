#include "seed.h"
#include "tile.h"

namespace Seed {

grid_t load(std::string filepath) {
	grid_t grid;
	std::ifstream file(filepath);

	if (!file.is_open()) {
		std::cerr << "Error: Could not open the file " << filepath << std::endl;
		throw std::runtime_error("Failed to open file");
	}

	std::vector<std::vector<uint32_t> > temp_grid;
	std::string line;
	int max_width = 0;

	// Read file line by line
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string tile;
		int x, y;

		// Parse the line
		std::getline(iss, tile, ',');

		char comma;
        iss >> x >> comma >> y;

		// Ensure the grid is large enough
		if (y >= temp_grid.size()) {
			temp_grid.resize(y + 1);
		}
		if (x >= temp_grid[y].size()) {
			temp_grid[y].resize(x + 1);
		}

		// Set the tile value
		temp_grid[y][x] = Tile::encode(tile);

		std::cout  << "Seed Tile: " << tile << "\t" << temp_grid[y][x] << "\tx: " << x << "\ty: " << y << std::endl;

		// Track maximum width
		if (x + 1 > max_width) {
			max_width = x + 1;
		}
	}

	file.close();

	// Allocate and copy the grid data
	grid.width = max_width;
	grid.height = temp_grid.size();
	grid.tiles = new uint32_t[grid.width * grid.height]();

	for (int y = 0; y < grid.height; ++y) {
		for (int x = 0; x < grid.width; ++x) {
			grid.tiles[y * grid.width + x] = temp_grid[y][x];
		}
	}

	return grid;
}

// Optionally, add a function to free the allocated memory
void free_grid(grid_t& grid) {
    delete[] grid.tiles;
}
} // namespace Seed
