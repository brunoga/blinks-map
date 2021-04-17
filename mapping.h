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

struct Iterator {
  int8_t start_x = 0;
  int8_t start_y = 0;

  int8_t curr_x = 0;
  int8_t curr_y = 0;

  int8_t end_x = 0;
  int8_t end_y = 0;

  bool initialized = false;
};

// Sets the given value at position x,y. Note that there is no bounds checking
// (to save storage space) so you must make sure that x is in
// [-MAPPING_MAP_WIDTH, MAPPING_MAP_WIDTH) and y is in [-MAPPING_MAP_HEIGHT,
// MAPPING_MAP_HEIGHT) and that the highest_x - lowest_x < MAPPING_MAP_WIDTH and
// highest_y - lowest_y < MAPPING_MAP_HEIGHT.
void Set(int8_t x, int8_t y, byte value);

// Returns the value at position x,y. Requests for positions that would fall
// outside of the map return MAPPING_POSITION_EMPTY.
byte Get(int8_t x, int8_t y);

// Returns the value of the next valid (occupied) position. The given iterator
// is updated with the position returned so a following call using the same
// iterator continues from the last returned valid position. The x and y
// parameter will contain the coordinates for the returned value if it is a
// valid one. Returns MAPPING_POSITION_EMPTY if no valid position is found (x
// and y are invalid in this case).
byte GetNextValidPosition(Iterator* iterator, int8_t* x, int8_t* y);

// Same as above, but returns valid positions at a distance equal to or smaller
// than from the given x0 and y0 coordinates. Note that although we are dealing
// only with x and y coordinates, we are actually mapping an hexagonal grid so
// there will be 6 neighbors for each position, not 4.
byte GetNextValidPositionAround(int8_t x0, int8_t y0, byte distance,
                                Iterator* iterator, int8_t* x, int8_t* y);

// Returns true if the mapping has started (Set() was called at least once
// since the last Reset() call or since the Blink started).
bool Initialized();

// Resets the map, making it empty.
void Reset();

}  // namespace mapping

#endif  // MAPPING_H_