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

	size_t index = find_index(rule.tile_a, rule.tile_b);

	if (index == table.size()) {
		std::cout << "BUG\n";
		return;
	}

	table[index] = rule;
	num_elements++;
}

template <typename TilePair>
size_t Rules<TilePair>::find_index(tile_t tile_a, tile_t tile_b) {
	uint64_t index = hash_rule({tile_a, tile_b}) % table.size();
	int probes = 0;

	// Loop until an empty slot is found or the table is full
	while (table[index].tile_a != EMPTY_TILE && (table[index].tile_a != tile_a || table[index].tile_b != tile_b) && probes < table.size()) {
		probes++;
		index = (index + probes * probes) % table.size(); // Quadratic probing
	}

	if (probes == table.size() || table[index].tile_a == EMPTY_TILE) {
		std::cout << "BUG 2\n";
		return table.size();
	}

	return index;
}

template <typename TilePair>
TilePair Rules<TilePair>::find(tile_t tile_a, tile_t tile_b) {
	size_t index = find_index(tile_a, tile_b);
	if (index == table.size()) {
		return get_invalid_value<TilePair>();
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
uint64_t Rules<TilePair>::hash_rule(const TilePair& rule) {
	uint64_t hash = 0x811c9dc5;
	if constexpr (std::is_same<TilePair, transition_t>::value) {
		hash ^= rule.tile_a;
		hash *= 0x01000193;
		hash ^= rule.tile_b;
		hash *= 0x01000193;
	} else if constexpr (std::is_same<TilePair, affinity_t>::value) {
		hash ^= rule.tile_a;
		hash *= 0x01000193;
	}

	/*
	if constexpr (std::is_same<TilePair, transition_t>::value) {
		hash = std::hash<tile_t>{}(rule.tile_a) ^ std::hash<tile_t>{}(rule.tile_b);
	} else if constexpr (std::is_same<TilePair, affinity_t>::value) {
		hash = std::hash<tile_t>{}(rule.tile_a);
	}
	*/
	return hash;
}

template <typename TilePair>
void Rules<TilePair>::resize(size_t new_size) {
	std::vector<TilePair> new_table(new_size, get_invalid_value<TilePair>());
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

template <typename TilePair>
TilePair get_invalid_value();

template <>
transition_t get_invalid_value<transition_t>() {
	return INVALID_TRANSITION;
}

template <>
affinity_t get_invalid_value<affinity_t>() {
	return INVALID_AFFINITY;
}

//////////////////////////////////////////
// Rules namespace functions
//////////////////////////////////////////

/**
 * Check if a transition is valid and can be applied
 */
template <typename TilePair>
bool is_invalid(TilePair& rule) {
	return (rule.tile_a != EMPTY_TILE && rule.tile_b != EMPTY_TILE);
}

/**
 * Parse a rule string and return a transition_t or affinity_t object
 * @param rule_str The rule string to parse
 * @return A transition_t or affinity_t struct
 * 
 */
template <typename TilePair>
TilePair parse_rule(std::string rule_str);

/**
 * Parse a rule in the format "A + B -> C + D" and return a transition_t struct
 * @param rule_str The rule string to parse
 * @return A transition_t struct
 */
template <>
transition_t parse_rule(std::string rule_str) {
	// Find the positions of the special characters
	size_t plus_pos = rule_str.find("+");
	size_t arrow_pos = rule_str.find("->");
	size_t plus_pos2 = rule_str.find("+", arrow_pos + 1);

	if (plus_pos == std::string::npos || arrow_pos == std::string::npos || plus_pos2 == std::string::npos) {
		std::cout << "Invalid transition format: " << rule_str << "\n";
		throw std::invalid_argument("Invalid transition format");
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

	return { tile_a, tile_b, to_a, to_b };
}

/**
 * Parse a rule in the format "A + B" and return an affinity_t struct
 * @param rule_str The rule string to parse
 * @return An affinity_t struct
 */
template <>
affinity_t parse_rule(std::string rule_str) {
	// Find the positions of the special characters
	size_t plus_pos = rule_str.find("+");

	if (plus_pos == std::string::npos) {
		std::cout << "Invalid affinity format: " << rule_str << "\n";
		throw std::invalid_argument("Invalid affinity format");
	}

	// Extract substrings for each part
	std::string tile_a_str = rule_str.substr(0, plus_pos);
	std::string tile_b_str = rule_str.substr(plus_pos + 1);

	// Dump strings to tile_t
	tile_t tile_a = Tile::encode(tile_a_str);
	tile_t tile_b = Tile::encode(tile_b_str);

	return { tile_a, tile_b };
}

/**
 * Load rules from a file and return a Rules object of either transition_t or affinity_t
 * @param filepath The path to the file containing the rules, either a .htrans, .vtrans, or .haff, .vaff file.
 * 
 * Rule files are always located in the input/ directory.
 * 
 * Transition files are formatted as:
 * <number of rules> Transitions
 * A + B -> C + D
 * 
 * Affinity files are formatted as:
 * <number of rules> Affinities
 * A + B
 * 
 * @return A Rules object containing the parsed rules
 * 
 */
template <typename TilePair>
Rules<TilePair> load(std::string filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open the file " << filepath << std::endl;
		throw std::invalid_argument("Could not open the file");
	}

	std::string line;
	std::getline(file, line);

	line = line.substr(0, line.find(' '));
	int rule_count = std::stoi(line);
	if constexpr (std::is_same<TilePair, transition_t>::value) {
		std::cout << "There are " << rule_count << " transition rules to parse\n";
	} else if constexpr (std::is_same<TilePair, affinity_t>::value) {
		std::cout << "There are " << rule_count << " affinity rules to parse\n";
	}

	Rules<TilePair> rules(rule_count * 3);

	for (int i = 0; i < rule_count; i++) {
		std::getline(file, line);

		// If the line begins with #, skip it.
		if (line[0] == '#') continue;

		rules.insert(parse_rule<TilePair>(line));

		if (i % 1000000 == 0 && i != 0) {
            std::cout << "Parsed " << i << " rules\n";
        }
	}

	file.close();

	std::cout << "Finished parsing rules\n\n";

	return rules;
}

template class Rules<transition_t>;
template class Rules<affinity_t>;
template Rules<transition_t> load<transition_t>(std::string filepath);
template Rules<affinity_t> load<affinity_t>(std::string filepath);
template transition_t parse_rule<transition_t>(std::string rule_str);
template affinity_t parse_rule<affinity_t>(std::string rule_str);
template bool is_invalid(transition_t& rule);
template bool is_invalid(affinity_t& rule);

} // namespace Rules
