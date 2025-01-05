import argparse
import json
import sys
from pathlib import Path

header_config_template = """#ifndef CONFIG_H
#define CONFIG_H
#include <string>

using namespace std::string_literals;

std::string default_json_config = "{json_config}"s;

#endif
"""


def escape_for_cpp_literal(py_string):
    # Escape double quotes and backslashes
    return py_string.replace("\\", "\\\\").replace('"', '\\"')


def main():
    parser = argparse.ArgumentParser(
        description="Process a JSON config file and write it to an config.h output file."
    )
    parser.add_argument("json_config", type=str, help="Path to the input JSON file.")
    parser.add_argument("header_config", type=str, help="Path to the output file.")
    args = parser.parse_args()

    # Validate JSON file existence
    json_file = Path(args.json_config)
    if not json_file.exists():
        print(f"Error: The file '{json_file}' does not exist.")
        sys.exit(1)
    try:
        with json_file.open("r") as jf:
            data = json.load(jf)
    except json.JSONDecodeError:
        print(f"Error: The file '{json_file}' is not a valid JSON file.")
        sys.exit(2)

    json_config = escape_for_cpp_literal(
        json.dumps(data, indent=None, separators=(",", ":"))
    )
    header_config_file = Path(args.header_config)
    header_config_file.touch(exist_ok=True)
    header_config_file.write_text(
        header_config_template.format(json_config=json_config)
    )

    # Write content or processed information to output file
    # with open(args.output_file, "w") as output:
    #     output.write(json.dumps(data, indent=4))
    # print(f"Data successfully written to '{args.output_file}'.")


if __name__ == "__main__":
    main()
