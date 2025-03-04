import string
import random
 
def generate_random_state():
    s = ""
    for _ in range(4):
        s += random.choice(string.ascii_letters)

    return s


map = {}

INPUT_FILE = "line/line.xml"
OUTPUT_FILE = "line/line"

with open(INPUT_FILE, 'r') as f:
    data = f.read()

data = data.split('\n')
states_xml, vtrans_xml, htrans_xml, vaff_xml, haff_xml = data[1], data[5], data[6], data[7], data[8] # these numbers correspond to the siepinski triangle system

original_states = []
states, vtrans, htrans, vaff, haff = [], [], [], [], []

# Get all states
states_xml = states_xml.split('/>')[:-1]
for sxml in states_xml:
    sxml = sxml.split("\"")
    original_states.append(sxml[1])

for s in original_states:
    while True:
        random_state = generate_random_state()

        if random_state not in states:
            map[s] = random_state
            states.append(random_state)
            break

# Get all vertical transitions
vtrans_xml = vtrans_xml.split('/>')[:-1]
for vxml in vtrans_xml:
    vxml = vxml.split("\"")
    s1, s2 = vxml[1], vxml[3]
    s1_final, s2_final = vxml[5], vxml[7]

    if s1 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s1] = random_state
                states.append(random_state)
                break
    if s2 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s2] = random_state
                states.append(random_state)
                break
    if s1_final not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s1_final] = random_state
                states.append(random_state)
                break
    if s2_final not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s2_final] = random_state
                states.append(random_state)
                break

    vtrans.append([map[s1], map[s2], map[s1_final], map[s2_final]])

# Get all horizontal transitions
htrans_xml = htrans_xml.split('/>')[:-1]
for hxml in htrans_xml:
    hxml = hxml.split("\"")
    s1, s2 = hxml[1], hxml[3]
    s1_final, s2_final = hxml[5], hxml[7]

    if s1 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s1] = random_state
                states.append(random_state)
                break
    if s2 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s2] = random_state
                states.append(random_state)
                break
    if s1_final not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s1_final] = random_state
                states.append(random_state)
                break
    if s2_final not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s2_final] = random_state
                states.append(random_state)
                break

    htrans.append([map[s1], map[s2], map[s1_final], map[s2_final]])    

# Get all vertical affinities
vaff_xml = vaff_xml.split('/>')[:-1]
for vxml in vaff_xml:
    vxml = vxml.split("\"")
    s1, s2, strength = vxml[1], vxml[3], vxml[5]

    if s1 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s1] = random_state
                states.append(random_state)
                break
    if s2 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s2] = random_state
                states.append(random_state)
                break

    vaff.append([map[s1], map[s2], strength])

# Get all horizontal affinities
haff_xml = haff_xml.split('/>')[:-1]
for hxml in haff_xml:
    hxml = hxml.split("\"")
    s1, s2, strength = hxml[1], hxml[3], hxml[5]

    if s1 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s1] = random_state
                states.append(random_state)
                break
    if s2 not in map: 
        while True:
            random_state = generate_random_state()

            if random_state not in states:
                map[s2] = random_state
                states.append(random_state)
                break

    haff.append([map[s1], map[s2], strength])

# Writing to files
with open(f'./{OUTPUT_FILE}.seed', 'w') as f:
    f.write(map['seed'] + ', ' + str(0) + ', ' + str(0) + '\n')
    f.write(map['Nl'] + ', ' + str(1) + ', ' + str(0) + '\n')
    f.write(map['Sl'] + ', ' + str(1) + ', ' + str(1) + '\n')

with open(f'./{OUTPUT_FILE}.states', 'w') as f:
    f.write(str(len(states)) + " States\n")
    for s in states:
        f.write(s + '\n')

with open(f'./{OUTPUT_FILE}.vtrans', 'w') as f:
    f.write(str(len(vtrans)) + " Rules\n")
    for [s1, s2, s1_final, s2_final] in vtrans:
        f.write(s1+'+'+s2+'->'+s1_final+'+'+s2_final+'\n')

with open(f'./{OUTPUT_FILE}.htrans', 'w') as f:
    f.write(str(len(htrans)) + " Rules\n")
    for [s1, s2, s1_final, s2_final] in htrans:
        f.write(s1+'+'+s2+'->'+s1_final+'+'+s2_final+'\n')

with open(f'./{OUTPUT_FILE}.vaff', 'w') as f:
    f.write(str(len(vaff)) + " Affinities\n")
    for [s1, s2, strength] in vaff:
        f.write(s1+'+'+s2+','+strength+'\n')

with open(f'./{OUTPUT_FILE}.haff', 'w') as f:
    f.write(str(len(haff)) + " Affinities\n")
    for [s1, s2, strength] in haff:
        f.write(s1+'+'+s2+','+strength+'\n')

# Write the key mappings to a file
with open(f'./{OUTPUT_FILE}.key', 'w') as key_file:
    for original, encoded in map.items():
        key_file.write(f"{encoded}\t{original}\n")