#include "mapping.h"

#include <string.h>

#if __has_include(<mapping_config.h>)
#include <mapping_config.h>
#endif

namespace mapping {

struct Map {
  byte positions[MAPPING_HEIGHT][MAPPING_WIDTH];

  int8_t min_x;
  int8_t min_y;

  bool initialized;
};
static Map map_;

// Clears the map from 0,0 to delta_x,delta_y.
static void clear(byte delta_x, byte delta_y) {
  for (byte y = 0; y < MAPPING_HEIGHT; y++) {
    for (byte x = 0; x < MAPPING_WIDTH; x++) {
      if (x < delta_x || y < delta_y) {
        map_.positions[y][x] = MAPPING_POSITION_EMPTY;
      }
    }
  }
}

// Moves map by the given deltas to adjust for a new entry being set. delta_x
// and delta_y are expected to always be nonnegative. This tries to keep all set
// positions as close to 0,0 on the map as possible.
static void move(byte delta_x, byte delta_y) {
  memmove(
      &map_.positions[delta_y][delta_x], map_.positions,
      MAPPING_WIDTH * MAPPING_HEIGHT - (delta_y * MAPPING_HEIGHT + delta_x));

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
  if (x < map_.min_x || y < map_.min_y || x > MAPPING_WIDTH + map_.min_x ||
      y > MAPPING_HEIGHT + map_.min_y) {
    return MAPPING_POSITION_EMPTY;
  }

  return map_.positions[y - map_.min_y][x - map_.min_x];
}

bool AllPositions(PositionHandler position_handler, void* context) {
  for (byte y = 0; y < MAPPING_HEIGHT; y++) {
    for (byte x = 0; x < MAPPING_WIDTH; x++) {
      if (!position_handler(x + map_.min_x, y + map_.min_y,
                            &map_.positions[y][x], context)) {
        return false;
      }
    }
  }

  return true;
}

bool AllPositionsAround(int8_t x, int8_t y, byte distance,
                        PositionHandler position_handler, void* context) {
  int8_t z = -x - y;
  for (int8_t y1 = y - distance; y1 <= y + distance; y1++) {
    for (int8_t x1 = x - distance; x1 <= x + distance; x1++) {
      int8_t z1 = -x1 - y1;
      if (x1 != x && y1 != y && Get(x1, y1) != MAPPING_POSITION_EMPTY &&
          abs(z - z1) <= distance) {
        if (!position_handler(x1, y1,
                              &map_.positions[y1 - map_.min_y][x1 - map_.min_x],
                              context)) {
          return false;
        }
      }
    }
  }

  return true;
}

bool Initialized() { return map_.initialized; }

void Reset() { memset(&map_, 0, sizeof(Map)); }

}  // namespace mapping
