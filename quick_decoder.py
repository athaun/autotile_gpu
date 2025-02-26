EMPTY_TILE = 0xEFFFFFFF

def encode(s: str) -> int:
    """Encodes a string into a uint32_t value."""
    result = 0
    for i in range(min(len(s), 4)):
        result |= (ord(s[i]) & 0xFF) << (i * 8)
    return result

def decode(value: int) -> str:
    """Decodes a uint32_t value into a string."""
    if value == EMPTY_TILE:
        return "[__]"
    result = "".join(chr((value >> (i * 8)) & 0xFF) for i in range(4))
    return result.rstrip("\x00")  # Remove trailing null characters

while True:
    # choice = input("Encode (e) or decode (d)? ").strip().lower()
    # if choice == "e":
    #     input_str = input("Enter a string (max 4 characters): ")[:4]
    #     encoded = encode(input_str)
    #     print(f"Encoded: {encoded:#010x}")
    # elif choice == "d":
    input_value = int(input("Encoded uint32_t (hex or decimal): "), 0)
    decoded = decode(input_value)
    print(f"Decoded: {decoded}")
    # else:
    #     print("Invalid choice. Please enter 'e' for encode or 'd' for decode.")
