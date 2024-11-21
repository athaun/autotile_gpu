#include "common.h"

namespace Rules {

//////////////////////////////////////////
// Rules implementation
//////////////////////////////////////////

// template <typename TilePair>
// Rules<TilePair>::Rules(size_t capacity) : num_elements(0) {
// 	table = std::vector<TilePair>(next_prime(capacity), get_invalid_value<TilePair>());
// }

// template <typename TilePair>
// void Rules<TilePair>::insert(const TilePair& rule) {
// 	if (load_factor() > 0.6) {
// 		resize(next_prime(table.size() * 3)); // Resize if load factor exceeds 0.7
// 		std::cout << "Resized table to " << table.size() << " elements\n";
// 	}

// 	size_t index = find_index(rule.tile_a, rule.tile_b);

// 	if (index == table.size()) {
// 		return;
// 		std::cout << "Error: Rule not found in table\n";
// 	}

// 	table[index] = rule;
// 	num_elements++;
// }

// template <typename TilePair>
// size_t Rules<TilePair>::find_index(tile_t tile_a, tile_t tile_b) {
//     // Primary hash to find the initial index
//     size_t index = hash_rule({tile_a, tile_b}) % table.size();
//     size_t probe = 1;  // Quadratic probe factor

//     // Loop until an empty slot or matching entry is found, or all slots are probed
//     while (table[index].tile_a != EMPTY_TILE) {
//         // If we find a matching tile pair, return the index
//         if (table[index].tile_a == tile_a && table[index].tile_b == tile_b) {
//             return index;
//         }

//         // Quadratic probing to find the next index
//         index = (index + probe * probe) % table.size();
//         probe++;

//         // If we exceed the table size in probes, no slot found
//         if (probe >= table.size()) {
//             return table.size();  // Indicate failure
//         }
//     }

//     // Return the index for insertion if an empty slot is found
//     return index;
// }





// template <typename TilePair>
// TilePair Rules<TilePair>::find(tile_t tile_a, tile_t tile_b) {
// 	size_t index = find_index(tile_a, tile_b);
// 	if (index == table.size()) {
// 		return get_invalid_value<TilePair>();
// 	}

// 	return table[index];
// }

// template <typename TilePair>
// double Rules<TilePair>::load_factor() {
// 	return (double)num_elements / table.size();
// }

//////////////////////////////////////////
// Rules private members
//////////////////////////////////////////
// template <typename TilePair>
// uint64_t Rules<TilePair>::hash_rule(const TilePair& rule) {
//     uint64_t hash = 0xcbf29ce484222325;
//     if constexpr (std::is_same<TilePair, transition_t>::value) {
//         hash ^= rule.tile_a;
//         hash *= 0x100000001b3;
//         hash ^= rule.tile_b;
//         hash *= 0x100000001b3;
//     } else if constexpr (std::is_same<TilePair, affinity_t>::value) {
//         hash ^= rule.tile_a;
//         hash *= 0x100000001b3;
//     }
//     return hash;
// }

// template <typename TilePair>
// void Rules<TilePair>::resize(size_t new_size) {
//     std::vector<TilePair> new_table(new_size);
//     // Rehash all elements into the new table
//     for (const auto& rule : table) {
//         if (rule.tile_a != EMPTY_TILE) {
//             size_t index = find_index(rule.tile_a, rule.tile_b);
//             new_table[index] = rule;
//         }
//     }
//     table = std::move(new_table);
// }

// template <typename TilePair>
// bool Rules<TilePair>::is_prime(size_t n) {
//     if (n <= 1) return false;
//     if (n <= 3) return true;
//     if (n % 2 == 0 || n % 3 == 0) return false;
//     for (size_t i = 5; i * i <= n; i += 6) {
//         if (n % i == 0 || n % (i + 2) == 0) return false;
//     }
//     return true;
// }

// template <typename TilePair>
// size_t Rules<TilePair>::next_prime(size_t n) {
// 	// yikes
//     while (!is_prime(n)) {
//         n++;
//     }
//     return n;
// }

// Insert a rule into the hash table
template <typename TilePair>
void Rules<TilePair>::insert(const TilePair& rule) {

	if constexpr (std::is_same<TilePair, affinity_t>::value) {
		TilePair temp_rule = {rule.tile_a, rule.tile_b};
		temp_rule.hash_tile_a = true;
		table[hash_rule(temp_rule)] = temp_rule;

		temp_rule.hash_tile_a = false;
		table_b[hash_rule(temp_rule)] = temp_rule;

		num_elements ++;
	} else {
		table[hash_rule(rule)] = rule;
		num_elements++;
	}
}

// Find a rule in the hash table
// template <typename TilePair>
// TilePair Rules<TilePair>::find(tile_t tile_a, tile_t tile_b) {
//     TilePair temp_rule = {tile_a, tile_b};
//     uint64_t key = hash_rule(temp_rule);

//     auto it = table.find(key);
//     if (it != table.end()) {
//         return it->second;
//     } else {
//         return get_invalid_value<TilePair>();
//     }
// }

template <typename TilePair>
TilePair Rules<TilePair>::find(tile_t tile_a, tile_t tile_b, bool use_tile_a) {
	uint64_t key;
	TilePair temp_rule = {tile_a, tile_b};

    if constexpr (std::is_same<TilePair, affinity_t>::value) {
        temp_rule.hash_tile_a = use_tile_a;
	}
	key = hash_rule(temp_rule);

	if constexpr (std::is_same<TilePair, affinity_t>::value) {
		if (!use_tile_a) {
			auto it = table_b.find(key);
			if (it != table_b.end()) {
				return it->second;
			}
		} else {
			auto it = table.find(key);
			if (it != table.end()) {
				return it->second;
			}
		}
	} else {
		auto it = table.find(key);
		if (it != table.end()) {
			return it->second;
		}
	}

    return get_invalid_value<TilePair>();
}


// Get the load factor of the table
template <typename TilePair>
double Rules<TilePair>::load_factor() const {
    return (double)num_elements / table.size();
}

// Hash function for a rule
template <typename TilePair>
uint64_t Rules<TilePair>::hash_rule(const TilePair& rule) {
    uint64_t hash = 0xcbf29ce484222325;
    if constexpr (std::is_same<TilePair, transition_t>::value) {
        hash ^= rule.tile_a;
        hash *= 0x100000001b3;
        hash ^= rule.tile_b;
        hash *= 0x100000001b3;
    } else if constexpr (std::is_same<TilePair, affinity_t>::value) {
		if (rule.hash_tile_a) hash ^= rule.tile_a;
		else hash ^= rule.tile_b;

        hash *= 0x100000001b3;
    }
    return hash;
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
    return (rule.tile_a == EMPTY_TILE || rule.tile_b == EMPTY_TILE);
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
	size_t plus_pos = rule_str.find('+');
    size_t comma_pos = rule_str.find(',');

    std::string tile_a_str = rule_str.substr(0, plus_pos);
    std::string tile_b_str = rule_str.substr(plus_pos + 1, comma_pos - plus_pos - 1);
    int strength = std::stoi(rule_str.substr(comma_pos + 1));

    affinity_t rule;
    rule.tile_a = Tile::encode(tile_a_str);
    rule.tile_b = Tile::encode(tile_b_str);

    return rule;
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
void load(std::string filepath, Rules<TilePair>& rules) {
    std::cout << "Parsing rules from " << filepath << "\n";

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

    rules = Rules<TilePair>(rule_count);

    for (int i = 0; i < rule_count; i++) {
        std::getline(file, line);

        // If the line begins with #, skip it.
        if (line[0] == '#') continue;

        auto rule = parse_rule<TilePair>(line);
        rules.insert(rule);

        // std::cout << "Find test " << rules.find_index(Tile::encode("DhIR"), Tile::encode("XTNi")) << "\n";


        if (i % 1000000 == 0 && i != 0) {
            std::cout << "Parsed " << i << " rules\n";
        }
    }

    file.close();

    // std::cout << "Finished parsing rules " << rules.find_index(Tile::encode("DhIR"), Tile::encode("XTNi")) << "\n";
}

template class Rules<transition_t>;
template class Rules<affinity_t>;
template void load<transition_t>(std::string filepath, Rules<transition_t>& rules);
template void load<affinity_t>(std::string filepath, Rules<affinity_t>& rules);
// template transition_t parse_rule<transition_t>(std::string rule_str);
// template affinity_t parse_rule<affinity_t>(std::string rule_str);
template bool is_invalid(transition_t& rule);
template bool is_invalid(affinity_t& rule);

} // namespace Rules
