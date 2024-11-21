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
// template <typename TilePair>
// class Rules {
// public:
// 	// Constructor
// 	Rules(size_t capacity);

// 	// Default constructor
// 	Rules() : table(0, get_invalid_value<TilePair>()), num_elements(0) {}

// 	// Insert a rule into the table
// 	void insert(const TilePair& rule);

// 	// Find the index of a rule based on tile_a and tile_b
// 	size_t find_index(tile_t tile_a, tile_t tile_b);

// 	// Find a rule based on tile_a and tile_b
// 	TilePair find(tile_t tile_a, tile_t tile_b);

// 	// Get the load factor of the hash table
// 	double load_factor();

// private:
// 	uint64_t hash_rule(const TilePair& rule);

// 	// Resize the table when load factor exceeds the limit
// 	void resize(size_t new_size);

// 	bool is_prime(size_t n);

//     size_t next_prime(size_t n);

// 	size_t num_elements;

// 	std::vector<TilePair> table;
// };

template <typename TilePair>
void load(std::string filepath, Rules<TilePair>& rules);

template <typename TilePair>
bool is_invalid(TilePair& rule);

}
