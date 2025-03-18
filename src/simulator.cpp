#include "common.h"
#include "shader/my_shader.hpp"
#include "simulator.h"
#include <thread>
#include <unordered_set>
#include <fmt/core.h>

namespace Simulator {

enum class SimState { RUNNING, PAUSED, STEP };
SimState sim_state = SimState::PAUSED;

Rules::Rules<Rules::transition_t> horizontal_transitions;
Rules::Rules<Rules::transition_t> vertical_transitions;
Rules::Rules<Rules::affinity_t> horizontal_affinities;
Rules::Rules<Rules::affinity_t> vertical_affinities;

Seed::grid_t grid;

std::vector<loc_t> available_locations_vector; // Hold all available locations for fast iteration
std::unordered_set<loc_t> available_locations_set; // Maintain unique locations
std::unordered_map<loc_t, size_t> location_indices; // Map locations to their indices in the vector for O(1) access

/**
 * Remove a location from the available locations datastructures
 * @param location The location to remove
 */
void remove_location(const loc_t& location) {
    auto it = location_indices.find(location);
    if (it == location_indices.end()) return;
    
    size_t index = it->second;

    if (index < available_locations_vector.size() - 1) {
        // Swap with the last element in the vector to maintain O(1) removal
        std::swap(available_locations_vector[index], available_locations_vector.back());
        // Update the index of the swapped element
        location_indices[available_locations_vector[index]] = index;
    }

    // Remove the last element and delete the index from the map and set
    available_locations_vector.pop_back();
    location_indices.erase(it);
    available_locations_set.erase(location);
}

void add_location(const loc_t& location) {
    // Check if the location is already in the set to avoid duplicates
    if (available_locations_set.find(location) != available_locations_set.end()) return;

    // Add the location to the vector and update the map and set
    available_locations_vector.push_back(location);
    location_indices[location] = available_locations_vector.size() - 1;
    available_locations_set.insert(location);
}

loc_t choose_random_location() {
    if (available_locations_vector.empty()) {
        return {-1, -1}; // No available locations
    }
    size_t index = rand() % available_locations_vector.size();
    return available_locations_vector[index];
}

void initialize_available_locations() {
    available_locations_vector.clear();
    available_locations_set.clear();
    location_indices.clear();
    
    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            if (grid.tiles[x + y * grid.width] == Rules::EMPTY_TILE) {
                continue;
            }
            add_location({x, y});
        }
    }
}

void offset_locations(int offset_x, int offset_y) {
    for (const loc_t& location : available_locations_vector) {
        loc_t& mutable_location = const_cast<loc_t&>(location);
        mutable_location.x += offset_x;
        mutable_location.y += offset_y;
    }
}


void DeltaBuffer::push(delta_t delta) {
    if (count < MAX_DELTAS) {
        deltas[count++] = delta;
    } else {
        count = 0;
    }
}

DeltaBuffer delta_buffer;

std::string system_name_cache;
void init(std::string system_name) {

    system_name_cache = "assets/systems/" + system_name;

	grid = Seed::load(system_name_cache + ".seed");

    initialize_available_locations();

	Rules::load(system_name_cache + ".htrans", horizontal_transitions);
    Rules::load(system_name_cache + ".vtrans", vertical_transitions);

    Rules::load(system_name_cache + ".haff", horizontal_affinities);
    Rules::load(system_name_cache + ".vaff", vertical_affinities);

	Rules::load_name_keys(system_name_cache + ".key", Tile::name_keys);
}

void reset_simulation() {
    Message message;
    message.type = Message::MessageType::RESET;
    message.content = "RESET_SIZE," + std::to_string(grid.width) + "," + std::to_string(grid.height);
    simulator_message_queue.push(message);

    grid = Seed::load(system_name_cache + ".seed");

    initialize_available_locations();

    delta_buffer.count = 0;

    sim_state = SimState::PAUSED;    
}

