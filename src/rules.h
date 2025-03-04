#pragma once

#include "common.h"
#include <unordered_map>

namespace Rules {

// Define a struct for the rule
struct transition_t {
	tile_t tile_a;
	tile_t tile_b;
	tile_t to_a;
	tile_t to_b;
};

struct affinity_t {
	tile_t tile_a;
	tile_t tile_b;
    bool hash_tile_a;
};

// Constants for invalid values
const tile_t EMPTY_TILE = 0xEFFFFFFF;
const transition_t INVALID_TRANSITION = { EMPTY_TILE, EMPTY_TILE, EMPTY_TILE, EMPTY_TILE };
const affinity_t INVALID_AFFINITY = { EMPTY_TILE, EMPTY_TILE };

template <typename TilePair>
TilePair get_invalid_value();

template <typename TilePair>
class Rules {
public:
    Rules(size_t capacity = 0) : num_elements(0) {}

    void insert(const TilePair& rule);
    TilePair find(tile_t tile_a, tile_t tile_b, bool use_tile_a = true);
    double load_factor() const;

private:
    std::unordered_map<uint64_t, TilePair> table;
    std::unordered_map<uint64_t, TilePair> table_b; // used for affinities as a tile_b hash
    size_t num_elements;

    uint64_t hash_rule(const TilePair& rule);
};

template <typename TilePair>
void load(std::string filepath, Rules<TilePair>& rules);

void load_name_keys(std::string filepath, std::unordered_map<tile_t, std::string>& name_keys);

template <typename TilePair>
bool is_invalid(TilePair& rule);

}
