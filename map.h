#ifndef MAP_H_
#define MAP_H_

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

// Map positions with this value mean not present (i.e. the position is not
// occupied by any Blink). Callers should not use this as a valid value.
#define MAP_POSITION_EMPTY 0

namespace mapping {

namespace map {

// Sets the given value at position x,y. Note that there is no bounds checking
// (to save storage space) so you must make sure that x is in
// [-MAPPING_MAP_WIDTH, MAPPING_MAP_WIDTH) and y is in [-MAPPING_MAP_HEIGHT,
// MAPPING_MAP_HEIGHT) and that the highest_x - lowest_x < MAPPING_MAP_WIDTH and
// highest_y - lowest_y < MAPPING_MAP_HEIGHT.
void Set(int8_t x, int8_t y, byte value);

// Returns the value at position x,y.
byte Get(int8_t x, int8_t y);

// Resets the map, making it empty.
void Reset();

}  // namespace map

}  // namespace mapping

#endif  // MAP_H_