#!/usr/bin/env python3
"""
Generate screen_map.json mapping Amulet page indices to screen names.

Reads #define XXXXX_HTM 0xNN from Amulet.h, matches each define name
to actual HTM filenames (case-insensitive), and outputs a JSON mapping
of integer page index -> screen name (matching the JSON filename without extension).
"""

import json
import os
import re
import sys

AMULET_H = os.path.join(
    os.path.dirname(__file__),
    "..",
    "firmware",
    "CRC-25R_Calib",
    "Headers",
    "Amulet.h",
)

HTM_DIR = os.path.join(
    os.path.dirname(__file__),
    "..",
    "firmware",
    "CRCHtml_3.00a",
)

SCREEN_DEFS_DIR = os.path.join(
    os.path.dirname(__file__),
    "htm_parser",
    "screen_defs",
)

OUTPUT_FILE = os.path.join(
    os.path.dirname(__file__),
    "..",
    "frontend",
    "public",
    "screen_map.json",
)


def parse_htm_defines(header_path):
    """Parse all #define XXXX_HTM 0xNN lines from Amulet.h."""
    defines = {}
    pattern = re.compile(r"^#define\s+(\w+)_HTM\s+(0x[0-9A-Fa-f]+)")
    with open(header_path, "r") as f:
        for line in f:
            m = pattern.match(line.strip())
            if m:
                name = m.group(1)
                hex_val = int(m.group(2), 16)
                defines[name] = hex_val
    return defines


def get_htm_filenames(htm_dir):
    """Get all .htm filenames (without extension) from the HTM directory."""
    names = []
    for fname in os.listdir(htm_dir):
        if fname.lower().endswith(".htm"):
            names.append(os.path.splitext(fname)[0])
    return names


def get_screen_def_names(screen_defs_dir):
    """Get all .json filenames (without extension) from screen_defs directory."""
    names = []
    if os.path.isdir(screen_defs_dir):
        for fname in os.listdir(screen_defs_dir):
            if fname.lower().endswith(".json"):
                names.append(os.path.splitext(fname)[0])
    return names


def match_define_to_filename(define_name, htm_filenames):
    """
    Match a define name (e.g. MAINSCREEN, WELLSETUPBIOASSAY) to an actual
    HTM filename (e.g. MainScreen, WellSetupBioAssay) using case-insensitive
    comparison.
    """
    define_lower = define_name.lower()
    for fname in htm_filenames:
        if fname.lower() == define_lower:
            return fname
    return None


def main():
    # Resolve paths
    header_path = os.path.abspath(AMULET_H)
    htm_dir = os.path.abspath(HTM_DIR)
    screen_defs_dir = os.path.abspath(SCREEN_DEFS_DIR)
    output_path = os.path.abspath(OUTPUT_FILE)

    print(f"Reading defines from: {header_path}")
    print(f"HTM directory: {htm_dir}")
    print(f"Screen defs directory: {screen_defs_dir}")
    print(f"Output: {output_path}")
    print()

    # Parse defines
    defines = parse_htm_defines(header_path)
    print(f"Found {len(defines)} _HTM defines in Amulet.h")

    # Get actual filenames
    htm_filenames = get_htm_filenames(htm_dir)
    screen_def_names = get_screen_def_names(screen_defs_dir)
    print(f"Found {len(htm_filenames)} .htm files")
    print(f"Found {len(screen_def_names)} screen def .json files")
    print()

    # Build case-insensitive lookup for screen defs (prefer screen_def name if it exists)
    screen_def_lookup = {name.lower(): name for name in screen_def_names}

    # Build the mapping
    screen_map = {}
    unmatched = []

    for define_name, page_index in sorted(defines.items(), key=lambda x: x[1]):
        # First try to match against screen_defs JSON names (case-insensitive)
        screen_name = screen_def_lookup.get(define_name.lower())

        if screen_name is None:
            # Fall back to HTM filenames
            screen_name = match_define_to_filename(define_name, htm_filenames)

        if screen_name:
            screen_map[str(page_index)] = screen_name
            print(f"  0x{page_index:02X} ({page_index:3d}) -> {define_name}_HTM -> {screen_name}")
        else:
            unmatched.append((define_name, page_index))
            print(f"  0x{page_index:02X} ({page_index:3d}) -> {define_name}_HTM -> *** NO MATCH ***")

    print()
    print(f"Matched: {len(screen_map)}")
    print(f"Unmatched: {len(unmatched)}")

    if unmatched:
        print("\nUnmatched defines:")
        for name, idx in unmatched:
            print(f"  {name}_HTM (0x{idx:02X})")

    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    # Write JSON output
    with open(output_path, "w") as f:
        json.dump(screen_map, f, indent=2, sort_keys=lambda x: int(x))

    # Actually sort_keys won't work with a lambda for json.dump.
    # Let's write it manually with proper integer-sorted keys.
    sorted_map = {}
    for key in sorted(screen_map.keys(), key=int):
        sorted_map[key] = screen_map[key]

    with open(output_path, "w") as f:
        json.dump(sorted_map, f, indent=2)

    print(f"\nWrote {len(sorted_map)} entries to {output_path}")


if __name__ == "__main__":
    main()
