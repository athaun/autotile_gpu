#include "common.h"

namespace Rules {

//////////////////////////////////////////
// Rules implementation
//////////////////////////////////////////

void Rules::insert(const rule_t& rule) {
	if (load_factor() > 0.7) {
		resize(table.size() * 2); // Resize if load factor exceeds 0.7
	}

	size_t index = hash_rule(rule) % table.size();
	int probes = 0;

	// Loop until an empty slot is found or the table is full
	while (table[index].from_1 != EMPTY_TILE && probes < table.size()) {
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

size_t Rules::find_index(tile_t from_1, tile_t from_2) {
	uint64_t index = hash_rule(from_1, from_2) % table.size();
	int probes = 0;

	// Loop until an empty slot is found or the table is full
	while (table[index].from_1 != EMPTY_TILE && (table[index].from_1 != from_1 || table[index].from_2 != from_2) && probes < table.size()) {
		probes++;
		index = (index + probes * probes) % table.size(); // Quadratic probing
	}

	if (probes == table.size() || table[index].from_1 == EMPTY_TILE) {
		return table.size();
	}

	return index;
}

rule_t Rules::find(tile_t from_1, tile_t from_2) {
	size_t index = find_index(from_1, from_2);
	if (index == table.size()) {
		return INVALID_RULE;
	}
	return table[index];
}

std::vector<rule_t> table;

double Rules::load_factor() {
	return (double)num_elements / table.size();
}

//////////////////////////////////////////
// Rules private members
//////////////////////////////////////////

uint64_t Rules::hash_rule(const tile_t from_1, const tile_t from_2) {
	uint64_t hash = 0x811c9dc5;
	hash ^= from_1;
	hash *= 0x01000193;
	hash ^= from_2;
	hash *= 0x01000193;

	return hash;
}

uint64_t Rules::hash_rule(const rule_t& rule) {
	return hash_rule(rule.from_1, rule.from_2);
}

void Rules::resize(size_t new_size) {
	std::vector<rule_t> new_table(new_size, INVALID_RULE);
	for (const rule_t& rule : table) {
		if (rule.from_1 != EMPTY_TILE) {
			size_t index = hash_rule(rule) % new_size;
			int probes = 0;
			while (new_table[index].from_1 != EMPTY_TILE) {
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
	return !(rule.from_1 == INVALID_RULE.from_1 && 
			rule.from_2 == INVALID_RULE.from_2 &&
			rule.to_1 == INVALID_RULE.to_1 &&
			rule.to_2 == INVALID_RULE.to_2);
}

rule_t parse_rule(std::string rule_str) {
	// Find the positions of the special characters
	size_t plus_pos = rule_str.find("+");
	size_t arrow_pos = rule_str.find("->");
	size_t plus_pos2 = rule_str.find("+", arrow_pos + 4);

	if (plus_pos == std::string::npos || arrow_pos == std::string::npos || plus_pos2 == std::string::npos) {
		std::cout << "Invalid rule format: " << rule_str << "\n";
		throw std::invalid_argument("Invalid rule format");
	}

	// Extract substrings for each part
	std::string from_1_str = rule_str.substr(0, plus_pos);								// From 0 to first "+"
	std::string from_2_str = rule_str.substr(plus_pos + 1, arrow_pos - (plus_pos + 1)); // After first "+"
	std::string to_1_str = rule_str.substr(arrow_pos + 2, plus_pos2 - (arrow_pos + 2)); // After "->"
	std::string to_2_str = rule_str.substr(plus_pos2 + 1);								// After second "+"

	// Dump strings to tile_t
	tile_t from_1 = Tile::encode(from_1_str);
	tile_t from_2 = Tile::encode(from_2_str);

	tile_t to_1 = Tile::encode(to_1_str);
	tile_t to_2 = Tile::encode(to_2_str);

	// Create and return the rule_t structure
	return { from_1, from_2, to_1, to_2 };
}

Rules load(std::string filepath) {
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

	Rules rules(rule_count * 3);

	for (int i = 0; i < rule_count; i++) {
		std::getline(file, line);
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
