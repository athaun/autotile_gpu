import string
import random
import xml.etree.ElementTree as ET

def generate_random_state(existing_states):
    while True:
        state = "".join(random.choices(string.ascii_letters, k=4))
        if state not in existing_states:
            return state

def parse_states(xml_root):
    state_map = {}
    states = []
    for state in xml_root.findall(".//State"):
        label = state.get("Label")
        if label and label not in state_map:
            random_state = generate_random_state(states)
            state_map[label] = random_state
            states.append(random_state)
    return state_map, states

def parse_rules(xml_root, state_map, rule_tag, rule_format):
    rules = []
    for rule in xml_root.findall(f".//{rule_tag}/Rule"):
        labels = [rule.get(attr) for attr in rule_format]
        mapped_labels = [state_map[label] for label in labels if label in state_map]
        if len(mapped_labels) == len(labels):
            rules.append(mapped_labels)
    return rules

def parse_affinities(xml_root, state_map, affinity_tag):
    affinities = []
    for affinity in xml_root.findall(f".//{affinity_tag}/Rule"):
        label1, label2, strength = affinity.get("Label1"), affinity.get("Label2"), affinity.get("Strength")
        if label1 in state_map and label2 in state_map:
            affinities.append([state_map[label1], state_map[label2], strength])
    return affinities

def parse_seed_tiles(xml_root, state_map):
    seeds = []
    tiles_section = xml_root.findall(".//Tiles/Tile")
    if tiles_section:
        for tile in tiles_section:
            label = tile.get("Label")
            x, y = tile.get("x"), tile.get("y")
            if label in state_map and x is not None and y is not None:
                seeds.append([state_map[label], x, y])
    else:
        seed_state = xml_root.find(".//SeedStates/State")
        if seed_state is not None:
            label = seed_state.get("Label")
            if label in state_map:
                seeds.append([state_map[label], "0", "0"])
    return seeds

def main(input_file, output_file):
    tree = ET.parse(input_file)
    root = tree.getroot()
    
    state_map, states = parse_states(root)
    vtrans = parse_rules(root, state_map, "VerticalTransitions", ["Label1", "Label2", "Label1Final", "Label2Final"])
    htrans = parse_rules(root, state_map, "HorizontalTransitions", ["Label1", "Label2", "Label1Final", "Label2Final"])
    vaff = parse_affinities(root, state_map, "VerticalAffinities")
    haff = parse_affinities(root, state_map, "HorizontalAffinities")
    seeds = parse_seed_tiles(root, state_map)
    
    with open(f'{output_file}.seed', 'w') as f:
        for seed in seeds:
            f.write(", ".join(seed) + '\n')
    
    with open(f'{output_file}.states', 'w') as f:
        f.write(f"{len(states)} States\n")
        for s in states:
            f.write(s + '\n')
    
    with open(f'{output_file}.vtrans', 'w') as f:
        f.write(f"{len(vtrans)} Rules\n")
        for rule in vtrans:
            f.write("+".join(rule[:2]) + "->" + "+".join(rule[2:]) + '\n')
    
    with open(f'{output_file}.htrans', 'w') as f:
        f.write(f"{len(htrans)} Rules\n")
        for rule in htrans:
            f.write("+".join(rule[:2]) + "->" + "+".join(rule[2:]) + '\n')
    
    with open(f'{output_file}.vaff', 'w') as f:
        f.write(f"{len(vaff)} Affinities\n")
        for aff in vaff:
            f.write("+".join(aff[:2]) + "," + aff[2] + '\n')
    
    with open(f'{output_file}.haff', 'w') as f:
        f.write(f"{len(haff)} Affinities\n")
        for aff in haff:
            f.write("+".join(aff[:2]) + "," + aff[2] + '\n')
    
    with open(f'{output_file}.key', 'w') as f:
        for original, encoded in state_map.items():
            f.write(f"{encoded}\t{original}\n")

if __name__ == "__main__":
    
    system = "fractal2/fractal"
    print("HELLO")
    INPUT_FILE = f"{system}.xml"
    print("HELLO")
    OUTPUT_FILE = f"{system}"
    print(f"HELLO printing {system} {INPUT_FILE} {OUTPUT_FILE}")
    main(INPUT_FILE, OUTPUT_FILE)