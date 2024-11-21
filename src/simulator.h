#pragma once

#include "common.h"

namespace Simulator {

    struct loc_t {
        int x;
        int y;
        tile_t& get(Seed::grid_t grid) const {
        return grid.tiles[x + y * grid.width];
        }
    };

    struct tile_pair_t {
        tile_t tile_a;
        tile_t tile_b;
    };

    struct delta_t {
        tile_pair_t before;
        tile_pair_t after;
        loc_t location_a;
        loc_t location_b;

        enum Type {
            ATTACHMENT,
            TRANSITION
        };
        Type type;
        std::string note;
    };

    constexpr size_t MAX_DELTAS = 1024;

    struct DeltaBuffer {
        delta_t deltas[MAX_DELTAS];
        size_t count;

        DeltaBuffer() : count(0) {}

        void push(delta_t delta);
    };

    void init(std::string horizontal_transitions_path, std::string vertical_transitions_path, std::string seed_path);
    void init(std::string system_name);
    void log_deltas(DeltaBuffer& buffer);
    void run_serial();
    void run_parallel();
}
