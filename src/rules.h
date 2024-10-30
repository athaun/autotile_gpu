#pragma once

#include "common.h"

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
	// Constructor
	Rules(size_t capacity) : table(capacity, get_invalid_value<TilePair>()), num_elements(0) {}

	// Default constructor
	Rules() : table(0, get_invalid_value<TilePair>()), num_elements(0) {}

	// Insert a rule into the table
	void insert(const TilePair& rule);

	// Find the index of a rule based on tile_a and tile_b
	size_t find_index(tile_t tile_a, tile_t tile_b);

	// Find a rule based on tile_a and tile_b
	TilePair find(tile_t tile_a, tile_t tile_b);

	// Get the load factor of the hash table
	double load_factor();

	std::vector<TilePair> table;

private:
	uint64_t hash_rule(const TilePair& rule);

	// Resize the table when load factor exceeds the limit
	void resize(size_t new_size);

	size_t num_elements;

};

template <typename TilePair>
Rules<TilePair> load(std::string filepath);

template <typename TilePair>
bool is_invalid(TilePair& rule);

}
