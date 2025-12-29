#  Converts a binary file into a C header file containing the binary data as a byte array.
#    Usage: python bin_to_c_header.py <input_binary_file> <output_header_file> 
#    go to terminal and give the command as  python bin_to_c.py <file_path_location> <name_of the genearted_C file>

import sys
import os

def bin_to_c_header(input_filepath, output_filepath):

    try:
        with open(input_filepath, 'rb') as f_in:
            binary_data = f_in.read()

        file_size = len(binary_data)
        
        var_name = os.path.basename(input_filepath)
        var_name = os.path.splitext(var_name)[0] 
        var_name = "".join(c if c.isalnum() else "_" for c in var_name)

        with open(output_filepath, 'w') as f_out:
            f_out.write(f"// Generated from {os.path.basename(input_filepath)}\n")
            f_out.write(f"// File size: {file_size} bytes\n\n")
            f_out.write(f"const unsigned int {var_name}_len = {file_size};\n")
            f_out.write(f"const unsigned char {var_name}_data[] = {{\n")

            for i, byte in enumerate(binary_data):
                if i % 16 == 0:
                    f_out.write("    ")
                f_out.write(f"0x{byte:02x}")
                if i < file_size - 1:
                    f_out.write(", ")
                if (i + 1) % 16 == 0 and i < file_size - 1:
                    f_out.write("\n")
            f_out.write("\n};")
        
        print(f"Successfully converted '{input_filepath}' to '{output_filepath}'.")

    except FileNotFoundError:
        print(f"Error: Input file '{input_filepath}' not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python bin_to_c_header.py <input_binary_file> <output_header_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    bin_to_c_header(input_file, output_file)
