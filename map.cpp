#include "map.h"

#include <string.h>

#if __has_include(<map_config.h>)
#include <map_config.h>
#endif

struct Map {
  byte positions[MAP_HEIGHT][MAP_WIDTH];

  int8_t min_x;
  int8_t min_y;

  bool initialized;
};
static Map map_;

namespace map {

// Clears the map from 0,0 to delta_x,delta_y.
static void clear(byte delta_x, byte delta_y) {
  for (byte y = 0; y < MAP_HEIGHT; y++) {
    for (byte x = 0; x < MAP_WIDTH; x++) {
      if (x < delta_x || y < delta_y) {
        map_.positions[y][x] = MAP_POSITION_EMPTY;
      }
    }
  }
}

// Moves map by the given deltas to adjust for a new entry being set. delta_x
// and delta_y are expected to always be nonnegative. This tries to keep all set
// positions as close to 0,0 on the map as possible.
static void move(byte delta_x, byte delta_y) {
  memmove(&map_.positions[delta_y][delta_x], map_.positions,
          MAP_WIDTH * MAP_HEIGHT - (delta_y * MAP_HEIGHT + delta_x));

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

  map_.positions[y - map_.min_y][x - map_.min_x] = value;

  map_.initialized = true;
}

byte Get(int8_t x, int8_t y) {
  return map_.positions[y - map_.min_y][x - map_.min_x];
}

bool AllPositions(PositionHandler position_handler) {
  for (byte y = 0; y < MAP_HEIGHT; y++) {
    for (byte x = 0; x < MAP_WIDTH; x++) {
      if (!position_handler(x + map_.min_x, y + map_.min_y,
                            &map_.positions[y][x])) {
        return false;
      }
    }
  }

  return true;
}

bool Initialized() { return map_.initialized; }

void Reset() { memset(&map_, 0, sizeof(Map)); }

}  // namespace map
