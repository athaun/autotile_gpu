#pragma once

#include "common.h"

namespace Rules {

// Define a struct for the rule
struct rule_t {
	uint32_t from_1;
	uint32_t from_2;
	uint32_t to_1;
	uint32_t to_2;
};

// Constants for invalid values
const uint32_t EMPTY_TILE = 0xFFFFFFFF;
const rule_t INVALID_RULE = { EMPTY_TILE, EMPTY_TILE, EMPTY_TILE, EMPTY_TILE };

class Rules {
public:
	// Constructor
	Rules(size_t capacity) : table(capacity, INVALID_RULE), num_elements(0) {}

	// Default constructor
	Rules() : table(0, INVALID_RULE), num_elements(0) {}

	// Insert a rule into the table
	void insert(const rule_t& rule);

	// Find the index of a rule based on from_1 and from_2
	size_t find_index(uint32_t from_1, uint32_t from_2);

	// Find a rule based on from_1 and from_2
	rule_t find(uint32_t from_1, uint32_t from_2);

	// Get the load factor of the hash table
	double load_factor();

	std::vector<rule_t> table;

private:
	uint64_t hash_rule(const uint32_t from_1, const uint32_t from_2);
	uint64_t hash_rule(const rule_t& rule);

	// Resize the table when load factor exceeds the limit
	void resize(size_t new_size);


	size_t num_elements;
};

Rules load(std::string filepath);
bool is_valid(rule_t& rule);
}
