import random
import string

# Constants
NUM_RULES = 3_000_000
TILE_NAME_LENGTH = 4  # Adjust the length as needed (less than 64 bits)
BATCH_SIZE = 1_000_000  # Number of rules to write per batch
STATUS_INTERVAL = 10_000_000  # Print status every 10 million rules

def generate_tile_name(length):
    """Generate a random tile name of the given length."""
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def generate_rule():
    """Generate a single rule in the format a+b->c+d."""
    a = generate_tile_name(TILE_NAME_LENGTH)
    b = generate_tile_name(TILE_NAME_LENGTH)
    c = generate_tile_name(TILE_NAME_LENGTH)
    d = generate_tile_name(TILE_NAME_LENGTH)
    return f"{a}+{b}->{c}+{d}"

def main():
    with open('rules_3m.txt', 'w') as file:
        file.write(f"{NUM_RULES} Rules\n")

        rules_buffer = []
        for i in range(NUM_RULES):
            rule = generate_rule()
            rules_buffer.append(rule)

            # Write rules in batches
            if len(rules_buffer) >= BATCH_SIZE:
                file.write('\n'.join(rules_buffer) + '\n')
                rules_buffer = []

                # Print status update
                if (i + 1) % STATUS_INTERVAL == 0:
                    print(f"Generated and wrote {i + 1} rules.")

        # Write any remaining rules
        if rules_buffer:
            file.write('\n'.join(rules_buffer) + '\n')

    print(f"Generated {NUM_RULES} rules and saved to 'rules.txt'.")

if __name__ == "__main__":
    main()
