#include "common.h"
#include "shader/my_shader.hpp"
#include "simulator.h"
#include <thread>

namespace Simulator {

enum class SimState { RUNNING, PAUSED, STEP };
SimState sim_state = SimState::PAUSED;

Rules::Rules<Rules::transition_t> horizontal_transitions;
Rules::Rules<Rules::transition_t> vertical_transitions;
Rules::Rules<Rules::affinity_t> horizontal_affinities;
Rules::Rules<Rules::affinity_t> vertical_affinities;

Seed::grid_t grid;

void DeltaBuffer::push(delta_t delta) {
	if (count < MAX_DELTAS) {
		deltas[count++] = delta;
	} else {
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

	Rules::load_name_keys(system_name + ".key", Tile::name_keys);
}

void log_deltas(DeltaBuffer& buffer) {
    if (buffer.count == 0) return;

	std::ofstream delta_file("deltas.txt", std::ios::app);  // Open in append mode
	if (!delta_file.is_open()) {
        std::cerr << "Error: Could not open file for writing deltas." << std::endl;
        return;
    }

	for (size_t i = 0; i < buffer.count; i++) {
		const delta_t& delta = buffer.deltas[i];
		std::cout << (delta.type == delta_t::Type::ATTACHMENT ? "(Attachment) " : "(Transition) ");
		if (delta.note.length() != 0) {
			std::cout << delta.note;
		}
		std::cout << "\n\t(" << delta.location_a.x << ", " << delta.location_a.y << ", "
				  << Tile::decode(delta.before.tile_a) << " + " << Tile::decode(delta.before.tile_b)
				  << " -> " << Tile::decode(delta.after.tile_a) << " + " << Tile::decode(delta.after.tile_b);
		
		std::cout << "\n\t(" << delta.location_a.x << ", " << delta.location_a.y << ", "
				  << Tile::name(delta.before.tile_a) << " + " << Tile::name(delta.before.tile_b)
				  << " -> " << Tile::name(delta.after.tile_a) << " + " << Tile::name(delta.after.tile_b) << "\n\n";


		delta_file << (delta.type == delta_t::Type::ATTACHMENT ? "(Attachment) " : "(Transition) ");
		if (delta.note.length() != 0) {
			delta_file << delta.note;
		}
		delta_file << "\n\t(" << delta.location_a.x << ", " << delta.location_a.y << ", "
				  << Tile::decode(delta.before.tile_a) << " + " << Tile::decode(delta.before.tile_b)
				  << " -> " << Tile::decode(delta.after.tile_a) << " + " << Tile::decode(delta.after.tile_b);
		
		delta_file << "\n\t(" << delta.location_a.x << ", " << delta.location_a.y << ", "
				  << Tile::name(delta.before.tile_a) << " + " << Tile::name(delta.before.tile_b)
				  << " -> " << Tile::name(delta.after.tile_a) << " + " << Tile::name(delta.after.tile_b) << "\n\n";
		
	}

	delta_file.close();
}

void send_deltas(DeltaBuffer& buffer) {
	for (size_t i = 0; i < buffer.count; i++) {
		const delta_t& delta = buffer.deltas[i];

		Message message_a, message_b;

		message_a.type = Message::TILE_UPDATE;
		message_a.location = delta.location_a;
		message_a.value = delta.after.tile_a;

		message_b.type = Message::TILE_UPDATE;
		message_b.location = delta.location_b;
		message_b.value = delta.after.tile_b;

		simulator_message_queue.push(message_a);
		simulator_message_queue.push(message_b);
	}
}

void apply_deltas(DeltaBuffer& buffer) {
    if (buffer.count == 0) {
        // exit(0);
		return;
    }

	if (sim_state == SimState::STEP) {
		sim_state = SimState::PAUSED;
	}

    

	for (size_t i = 0; i < buffer.count; i++) {
		const delta_t& delta = buffer.deltas[i];

		tile_t& tile_a = grid.tiles[delta.location_a.x + delta.location_a.y * grid.width];
		tile_t& tile_b = grid.tiles[delta.location_b.x + delta.location_b.y * grid.width];

		// This assignment will unlock the tile by overwriting the lock bit
		tile_a = delta.after.tile_a;
		tile_b = delta.after.tile_b;
	}

	// sleep for a bit
	// std::this_thread::sleep_for(std::chrono::milliseconds(100));

	buffer.count = 0;
}


loc_t neighborhood[4] = {
    { 1, 0 },  // Right
    { 0, -1 }, // Up   (Adjusted for bottom-left origin)
    { -1, 0 }, // Left
    { 0, 1 }   // Down (Adjusted for bottom-left origin)
};

void check_attachment(tile_t& center_tile, const loc_t& location, std::vector<delta_t>& possible_deltas) {
    for (int dir = 0; dir < 4; dir++) {
        // Select the appropriate set of affinities based on direction
        Rules::Rules<Rules::affinity_t>& affinities = (dir % 2 == 0) ? horizontal_affinities : vertical_affinities;

        std::string note = (dir % 2 == 0) ? "haff" : "vaff";

        loc_t neighbor_tile_location = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

        // Check if the neighbor location is out of bounds
        if (neighbor_tile_location.x < 0 || neighbor_tile_location.x >= grid.width || 
            neighbor_tile_location.y < 0 || neighbor_tile_location.y >= grid.height) {
            continue;
        }

        // Get the tile at the neighbor location
        tile_t neighbor_tile = neighbor_tile_location.get(grid);

        // Ensure that both tiles are not empty, there is nothing to attach to
        if (center_tile == Rules::EMPTY_TILE && neighbor_tile == Rules::EMPTY_TILE) {
            continue;
        }

        // Ensure both tiles are not already filled, there is no space to attach
        if (center_tile != Rules::EMPTY_TILE && neighbor_tile != Rules::EMPTY_TILE) {
            continue;
        }

        // Ensure neither tile is locked
        if ((Tile::is_locked(center_tile) && center_tile != Rules::EMPTY_TILE) || 
            (Tile::is_locked(neighbor_tile) && neighbor_tile != Rules::EMPTY_TILE)) {
            continue;
        }

        // Determine the correct orientation based on direction
        Rules::affinity_t affinity;
        loc_t delta_location_a, delta_location_b;

        /**
         * Directional Handling for Affinity Lookup (Bottom-Left Origin)
         *
         * Ensures that tile A is always west (left) or north (above) of tile B.
         *
         * | Direction       | Affinity Lookup                         | tile_a        | tile_b        | use_tile_a |
         * |-----------------|-----------------------------------------|---------------|---------------|------------|
         * | Right { 1, 0 }  | find(center_tile, neighbor_tile, true)  | center_tile   | neighbor_tile | true       |
         * | Up    { 0, -1 } | find(center_tile, neighbor_tile, true)  | center_tile   | neighbor_tile | true       |
         * | Left  { -1, 0 } | find(neighbor_tile, center_tile, false) | neighbor_tile | center_tile   | false      |
         * | Down  { 0, 1 }  | find(neighbor_tile, center_tile, false) | neighbor_tile | center_tile   | false      |
         */

        switch (dir) { 
            case 0:
                // { 1, 0 }, Right
                // center_tile is `tile_a`
                affinity = affinities.find(center_tile, neighbor_tile, true);
                delta_location_a = location;
                delta_location_b = neighbor_tile_location;
                note += "-right";
                break;
            case 1:
                // { 0, -1 }, Up (Adjusted for bottom-left origin)
                // center_tile is `tile_a`
                affinity = affinities.find(center_tile, neighbor_tile, true);
                delta_location_a = location;
                delta_location_b = neighbor_tile_location;
                note += "-up";
                break;
            case 2:
                // { -1, 0 }, Left
                // neighbor_tile is `tile_a`
                affinity = affinities.find(neighbor_tile, center_tile, false);
                delta_location_a = neighbor_tile_location;
                delta_location_b = location;
                note += "-left";
                break;
            case 3:
                // { 0, 1 }, Down (Adjusted for bottom-left origin)
                // neighbor_tile is `tile_a`
                affinity = affinities.find(neighbor_tile, center_tile, false);
                delta_location_a = neighbor_tile_location;
                delta_location_b = location;
                note += "-down";
                break;
            default:
                continue;
        }

        note += " of center tile: " + Tile::decode(center_tile);

        // If the affinity is invalid, skip this iteration
        if (Rules::is_invalid(affinity)) continue; // TODO CHECK IF SHOULD BE && or || in this fn

        // Lock the tiles
        Tile::lock(grid.tiles[location.x + location.y * grid.width]);
        Tile::lock(grid.tiles[neighbor_tile_location.x + neighbor_tile_location.y * grid.width]);

        tile_t before_tile_a, before_tile_b;
        if (dir < 2) { // Right and Up: center_tile is A
            before_tile_a = center_tile;
            before_tile_b = neighbor_tile;
        } else { // Left and Down: neighbor_tile is A
            before_tile_a = neighbor_tile;
            before_tile_b = center_tile;
        }

        delta_t delta = {
            { before_tile_a, before_tile_b }, // before state, now correctly ordered
            { affinity.tile_a, affinity.tile_b }, // after state from the affinity lookup
            delta_location_a,
            delta_location_b,
            delta_t::Type::ATTACHMENT,
            note
        };

        // Add the valid delta to the list of possible deltas
        possible_deltas.push_back(delta);
    }
}

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

void process_control_messages() {
    while (auto msg = frontend_message_queue.try_pop()) {

        if (msg->type == Message::PAUSE) {
            sim_state = SimState::PAUSED;
			std::cout << "[BACKEND] Simulator paused." << std::endl;
        } else if (msg->type == Message::STEP) {
            sim_state = SimState::STEP;
			std::cout << "[BACKEND] Simulator stepping." << std::endl;
        } else if (msg->type == Message::RUN) {
            sim_state = SimState::RUNNING;
			std::cout << "[BACKEND] Simulator running." << std::endl;
        } else if (msg->type == Message::EXIT) {
			// TODO CLEANUP MEMORY :D
			std::cout << "[BACKEND] Simulator exiting." << std::endl;
			exit(0);
		}
    }
}


void run_serial() {
	std::vector<delta_t> possible_deltas = std::vector<delta_t>();
    srand(time(NULL));

	std::cout << "Initial grid state:" << std::endl;

	int ticks = 0;
	while (true) {
		process_control_messages();


		if (sim_state == SimState::PAUSED) {
			continue;
		}
		
	    // Randomly select an X and Y coordinate in the grid
		loc_t location = { rand() % grid.width, rand() % grid.height };
		tile_t tile_a = grid.tiles[location.x + location.y * grid.width];

		check_transitions(tile_a, location, possible_deltas);
		check_attachment(tile_a, location, possible_deltas);

		choose_delta(possible_deltas);

		if (++ticks % 100 == 0) {
			log_deltas(delta_buffer);
			send_deltas(delta_buffer);
			apply_deltas(delta_buffer);
		}
	}
}

void run_parallel() {
/*	kp::Manager mgr;

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
	}*/
}
} // namespace Simulator
