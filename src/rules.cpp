#include "common.h"

namespace Rules {

//////////////////////////////////////////
// Rules implementation
//////////////////////////////////////////

template <typename TilePair>
void Rules<TilePair>::insert(const TilePair& rule) {
	if (load_factor() > 0.7) {
		resize(table.size() * 2); // Resize if load factor exceeds 0.7
		std::cout << "Resized table to " << table.size() << " elements\n";
	}

	size_t index = hash_rule(rule) % table.size();
	int probes = 0;

	// Loop until an empty slot is found or the table is full
	while (table[index].tile_a != EMPTY_TILE && probes < table.size()) {
		probes++;
		index = (index + probes * probes) % table.size(); // Quadratic probing
	}

	if (probes == table.size()) {
		std::cerr << "Error: Hash table overflow\n";
		return;
	}

	table[index] = rule;
	num_elements++;
}

template <typename TilePair>
size_t Rules<TilePair>::find_index(tile_t tile_a, tile_t tile_b) {
	uint64_t index = hash_rule(tile_a, tile_b) % table.size();
	int probes = 0;

	// Loop until an empty slot is found or the table is full
	while (table[index].tile_a != EMPTY_TILE && (table[index].tile_a != tile_a || table[index].tile_b != tile_b) && probes < table.size()) {
		probes++;
		index = (index + probes * probes) % table.size(); // Quadratic probing
	}

	if (probes == table.size() || table[index].tile_a == EMPTY_TILE) {
		return table.size();
	}

	return index;
}

template <typename TilePair>
TilePair Rules<TilePair>::find(tile_t tile_a, tile_t tile_b) {
	size_t index = find_index(tile_a, tile_b);
	if (index == table.size()) {
		return INVALID_RULE;
	}
	return table[index];
}

template <typename TilePair>
double Rules<TilePair>::load_factor() {
	return (double)num_elements / table.size();
}

//////////////////////////////////////////
// Rules private members
//////////////////////////////////////////

template <typename TilePair>
uint64_t Rules<TilePair>::hash_rule(const tile_t tile_a, const tile_t tile_b) {
	uint64_t hash = 0x811c9dc5;
	hash ^= tile_a;
	hash *= 0x01000193;
	hash ^= tile_b;
	hash *= 0x01000193;

	return hash;
}

template <typename TilePair>
uint64_t Rules<TilePair>::hash_rule(const TilePair& rule) {
	return hash_rule(rule.tile_a, rule.tile_b);
}

template <typename TilePair>
void Rules<TilePair>::resize(size_t new_size) {
	std::vector<TilePair> new_table(new_size, INVALID_RULE);
	for (const TilePair& rule : table) {
		if (rule.tile_a != EMPTY_TILE) {
			size_t index = hash_rule(rule) % new_size;
			int probes = 0;
			while (new_table[index].tile_a != EMPTY_TILE) {
				probes++;
				index = (index + probes * probes) % new_size; // Quadratic probing
			}
			new_table[index] = rule;
		}
	}
	table.swap(new_table);
}

//////////////////////////////////////////
// Rules namespace functions
//////////////////////////////////////////

bool is_valid(rule_t& rule) {
	return !(rule.tile_a == INVALID_RULE.tile_a &&
			rule.tile_b == INVALID_RULE.tile_b &&
			rule.to_a == INVALID_RULE.to_a &&
			rule.to_b == INVALID_RULE.to_b);
}

rule_t parse_rule(std::string rule_str) {
	// Find the positions of the special characters
	size_t plus_pos = rule_str.find("+");
	size_t arrow_pos = rule_str.find("->");
	size_t plus_pos2 = rule_str.find("+", arrow_pos + 1);

	if (plus_pos == std::string::npos || arrow_pos == std::string::npos || plus_pos2 == std::string::npos) {
		std::cout << "Invalid rule format: " << rule_str << "\n";
		throw std::invalid_argument("Invalid rule format");
	}

	// Extract substrings for each part
	std::string tile_a_str = rule_str.substr(0, plus_pos);								// From 0 to first "+"
	std::string tile_b_str = rule_str.substr(plus_pos + 1, arrow_pos - (plus_pos + 1)); // After first "+"
	std::string to_a_str = rule_str.substr(arrow_pos + 2, plus_pos2 - (arrow_pos + 2)); // After "->"
	std::string to_b_str = rule_str.substr(plus_pos2 + 1);								// After second "+"

	// Dump strings to tile_t
	tile_t tile_a = Tile::encode(tile_a_str);
	tile_t tile_b = Tile::encode(tile_b_str);

	tile_t to_a = Tile::encode(to_a_str);
	tile_t to_b = Tile::encode(to_b_str);

	// std::cout << "Parsed rule: " << tile_a_str << " + " << tile_b_str << " -> " << to_a_str << " + " << to_b_str << "\n";
	// std::cout << "Encoded rule: " << tile_a << " + " << tile_b << " -> " << to_a << " + " << to_b << "\n\n";

	// Create and return the rule_t structure
	return { tile_a, tile_b, to_a, to_b };
}

Rules<rule_t> load(std::string filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open the file " << filepath << std::endl;
		throw std::invalid_argument("Could not open the file");
	}

	std::string line;
	std::getline(file, line);

	line = line.substr(0, line.find(' '));
	int rule_count = std::stoi(line);
	std::cout << "There are " << rule_count << " rules to parse\n";

	Rules rules<rule_t>(rule_count * 3);

	for (int i = 0; i < rule_count; i++) {
		std::getline(file, line);

		// If the line begins with #, skip it.
		if (line[0] == '#') continue;

		rules.insert(parse_rule(line));

		if (i % 1000000 == 0 && i != 0) {
            std::cout << "Parsed " << i << " rules\n";
        }
	}

	file.close();

	std::cout << "Finished parsing rules\n\n";

	return rules;
}
} // namespace Rules
