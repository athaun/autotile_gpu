#include "common.h"
#include "shader/my_shader.hpp"
#include "simulator.h"
#include <thread>

namespace Simulator {
Rules::Rules<Rules::transition_t> horizontal_transitions;
Rules::Rules<Rules::transition_t> vertical_transitions;
Rules::Rules<Rules::affinity_t> horizontal_affinities;
Rules::Rules<Rules::affinity_t> vertical_affinities;
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

void init(std::string system_name) {
    system_name = "input/" + system_name;

	grid = Seed::load(system_name + ".seed");

	Rules::load(system_name + ".htrans", horizontal_transitions);
    Rules::load(system_name + ".vtrans", vertical_transitions);

    Rules::load(system_name + ".haff", horizontal_affinities);
    Rules::load(system_name + ".vaff", vertical_affinities);
}

void print_grid() {
    std::cout << "\n";
    for (int y = 0; y < grid.height; y++) {
        for (int x = 0; x < grid.width; x++) {
			if (grid.tiles[x + y * grid.width] == Rules::EMPTY_TILE) {
				std::cout << "[__] ";
				continue;
			}
            std::cout << Tile::decode(grid.tiles[x + y * grid.width]) << " ";
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
				  << Tile::decode(delta.before.tile_a) << " + " << Tile::decode(delta.before.tile_b)
				  << " -> " << Tile::decode(delta.after.tile_a) << " + " << Tile::decode(delta.after.tile_b) << " " << (delta.type == delta_t::Type::ATTACHMENT ? "(Attachment)" : "(Transition)") << "\n";
	}

	print_grid();
}

void apply_deltas(DeltaBuffer& buffer) {
    if (buffer.count == 0) {
        // exit(0);
		return;
    }

    std::ofstream delta_file("deltas.txt", std::ios::app);  // Open in append mode
	if (!delta_file.is_open()) {
        std::cerr << "Error: Could not open file for writing deltas." << std::endl;
        return;
    }

	for (size_t i = 0; i < buffer.count; i++) {
		const delta_t& delta = buffer.deltas[i];

		tile_t& tile_a = grid.tiles[delta.location_a.x + delta.location_a.y * grid.width];
		tile_t& tile_b = grid.tiles[delta.location_b.x + delta.location_b.y * grid.width];

		// This assignment will unlock the tile by overwriting the lock bit
		tile_a = delta.after.tile_a;
		tile_b = delta.after.tile_b;

		delta_file << "(" << delta.location_a.x << ", " << delta.location_a.y << ") "
				  << Tile::decode(delta.before.tile_a) << "+" << Tile::decode(delta.before.tile_b)
				  << "->" << Tile::decode(delta.after.tile_a) << "+" << Tile::decode(delta.after.tile_b) << " " << (delta.type == delta_t::Type::ATTACHMENT ? "(Attachment)" : "(Transition)") << "\n";
	}

	delta_file.close();

	// Print the grid
	std::cout << "\nApplied deltas:";
	print_grid();

	// sleep for a bit
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	buffer.count = 0;
}

loc_t neighborhood[4] = {
    { 1, 0 }, // Right
    { 0, 1 }, // Down
    { -1, 0 }, // Left
    { 0, -1 }, // Up
};

void check_attachment(tile_t& existing_tile, const loc_t& location, std::vector<delta_t>& possible_deltas) {
    for (int dir = 0; dir < 4; dir++) {

        // Select the appropriate set of affinities based on direction
        Rules::Rules<Rules::affinity_t>& affinities = (dir % 2 == 0) ? horizontal_affinities : vertical_affinities;

        loc_t existing_tile_location = { location.x, location.y };
        loc_t new_tile_location = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

        // Check if the neighbor location is out of bounds
        if (new_tile_location.x < 0 || new_tile_location.x >= grid.width || new_tile_location.y < 0 || new_tile_location.y >= grid.height) {
            continue;
        }

        // Get the tile at the neighbor location
        tile_t new_tile = new_tile_location.get(grid);

        // If the neighboring tile is empty, skip
        if (new_tile == Rules::EMPTY_TILE) continue;

        // Determine the correct orientation based on direction and affinity
        Rules::affinity_t affinity;
		tile_pair_t new_pair;
        if (dir == 0 || dir == 1) {
			affinity = affinities.find(existing_tile, new_tile);
			new_pair = { affinity.tile_b, affinity.tile_a };
		} else if (dir == 2 || dir == 3) {
			affinity = affinities.find(new_tile, existing_tile);
			new_pair = { affinity.tile_a, affinity.tile_b };
		}

        // If the affinity is invalid, skip this iteration
        if (Rules::is_invalid(affinity)) continue;

        // Lock the tiles
        Tile::lock(grid.tiles[location.x + location.y * grid.width]);
        Tile::lock(grid.tiles[new_tile_location.x + new_tile_location.y * grid.width]);

        // Construct the delta for attachment
        delta_t delta = {
            { existing_tile, new_tile },
            new_pair,
            new_tile_location,
            existing_tile_location,
            delta_t::Type::ATTACHMENT
        };

        // Add the valid delta to the list of possible deltas
        possible_deltas.push_back(delta);
    }
}

// void check_attachment(tile_t& tile_a, const loc_t& location, std::vector<delta_t>& possible_deltas) {
// 	for (int dir = 0; dir < 4; dir++) {

// 		Rules::Rules<Rules::affinity_t>& affinities = (dir % 2 == 0) ? horizontal_affinities : vertical_affinities;

// 		loc_t tile_a_location = { location.x, location.y };
// 		loc_t tile_b_location = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

// 		// Check if the neighbor location is out of bounds
// 		if (tile_b_location.x < 0 || tile_b_location.x >= grid.width || tile_b_location.y < 0 || tile_b_location.y >= grid.height) {
// 			continue;
// 		}

// 		tile_t tile_b = tile_b_location.get(grid);

// 		if (tile_b == Rules::EMPTY_TILE) continue;

// 		Rules::affinity_t affinity = affinities.find(tile_b, tile_a);

// 		// std::cout << "Checking attachment: " << Tile::decode(tile_b) << " : " << Tile::decode(affinity.tile_a) << " + " << Tile::decode(affinity.tile_b) << "\n";
		
// 		if (Rules::is_invalid(affinity)) continue;

// 		Tile::lock(grid.tiles[location.x + location.y * grid.width]);
// 		Tile::lock(grid.tiles[tile_b_location.x + tile_b_location.y * grid.width]);

// 		delta_t delta = {
// 			{ tile_a, tile_b },
// 			{ affinity.tile_a, affinity.tile_b },
// 			tile_b_location,
// 			tile_a_location,
// 			delta_t::Type::ATTACHMENT
// 		};
		
// 		// std::cout << "Possible attachment: " << Tile::decode(tile_a) << " + " << Tile::decode(tile_b) << " -> " << Tile::decode(affinity.tile_a) << " + " << Tile::decode(affinity.tile_b) << "\n";

// 		possible_deltas.push_back(delta);
// 	}
// }

void check_transitions(tile_t& tile_a, const loc_t& location, std::vector<delta_t>& possible_deltas) {
	if (Tile::is_locked(tile_a)) return;

    for (int dir = 0; dir < 2; dir++) {

		Rules::Rules<Rules::transition_t>& transitions = dir == 1 ? vertical_transitions : horizontal_transitions;

    	loc_t neighbor = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

        // Check if the neighbor is out of bounds
    	if (neighbor.x < 0 || neighbor.x >= grid.width || neighbor.y < 0 || neighbor.y >= grid.height) {
    		continue;
    	}

    	tile_t tile_b = neighbor.get(grid);

		if (Tile::is_locked(tile_b)) continue;

    	Rules::transition_t transition = transitions.find(tile_a, tile_b);

    	if (Rules::is_invalid(transition)) continue;

        Tile::lock(grid.tiles[location.x + location.y * grid.width]);
    	Tile::lock(grid.tiles[neighbor.x + neighbor.y * grid.width]);

    	delta_t delta = {
    		{ tile_a, tile_b },
    		{ transition.to_a, transition.to_b },
    		location,
    		neighbor,
			delta_t::Type::TRANSITION
    	};

		// std::cout << "Possible transition: " << Tile::decode(tile_a) << " + " << Tile::decode(tile_b) << " -> " << Tile::decode(transition.to_a) << " + " << Tile::decode(transition.to_b) << "\n";

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

	std::cout << "Initial grid state:" << std::endl;
	print_grid();

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

		// std::cout << vertical_affinities.find_index(Tile::encode("DhIR"), Tile::encode("XTNi")) << " - vertical affinities\n";
		// std::cout << horizontal_affinities.find_index(Tile::encode("DhIR"), Tile::encode("XTNi")) << " - horizontal affinities\n";
		// std::cout << vertical_transitions.find_index(Tile::encode("DhIR"), Tile::encode("XTNi")) << " - vertical transitions\n";
		// std::cout << horizontal_transitions.find_index(Tile::encode("DhIR"), Tile::encode("XTNi")) << " - horizontal transitions\n";
		// std::cout << "========================\n";

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
