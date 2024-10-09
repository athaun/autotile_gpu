#include "common.h"
#include "shader/my_shader.hpp"
#include "simulator.h"

namespace Simulator {
Rules::Rules horizontal_rules;
Rules::Rules vertical_rules;
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

void init(std::string horizontal_rules_path, std::string vertical_rules_path, std::string seed_path) {
	horizontal_rules = Rules::load(horizontal_rules_path);
	vertical_rules = Rules::load(vertical_rules_path);
	grid = Seed::load(seed_path);
}

void init(std::string system_name) {
    system_name = "input/" + system_name;
    init(system_name + ".hrules", system_name + ".vrules", system_name + ".seed");
}

void log_deltas(DeltaBuffer& buffer) {
    if (buffer.count == 0) return;

    std::cout << "========================\nStep deltas: " << buffer.count << "\n";
	for (size_t i = 0; i < buffer.count; i++) {
		const delta_t& delta = buffer.deltas[i];
		std::cout << "(" << delta.location_a.x << ", " << delta.location_a.y << ", "
				  << (delta.dir == delta_t::Direction::HORIZONTAL ? "H" : "V") << ")\t"
				  << Tile::decode(delta.before.tile_a) << " + " << Tile::decode(delta.before.tile_b)
				  << " -> " << Tile::decode(delta.after.tile_a) << " + " << Tile::decode(delta.after.tile_b) << "\n";
	}

	// Print the grid
	std::cout << "\n";
	for (int y = 0; y < grid.height; y++) {
        for (int x = 0; x < grid.width; x++) {
            std::cout << Tile::decode(grid.tiles[x + y * grid.width]) << "\t";
        }
        std::cout << "\n";
    }
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
	std::cout << "\nApplied deltas\n";
	for (int y = 0; y < grid.height; y++) {
        for (int x = 0; x < grid.width; x++) {
            std::cout << Tile::decode(grid.tiles[x + y * grid.width]) << "\t";
        }
        std::cout << "\n";
    }

	buffer.count = 0;
}

loc_t neighborhood[2] = {
	{ 1, 0 }, // Right
	{ 0, 1 }, // Down
};

loc_t attachment_points[4] = {
    { 0, 1 }, // Down
    { 1, 0 }, // Right
    { 0, -1 }, // Up
    { -1, 0 }, // Left
};

void check_attachment(tile_t& tile_a, const loc_t& location, std::vector<delta_t>& possible_deltas) {

}

void check_transitions(tile_t& tile_a, const loc_t& location, std::vector<delta_t>& possible_deltas) {
    for (int dir = 0; dir < 2; dir++) {
		Rules::Rules& rules = dir == 1 ? vertical_rules : horizontal_rules;

        if (Tile::is_locked(tile_a)) continue;

    	loc_t neighbor = { location.x + neighborhood[dir].x, location.y + neighborhood[dir].y };

    	if (neighbor.x < 0 || neighbor.x >= grid.width || neighbor.y < 0 || neighbor.y >= grid.height) {
    		continue;
    	}

    	tile_t tile_b = neighbor.get(grid);
    	Rules::rule_t rule = rules.find(tile_a, tile_b);

    	if (Tile::is_locked(tile_b)) continue;
    	if (!Rules::is_valid(rule)) continue;

        Tile::lock(grid.tiles[location.x + location.y * grid.width]);
    	Tile::lock(grid.tiles[neighbor.x + neighbor.y * grid.width]);

    	delta_t delta = {
    		{ tile_a, tile_b },
    		{ rule.to_1, rule.to_2 },
    		location,
    		static_cast<delta_t::Direction>(dir)
    	};

    	possible_deltas.push_back(delta);
    }
}

void choose_deltas(std::vector<delta_t>& possible_deltas) {
    if (possible_deltas.empty()) return;

   	delta_buffer.push(possible_deltas[rand() % possible_deltas.size()]);
    possible_deltas.clear();
}

void run_serial() {
    srand(time(NULL));

	int ticks = 0;
	while (true) {
	    // Randomly select an X and Y coordinate in the grid
		loc_t location = { rand() % grid.width, rand() % grid.height };

		tile_t tile_a = grid.tiles[location.x + location.y * grid.width];

		std::vector<delta_t> possible_deltas = std::vector<delta_t>();

		if (tile_a == Rules::EMPTY_TILE) {
			check_attachment(tile_a, location, possible_deltas);
		} else {
		    check_transitions(tile_a, location, possible_deltas);
		}

		choose_deltas(possible_deltas);

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
