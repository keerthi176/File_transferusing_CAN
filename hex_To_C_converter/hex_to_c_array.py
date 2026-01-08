import sys
import os

# FIXED: Match your bootloader's APP_FIRMWARE_START_ADDRESS
APP_START_ADDRESS = 0x10000 
APP_END_ADDRESS   = 0x80000 # 512KB Total Flash Size

def parse_intel_hex(hex_file, start_addr, end_addr):
    """
    Manual Intel HEX parser to avoid dependencies.
    Extracts data in the range [start_addr, end_addr)
    """
    flash_size = end_addr - start_addr
    # Initialize with 0xFF (Unprogrammed Flash)
    flash = bytearray([0xFF] * flash_size)
    
    extended_linear_address = 0
    max_addr_written = start_addr
    data_found = False

    try:
        with open(hex_file, 'r', encoding='utf-8') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line: continue
                if not line.startswith(':'): continue
                
                try:
                    byte_count = int(line[1:3], 16)
                    address = int(line[3:7], 16)
                    record_type = int(line[7:9], 16)
                    data_hex = line[9:9 + byte_count * 2]
                    
                    # Full 32-bit address
                    full_addr = extended_linear_address + address
                    
                    if record_type == 0: # Data Record
                        for i in range(byte_count):
                            target_addr = full_addr + i
                            if start_addr <= target_addr < end_addr:
                                val = int(data_hex[i*2:i*2+2], 16)
                                flash[target_addr - start_addr] = val
                                data_found = True
                                if (target_addr + 1) > max_addr_written:
                                    max_addr_written = target_addr + 1
                                    
                    elif record_type == 4: # Extended Linear Address Record
                        extended_linear_address = int(line[9:13], 16) << 16
                    elif record_type == 2: # Extended Segment Address Record
                        extended_linear_address = int(line[9:13], 16) << 4
                    elif record_type == 1: # End of File
                        break
                except ValueError:
                    print(f"Warning: Skipping malformed line {line_num}")
    except FileNotFoundError:
        print(f"Error: File '{hex_file}' not found.")
        return None

    if not data_found:
        return None
        
    # Return only the used portion
    return flash[:max_addr_written - start_addr]

def hex_to_c_array(input_hex, output_h):
    filename = os.path.basename(input_hex)
    print(f"Processing {filename}...")

    binary_data = parse_intel_hex(input_hex, APP_START_ADDRESS, APP_END_ADDRESS)

    if binary_data is None:
        print(f"Error: No data found in HEX file for address range 0x{APP_START_ADDRESS:X} to 0x{APP_END_ADDRESS:X}")
        return

    data_len = len(binary_data)
    
    with open(output_h, 'w', encoding='utf-8', newline='\n') as f:
        # Header comments
        f.write(f"// Generated from {filename}\n")
        f.write(f"// Range: 0x{APP_START_ADDRESS:08X} - 0x{APP_START_ADDRESS + data_len:08X}\n")
        f.write(f"// Size: {data_len} bytes\n\n")
        
        # Guard
        f.write("#ifndef FIRMWARE_IMAGE_H_\n")
        f.write("#define FIRMWARE_IMAGE_H_\n\n")
        f.write("#include <stdint.h>\n\n")

        # Array Length
        f.write(f"const uint32_t firmware_len = {data_len};\n\n")

        # Array Data
        f.write("const uint8_t firmware_data[] = {\n")
        
        # Write bytes with proper formatting
        for i, byte in enumerate(binary_data):
            if i % 16 == 0:
                f.write("    ") # Indent
            
            f.write(f"0x{byte:02X}") # UPPERCASE HEX
            
            # Add comma if not the very last byte
            if i < data_len - 1:
                f.write(", ")
            
            # Newline every 16 bytes
            if (i + 1) % 16 == 0:
                f.write("\n")
            
        f.write("\n};

")
        f.write("#endif /* FIRMWARE_IMAGE_H_ */\n")

    print(f"Success! {output_h} generated. ({data_len} bytes)")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python hex_to_c_array.py <input.hex> <output.h>")
    else:
        hex_to_c_array(sys.argv[1], sys.argv[2])