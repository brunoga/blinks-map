#ifndef MAPPING_H_
#define MAPPING_H_

#include <blinklib.h>

// Map abstraction for Blinks. This supports an arbitrary-sized rectangular map
// that accepts negative coordinates but internaly maps them to positive
// coordinates in a 2d array. This is well suited for using axial coordinates
// for hexagonal grids (x + y + z = 0, so we only need to store x and y).
//
// Note that depending on how the coordinate system is set up, there is a
// potential for wasted positions so even if the total number of used positions
// is smaller than MAPPING_MAP_WIDTH * MAPPING_MAP_HEIGHT, entries might still
// not fit in the map.
//
// Map size (width and height) can be set in the config/mapping_config.h file.

// Mapping positions with this value mean not present (i.e. the position is not
// occupied by any Blink). Callers should not use this as a valid value.
#define MAPPING_POSITION_EMPTY 0

namespace mapping {

typedef bool (*PositionHandler)(int8_t x, int8_t y, byte* value, void* context);

// Sets the given value at position x,y. Note that there is no bounds checking
// (to save storage space) so you must make sure that x is in
// [-MAPPING_MAP_WIDTH, MAPPING_MAP_WIDTH) and y is in [-MAPPING_MAP_HEIGHT,
// MAPPING_MAP_HEIGHT) and that the highest_x - lowest_x < MAPPING_MAP_WIDTH and
// highest_y - lowest_y < MAPPING_MAP_HEIGHT.
void Set(int8_t x, int8_t y, byte value);

// Returns the value at position x,y. Requests for positions that would fall
// outside of the map return MAPPING_POSITION_EMPTY.
byte Get(int8_t x, int8_t y);

// Calls the given position_handler for every valid (set) position in the map.
bool AllValidPositions(PositionHandler position_handler, void* context);

// Calls the given position_handler for every valid (set) position that is at
// most at the given distance from the x0,y0 coordinates. We are dealing with
// hexagons so there are 6 neighbors at distance 1.
bool AllValidPositionsAround(int8_t x0, int8_t y0, byte distance,
                             PositionHandler position_handler, void* context);

// Returns true if the mapping has started (Set() was called at least once
// since the last Reset() call or since the Blink started).
bool Initialized();

// Resets the map, making it empty.
void Reset();

}  // namespace mapping

#endif  // MAPPING_H_