#include "common.h"
#include "shader/my_shader.hpp"


namespace Simulator {
    Rules::Rules rules;
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

    void init(std::string rules_path, std::string seed_path) {
        rules = Rules::load(rules_path);
        grid = Seed::load(seed_path);
    }

    void log_deltas(DeltaBuffer& buffer) {
        for (size_t i = 0; i < buffer.count; i++) {
            const delta_t& delta = buffer.deltas[i];
            std::cout << "(" << delta.location_a.x << ", " << delta.location_a.y << ", " 
                      << (delta.dir == delta_t::Direction::HORIZONTAL ? "H" : "V") << ")\t" 
                      << Tile::decode(delta.before.tile_a) << " + " << Tile::decode(delta.before.tile_b) 
                      << " -> " << Tile::decode(delta.after.tile_a) << " + " << Tile::decode(delta.after.tile_b) << "\n";
        }
    }

    void apply_deltas(DeltaBuffer& buffer) {
        for (size_t i = 0; i < buffer.count; i++) {
            const delta_t& delta = buffer.deltas[i];

            int direction_offset = delta.dir == delta_t::Direction::HORIZONTAL ? 1 : grid.width;

            tile_t* tile_a = &grid.tiles[delta.location_a.x + delta.location_a.y * grid.width];
            tile_t* tile_b = &grid.tiles[delta.location_a.x + delta.location_a.y * grid.width + direction_offset];
            // Tile::unlock(*tile_a);
            // Tile::unlock(*tile_b);
            *tile_a = delta.after.tile_a;
            *tile_b = delta.after.tile_b;
        }

        buffer.count = 0;
    }

    loc_t neighborhood[2] = {
        { 0, 1 }, // Down
        { 1, 0 }, // Right
    };

    void check_tile(tile_t tile_a, const loc_t& location) {

        for (int i = 0; i < 2; i++) {
            if (tile_a == Rules::EMPTY_TILE) {
                // Check attachment
                return;
            }
            if (Tile::is_locked(tile_a)) {
                return;
            }


            loc_t neighbor = { location.x + neighborhood[i].x, location.y + neighborhood[i].y };
            
            if (neighbor.x < 0 || neighbor.x >= grid.width || neighbor.y < 0 || neighbor.y >= grid.height) {
                continue;
            }

            tile_t tile_b = neighbor.get(grid);
            Rules::rule_t rule = rules.find(tile_a, tile_b);

            if (Tile::is_locked(tile_b)) {
                return;
            }///////////////////////////////

            if (!Rules::is_valid(rule)) continue;

            std::cout << "_";

            delta_t delta = {
                { tile_a, tile_b },
                { rule.to_1, rule.to_2 },
                location,
                static_cast<delta_t::Direction>(!i)
            };

            // Set the lock bit flag
            Tile::lock(grid.tiles[location.x + location.y * grid.width]);

            delta_buffer.push(delta);

            return;
        }
    }

    void run_serial() {
        for (int steps = 0; steps < 5; steps++) {
                for (int y = 0; y < grid.height; y++) {
            for (int x = 0; x < grid.width; x++) {
                    tile_t tile_a = grid.tiles[x + y * grid.width];
                    check_tile(tile_a, {x, y});
                    std::cout << Tile::decode(tile_a) << "\t";
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";
            log_deltas(delta_buffer);
            std::cout << "\n\n";
            apply_deltas(delta_buffer);
        }
    }

    void run_parallel() {
        kp::Manager mgr;

        std::shared_ptr<kp::TensorT<float>> tensorInA = mgr.tensor({ 2.0, 4.0, 6.0 });
        std::shared_ptr<kp::TensorT<float>> tensorInB = mgr.tensor({ 0.0, 1.0, 2.0 });
        std::shared_ptr<kp::TensorT<float>> tensorOut = mgr.tensor({ 0.0, 0.0, 0.0 });

        const std::vector<std::shared_ptr<kp::Tensor>> params = { tensorInA, tensorInB, tensorOut };

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
}