void resize_grid(Seed::grid_t& grid, int& resize_offset_x, int& resize_offset_y, loc_t attachment_location) {
    int new_width = grid.width;
    int new_height = grid.height;

    if (attachment_location.x < 2 || attachment_location.x >= grid.width - 2) {
        new_width *= 2;
        resize_offset_x = (new_width - grid.width) / 2;
    }

    if (attachment_location.y < 2 || attachment_location.y >= grid.height - 2) {
        new_height *= 2;
        resize_offset_y = (new_height - grid.height) / 2;
    }

    Seed::grid_t new_grid;
    new_grid.width = new_width;
    new_grid.height = new_height;
    new_grid.tiles = new tile_t[new_grid.width * new_grid.height]();
    
    std::fill(new_grid.tiles, new_grid.tiles + new_grid.width * new_grid.height, Rules::EMPTY_TILE);

    // Copy the old grid into the new grid
    for (int y = 0; y < grid.height; y++) {
        std::memcpy(
            &new_grid.tiles[(y + resize_offset_y) * new_grid.width + resize_offset_x], 
            &grid.tiles[y * grid.width], 
            grid.width * sizeof(tile_t)
        );
    }

    delete[] grid.tiles;
    grid = std::move(new_grid);

    offset_locations(resize_offset_x, resize_offset_y);
    Message grid_message;
    grid_message.type = Message::MessageType::CUSTOM;
    grid_message.content = fmt::format("GRID_RESIZE,{},{},{},{}", grid.width, grid.height, resize_offset_x, resize_offset_y);
    simulator_message_queue.push(grid_message);
    sim_state = SimState::PAUSED;
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
        fmt::print("{}", (delta.type == delta_t::Type::ATTACHMENT ? "(Attachment) " : "(Transition) "));
        if (!delta.note.empty()) {
            fmt::print("{}", delta.note);
        }
        
        fmt::print("\n\t({}, {}, {} + {} -> {} + {})",
                  delta.location_a.x, delta.location_a.y,
                  Tile::decode(delta.before.tile_a), Tile::decode(delta.before.tile_b),
                  Tile::decode(delta.after.tile_a), Tile::decode(delta.after.tile_b));
        
        fmt::print("\n\t({}, {}, {} + {} -> {} + {})\n\n",
                  delta.location_a.x, delta.location_a.y,
                  Tile::name(delta.before.tile_a), Tile::name(delta.before.tile_b),
                  Tile::name(delta.after.tile_a), Tile::name(delta.after.tile_b));

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
    if (buffer.count == 0) return;

    if (sim_state == SimState::STEP) {
        sim_state = SimState::PAUSED;
    }

    int resize_offset_x = 0;
    int resize_offset_y = 0;

    for (size_t i = 0; i < buffer.count; i++) {
        delta_t& delta = buffer.deltas[i];

        if (delta.location_a.x >= grid.width - 2 || 
            delta.location_a.y >= grid.height - 2 ||
            delta.location_a.x < 2 || 
            delta.location_a.y < 2)  {
            
            resize_grid(grid, resize_offset_x, resize_offset_y, delta.location_a);
        }
    }

    for (size_t i = 0; i < buffer.count; i++) {
        delta_t& delta = buffer.deltas[i];

        if (resize_offset_x != 0 || resize_offset_y != 0) {
            delta.location_a.x += resize_offset_x;
            delta.location_a.y += resize_offset_y;
            delta.location_b.x += resize_offset_x;
            delta.location_b.y += resize_offset_y;
        }

        tile_t& tile_a = grid.tiles[delta.location_a.x + delta.location_a.y * grid.width];
        tile_t& tile_b = grid.tiles[delta.location_b.x + delta.location_b.y * grid.width];

        tile_a = delta.after.tile_a;
        tile_b = delta.after.tile_b;

        if (delta.type == delta_t::Type::ATTACHMENT) {
            add_location(delta.location_a);
            add_location(delta.location_b);
        }
    }

    buffer.count = 0;
}

void check_attachment(tile_t& center_tile, const loc_t& location, std::vector<delta_t>& possible_deltas) {
    static loc_t neighborhood[4] = {
        { 1, 0 },  // Right
        { 0, -1 }, // Up  
        { -1, 0 }, // Left
        { 0, 1 }   // Down 
    };

    std::vector<delta_t> local_deltas[4];

    #pragma omp parallel for
    for (int dir = 0; dir < 4; dir++) {
        loc_t neighbor_tile_location = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

        // Check if the neighbor location is out of bounds
        if (neighbor_tile_location.x < 0 || neighbor_tile_location.x >= grid.width || 
            neighbor_tile_location.y < 0 || neighbor_tile_location.y >= grid.height) {
            continue;
        }

        // Select the appropriate set of affinities based on direction
        Rules::Rules<Rules::affinity_t>& affinities = (dir % 2 == 0) ? horizontal_affinities : vertical_affinities;

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
                break;
            case 1:
                // { 0, -1 }, Up (Adjusted for bottom-left origin)
                // center_tile is `tile_a`
                affinity = affinities.find(center_tile, neighbor_tile, true);
                delta_location_a = location;
                delta_location_b = neighbor_tile_location;
                break;
            case 2:
                // { -1, 0 }, Left
                // neighbor_tile is `tile_a`
                affinity = affinities.find(neighbor_tile, center_tile, false);
                delta_location_a = neighbor_tile_location;
                delta_location_b = location;
                break;
            case 3:
                // { 0, 1 }, Down (Adjusted for bottom-left origin)
                // neighbor_tile is `tile_a`
                affinity = affinities.find(neighbor_tile, center_tile, false);
                delta_location_a = neighbor_tile_location;
                delta_location_b = location;
                break;
            default:
                continue;
        }

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
            delta_t::Type::ATTACHMENT
        };

        local_deltas[dir].push_back(delta);
    }

    // Combine local deltas into the main possible_deltas vector
    for (int dir = 0; dir < 4; dir++) {
        possible_deltas.insert(possible_deltas.end(), local_deltas[dir].begin(), local_deltas[dir].end());
    }
}

