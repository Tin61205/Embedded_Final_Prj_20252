#!/usr/bin/env python3
"""
Map Converter for Pacman STM32 Project
=======================================
Converts a text file map into a C source file (const array)
that can be compiled directly into the firmware.

Text file format:
  Line 1:  ROWS COLS        (e.g. "9 25")
  Lines 2+: space-separated cell values
    0 = path with normal food (small dot)
    1 = wall
    2 = path with energy food (big dot)
    3 = path with NO food (for spawn points etc.)

The map is centered into the 28x31 game grid.
Borders (row 0, row 30, col 0, col 27) are always walls.

Usage:
  python map_converter.py input.txt [MAP_NAME]
  
  MAP_NAME defaults to the input filename without extension.
  Output goes to stdout — redirect to a .c file.

Example:
  python map_converter.py my_level.txt MY_LEVEL > maze_data_my_level.c
"""

import sys
import os

GRID_W = 28   # ROOM_CNT_X
GRID_H = 31   # ROOM_CNT_Y

# Cell values in the output C array
CELL_WALL        = 1
CELL_PATH_FOOD   = 0
CELL_PATH_ENERGY = 2
CELL_PATH_EMPTY  = 3


def parse_map(filename):
    with open(filename, 'r') as f:
        raw_lines = f.readlines()

    # Filter out empty or whitespace-only lines
    lines = [line.strip() for line in raw_lines if line.strip()]

    if not lines:
        raise ValueError("File is empty")

    # Parse header
    header = lines[0].split()
    rows = int(header[0])
    cols = int(header[1])

    grid = []
    for i in range(1, len(lines)):
        values = lines[i].split()
        if len(values) != cols:
            raise ValueError(f"Data row {i}: expected {cols} values, got {len(values)}")
        row = [int(v) for v in values]
        grid.append(row)

    if len(grid) != rows:
        raise ValueError(f"Expected {rows} data rows, but found {len(grid)}")

    # Auto-transpose if columns exceed grid width but fits if transposed
    if cols > GRID_W and cols <= GRID_H and rows <= GRID_W:
        sys.stderr.write("Note: Auto-transposing map (rotating 90 degrees) to fit the 28x31 screen layout.\n")
        grid = list(map(list, zip(*grid)))
        rows, cols = cols, rows

    return rows, cols, grid


def center_map(rows, cols, grid):
    """Center the user map into the 28x31 game grid.
    
    If the map size is smaller than the grid size, it is centered.
    """
    if cols > GRID_W:
        raise ValueError(f"Map width {cols} exceeds grid width {GRID_W}")
    if rows > GRID_H:
        raise ValueError(f"Map height {rows} exceeds grid height {GRID_H}")

    # Calculate offset to center in the full grid
    off_x = (GRID_W - cols) // 2
    off_y = (GRID_H - rows) // 2

    # Initialize full grid as walls
    full = [[CELL_WALL] * GRID_W for _ in range(GRID_H)]

    # Copy user map into the centered position
    for r in range(rows):
        for c in range(cols):
            gx = off_x + c
            gy = off_y + r
            full[gy][gx] = grid[r][c]

    return full, off_x, off_y


def generate_c_source(full_grid, map_name):
    """Generate the C source with the const map data array.
    
    The array is stored as [GRID_H][GRID_W] (row-major: grid[y][x]).
    In the firmware, it will be accessed as maze_txt_data[MAP_ID][y * ROOM_CNT_X + x].
    """
    upper_name = map_name.upper()

    lines = []
    lines.append(f"//--------------------------------------------------------------")
    lines.append(f"// Auto-generated map data: {map_name}")
    lines.append(f"// Grid size: {GRID_W} x {GRID_H}")
    lines.append(f"// Cell values: 0=path+food, 1=wall, 2=path+energy, 3=path+empty")
    lines.append(f"//--------------------------------------------------------------")
    lines.append(f"")
    lines.append(f"#include \"maze_txtmap.h\"")
    lines.append(f"")
    lines.append(f"// Map data: {map_name} [{GRID_H} rows x {GRID_W} cols]")
    lines.append(f"// Access as: maze_txtmap_{map_name.lower()}[y * ROOM_CNT_X + x]")
    lines.append(f"const uint8_t maze_txtmap_{map_name.lower()}[MAZE_TXTMAP_SIZE] = {{")

    for y in range(GRID_H):
        row_str = "    "
        for x in range(GRID_W):
            row_str += f"{full_grid[y][x]},"
        row_str += f"  // y={y}"
        lines.append(row_str)

    lines.append(f"}};")
    lines.append(f"")

    return "\n".join(lines)


def print_ascii_preview(full_grid):
    """Print an ASCII preview of the map to stderr for visual verification."""
    chars = {
        CELL_WALL: '█',
        CELL_PATH_FOOD: '·',
        CELL_PATH_ENERGY: '●',
        CELL_PATH_EMPTY: ' ',
    }
    sys.stderr.write("\n--- Map Preview ---\n")
    for y in range(GRID_H):
        row = ""
        for x in range(GRID_W):
            row += chars.get(full_grid[y][x], '?')
        sys.stderr.write(row + "\n")
    sys.stderr.write("--- End Preview ---\n\n")


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} input.txt [MAP_NAME]", file=sys.stderr)
        sys.exit(1)

    input_file = sys.argv[1]
    if len(sys.argv) >= 3:
        map_name = sys.argv[2]
    else:
        map_name = os.path.splitext(os.path.basename(input_file))[0]

    rows, cols, grid = parse_map(input_file)
    sys.stderr.write(f"Parsed map: {rows} rows x {cols} cols\n")

    full_grid, off_x, off_y = center_map(rows, cols, grid)
    sys.stderr.write(f"Centered at offset: x={off_x}, y={off_y}\n")

    print_ascii_preview(full_grid)

    c_source = generate_c_source(full_grid, map_name)
    print(c_source)

    sys.stderr.write(f"Done. Redirect stdout to save the .c file.\n")


if __name__ == "__main__":
    main()
