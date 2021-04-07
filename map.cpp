#include "map.h"

#include <string.h>

#if __has_include(<mapping_config.h>)
#include <mapping_config.h>
#else
#define MAPPING_MAP_WIDTH 10
#define MAPPING_MAP_HEIGHT 10
#endif

namespace mapping {

struct Map {
  byte positions[MAPPING_MAP_WIDTH][MAPPING_MAP_HEIGHT];

  int8_t min_x;
  int8_t min_y;

  bool initialized;
};
static Map map_;

namespace map {

// Clears the map from 0,0 to delta_x,delta_y.
static void clear(byte delta_x, byte delta_y) {
  for (byte x = 0; x < MAPPING_MAP_WIDTH; x++) {
    for (byte y = 0; y < MAPPING_MAP_HEIGHT; y++) {
      if (x < delta_x || y < delta_y) {
        map_.positions[x][y] = MAP_POSITION_EMPTY;
      }
    }
  }
}

// Moves map by the given deltas to adjust for a new entry being set. delta_x
// and delta_y are expected to always be nonpositive. This tries to keep all set
// positions as close to 0,0 on the map as possible.
static void move(byte delta_x, byte delta_y) {
  memmove(&map_.positions[delta_x][delta_y], map_.positions,
          MAPPING_MAP_WIDTH * MAPPING_MAP_HEIGHT -
              (delta_y * MAPPING_MAP_WIDTH + delta_x));

  clear(delta_x, delta_y);
}

void Set(int8_t x, int8_t y, byte value) {
  if (map_.initialized) {
    byte delta_x = 0;
    if (x < map_.min_x) {
      delta_x = map_.min_x - x;
      map_.min_x = x;
    }

    byte delta_y = 0;
    if (y < map_.min_y) {
      delta_y = map_.min_y - y;
      map_.min_y = y;
    }

    move(delta_x, delta_y);
  } else {
    map_.min_x = x;
    map_.min_y = y;
  }

  map_.positions[x - map_.min_x][y - map_.min_y] = value;

  map_.initialized = true;
}

byte Get(int8_t x, int8_t y) {
  return map_.positions[x - map_.min_x][y - map_.min_y];
}

void Reset() { memset(&map_, 0, sizeof(Map)); }

}  // namespace map

}  // namespace mapping