void check_transitions(tile_t& tile_a, const loc_t& location, std::vector<delta_t>& possible_deltas) {
    static loc_t neighborhood[4] = {
        { 1, 0 },  // Right
        { 0, -1 }, // Up  
        { -1, 0 }, // Left
        { 0, 1 }   // Down 
    };


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
            fmt::print("[BACKEND] Simulator paused.\n");
        } else if (msg->type == Message::STEP) {
            sim_state = SimState::STEP;
            fmt::print("[BACKEND] Simulator stepping.\n");
        } else if (msg->type == Message::RUN) {
            sim_state = SimState::RUNNING;
            fmt::print("[BACKEND] Simulator running.\n");
        } else if (msg->type == Message::RESET) {
            reset_simulation();
            fmt::print("[BACKEND] Simulator reset.\n");
        } else if (msg->type == Message::EXIT) {
            // TODO CLEANUP MEMORY :D
            fmt::print("[BACKEND] Simulator exiting.\n");
            exit(0);
        }
    }
}

Timer profiler;

void run_serial() {
	std::vector<delta_t> possible_deltas = std::vector<delta_t>();
    srand(time(NULL));

	long ticks = 0;
	while (true) {
        // profiler.start("process_messages");
		    // process_control_messages();
        // profiler.end("process_messages");

		// if (sim_state == SimState::PAUSED) {
		// 	continue;
		// }
		
        // profiler.start("choose_location");
            loc_t location = choose_random_location();
        // profiler.end("choose_location");

		tile_t tile_a = grid.tiles[location.x + location.y * grid.width];

        // profiler.start("check_transitions");
            check_transitions(tile_a, location, possible_deltas);
        // profiler.end("check_transitions");

        // profiler.start("check_attachment");
            check_attachment(tile_a, location, possible_deltas);
        // profiler.end("check_attachment");

        // profiler.start("choose_delta");
            choose_delta(possible_deltas);
        // profiler.end("choose_delta");

        // profiler.start("send_deltas");
            send_deltas(delta_buffer);        
        // profiler.end("send_deltas");

        // profiler.start("apply_deltas");
            apply_deltas(delta_buffer);
        // profiler.end("apply_deltas");

        // if (ticks++ % 999999 == 0) {
        //     profiler.report();
        //     profiler.reset();
        //     fmt::print("Ticks: {}\nGrid Size: {}x{}\nAvailable Locations: {}\nDelta Buffer Count: {}\n", 
        //                 ticks, grid.width, grid.height, available_locations_vector.size(), delta_buffer.count);
        // }
        
	}
}

void run_parallel() {
    // 1. Create Kompute Manager with default settings (device 0, first queue and no extensions)
    kp::Manager mgr;

    // 2. Create and initialize Kompute Tensors through manager
    auto tensorInA = mgr.tensor({ 2.0, 4.0, 6.0 });
    auto tensorInB = mgr.tensor({ 0.0, 1.0, 2.0 });
    auto tensorOut = mgr.tensorT<uint32_t>({ 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Memory>> params = { tensorInA, tensorInB, tensorOut };

    // 3. Create algorithm based on shader (supports buffers & push/spec constants)
    kp::Workgroup workgroup({3, 1, 1});
    std::vector<float> specConsts({ 2 });
    std::vector<float> pushConsts({ 2.0 });

    const std::vector<uint32_t> shader = std::vector<uint32_t>(shader::MY_SHADER_COMP_SPV.begin(), shader::MY_SHADER_COMP_SPV.end());
    auto algorithm = mgr.algorithm(params, shader, workgroup, specConsts, pushConsts);

    // 4. Run operation synchronously using sequence
    mgr.sequence()
        ->record<kp::OpSyncDevice>(params)
        ->record<kp::OpAlgoDispatch>(algorithm)
        ->record<kp::OpSyncLocal>(params)
        ->eval();

    // 5. Sync results from the GPU asynchronously
    auto sq = mgr.sequence();
    sq->evalAsync<kp::OpSyncLocal>(params);

    // ... Do other work asynchronously whilst GPU finishes

    sq->evalAwait();

    // 6. Print the output
    fmt::print("Output: {{  ");
    for (const uint32_t& elem : tensorOut->vector()) {
        fmt::print("{}  ", elem);
    }
    fmt::print("}}\n");

    // Verify the result
    if (tensorOut->vector() != std::vector<uint32_t>{ 0, 4, 12 }) {
        throw std::runtime_error("Result does not match");
    }
}
} // namespace Simulator
