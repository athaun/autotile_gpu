#include "common.h"
#include "shader/my_shader.hpp"
#include "simulator.h"

namespace Simulator {
Rules::Rules<rule_t> horizontal_transitions;
Rules::Rules<rule_t> vertical_transitions;
Seed::grid_t grid;

void DeltaBuffer::push(delta_t delta) {
	if (count < MAX_DELTAS) {
		deltas[count++] = delta;
	} else {
		// log_deltas(*this);
		count = 0;
	}
}

DeltaBuffer delta_buffer;

void init(std::string horizontal_transitions_path, std::string vertical_transitions_path, std::string seed_path) {
	horizontal_transitions = Rules::load(horizontal_transitions_path);
	vertical_transitions = Rules::load(vertical_transitions_path);
	grid = Seed::load(seed_path);
}

void init(std::string system_name) {
    system_name = "input/" + system_name;
    init(system_name + ".hrules", system_name + ".vrules", system_name + ".seed");
}

void print_grid() {
    std::cout << "\n";
    for (int y = 0; y < grid.height; y++) {
        for (int x = 0; x < grid.width; x++) {
			if (grid.tiles[x + y * grid.width] == Rules::EMPTY_TILE) {
				std::cout << "EMPTY\t";
				continue;
			}
            std::cout << Tile::decode(grid.tiles[x + y * grid.width]) << "\t";
        }
        std::cout << "\n";
    }
}

void log_deltas(DeltaBuffer& buffer) {
    if (buffer.count == 0) return;

    std::cout << "\n========================\n\nStep deltas: " << buffer.count << "\n";
	for (size_t i = 0; i < buffer.count; i++) {
		const delta_t& delta = buffer.deltas[i];
		std::cout << "(" << delta.location_a.x << ", " << delta.location_a.y << ", "
				  << (delta.dir == delta_t::Direction::HORIZONTAL ? "H" : "V") << ")\t"
				  << Tile::decode(delta.before.tile_a) << " + " << Tile::decode(delta.before.tile_b)
				  << " -> " << Tile::decode(delta.after.tile_a) << " + " << Tile::decode(delta.after.tile_b) << "\n";
	}

	print_grid();
}

void apply_deltas(DeltaBuffer& buffer) {
    if (buffer.count == 0) {
        exit(0);
    }

	for (size_t i = 0; i < buffer.count; i++) {
		const delta_t& delta = buffer.deltas[i];

		int direction_offset = delta.dir == delta_t::Direction::HORIZONTAL ? 1 : grid.width;

		tile_t& tile_a = grid.tiles[delta.location_a.x + delta.location_a.y * grid.width];
		tile_t& tile_b = grid.tiles[delta.location_a.x + delta.location_a.y * grid.width + direction_offset];

		// This assignment will unlock the tile by overwriting the lock bit
		tile_a = delta.after.tile_a;
		tile_b = delta.after.tile_b;
	}

	// Print the grid
	std::cout << "\nApplied deltas:";
	print_grid();

	buffer.count = 0;
}

loc_t neighborhood[4] = {
    { 1, 0 }, // Right
    { 0, 1 }, // Down
    { -1, 0 }, // Left
    { 0, -1 }, // Up
};

void check_attachment(tile_t& tile_a, const loc_t& location, std::vector<delta_t>& possible_deltas) {
	for (int dir = 0; dir < 4; dir++) {
		Rules::Rules& rules = (dir % 2 == 0) ? horizontal_transitions : vertical_transitions;

		loc_t tile_a_location = { location.x, location.y };
		loc_t tile_b_location = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

		// Check if the neighbor location is out of bounds
		if (tile_b_location.x < 0 || tile_b_location.x >= grid.width || tile_b_location.y < 0 || tile_b_location.y >= grid.height) {
			continue;
		}

		tile_t& tile_a = tile_a_location.get(grid);
		tile_t& tile_b = tile_b_location.get(grid);
		
		if (Tile::is_locked(tile_a) || Tile::is_locked(tile_b)) continue;

		// Rules::rule_t rule = rules.find(tile_a, tile_b);
		// if (!Rules::is_valid(rule)) continue;

		// Create a new tile from A-E and attach it
		std::string new_tile(1, 'A' + (rand() % 5));

		// In the future, check temperature here.
		
		// Tile::lock(grid.tiles[tile_a_location.x + tile_a_location.y * grid.width]);
		// Tile::lock(grid.tiles[tile_b_location.x + tile_b_location.y * grid.width]);

		delta_t delta = {
			{ tile_a, tile_b },
			{ tile_a, Tile::encode(new_tile) },
			tile_a_location,
			static_cast<delta_t::Direction>(dir % 2)
		};

		possible_deltas.push_back(delta);
	}
}

void check_transitions(tile_t& tile_a, const loc_t& location, std::vector<delta_t>& possible_deltas) {
    for (int dir = 0; dir < 2; dir++) {
		Rules::Rules& transitions = dir == 1 ? vertical_transitions : horizontal_transitions;

        if (Tile::is_locked(tile_a)) return;

    	loc_t neighbor = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

        // Check if the neighbor is out of bounds
    	if (neighbor.x < 0 || neighbor.x >= grid.width || neighbor.y < 0 || neighbor.y >= grid.height) {
    		continue;
    	}

    	tile_t tile_b = neighbor.get(grid);
    	Rules::transition_t transition = rules.find(tile_a, tile_b);

    	// if (Tile::is_locked(tile_b) || !Rules::is_valid(rule)) continue;

        Tile::lock(grid.tiles[location.x + location.y * grid.width]);
    	Tile::lock(grid.tiles[neighbor.x + neighbor.y * grid.width]);

    	delta_t delta = {
    		{ tile_a, tile_b },
    		{ transition.to_a, transition.to_b },
    		location,
    		static_cast<delta_t::Direction>(dir)
    	};

    	possible_deltas.push_back(delta);
    }
}

/*
 * Non-deterministically choose a delta from the list of
 * possible deltas generated from attachment or transition rules
*/
void choose_delta(std::vector<delta_t>& possible_deltas) {
    if (possible_deltas.empty()) return;

   	delta_buffer.push(possible_deltas[rand() % possible_deltas.size()]);

	for (delta_t& delta : possible_deltas) {
		Tile::unlock(delta.before.tile_a);
		Tile::unlock(delta.before.tile_b);
	}

    possible_deltas.clear();
}

void run_serial() {
	std::vector<delta_t> possible_deltas = std::vector<delta_t>();
    srand(time(NULL));

	int ticks = 0;
	while (true) {
	    // Randomly select an X and Y coordinate in the grid
		loc_t location = { rand() % grid.width, rand() % grid.height };
		tile_t tile_a = grid.tiles[location.x + location.y * grid.width];

		if (tile_a == Rules::EMPTY_TILE) {
			check_attachment(tile_a, location, possible_deltas);
		} else {
		    check_transitions(tile_a, location, possible_deltas);
		}

		choose_delta(possible_deltas);

		if (++ticks % 100 == 0) {
			log_deltas(delta_buffer);
			apply_deltas(delta_buffer);
		}
	}
}

void run_parallel() {
	kp::Manager mgr;

	std::shared_ptr<kp::TensorT<float> > tensorInA = mgr.tensor({ 2.0, 4.0, 6.0 });
	std::shared_ptr<kp::TensorT<float> > tensorInB = mgr.tensor({ 0.0, 1.0, 2.0 });
	std::shared_ptr<kp::TensorT<float> > tensorOut = mgr.tensor({ 0.0, 0.0, 0.0 });

	const std::vector<std::shared_ptr<kp::Tensor> > params = { tensorInA, tensorInB, tensorOut };

	const std::vector<uint32_t> shader = std::vector<uint32_t>(shader::MY_SHADER_COMP_SPV.begin(), shader::MY_SHADER_COMP_SPV.end());
	std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(params, shader);

	mgr.sequence()
		->record<kp::OpTensorSyncDevice>(params)
		->record<kp::OpAlgoDispatch>(algo)
		->record<kp::OpTensorSyncLocal>(params)
		->eval();

	// prints "Output {  0  4  12  }"
	std::cout << "Output: {  ";
	for (const float& elem : tensorOut->vector()) {
		std::cout << elem << "  ";
	}
	std::cout << "}" << std::endl;

	if (tensorOut->vector() != std::vector<float>{ 0, 4, 12 }) {
		throw std::runtime_error("Result does not match");
	}
}
} // namespace Simulator
