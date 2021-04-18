#include "mapping.h"

#include <string.h>

#if __has_include(<mapping_config.h>)
#include <mapping_config.h>
#endif

#define ABS(x) ((x) > 0 ? (x) : -(x))

namespace mapping {

struct Map {
  byte positions[MAPPING_HEIGHT][MAPPING_WIDTH];

  int8_t min_x;
  int8_t min_y;

  int8_t max_x;
  int8_t max_y;

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

    if (map_.max_x < x) map_.max_x = x;

    byte delta_y = 0;
    if (y < map_.min_y) {
      delta_y = map_.min_y - y;
      map_.min_y = y;
    }

    if (map_.max_y < y) map_.max_y = y;

    move(delta_x, delta_y);
  } else {
    map_.min_x = x;
    map_.min_y = y;
    map_.max_x = x;
    map_.max_y = y;
  }

  map_.positions[y - map_.min_y][x - map_.min_x] = value;

  map_.initialized = true;
}

byte Get(int8_t x, int8_t y) {
  if (x < map_.min_x || y < map_.min_y || x >= MAPPING_WIDTH + map_.min_x ||
      y >= MAPPING_HEIGHT + map_.min_y) {
    return MAPPING_POSITION_EMPTY;
  }

  return &map_.positions[y - map_.min_y][x - map_.min_x];
}

static bool bounding_box_valid_positions(int8_t z0, byte distance,
                                         int8_t start_x, int8_t start_y,
                                         int8_t end_x, int8_t end_y,
                                         PositionHandler position_handler,
                                         void* context) {
  for (int8_t y = start_y; y <= end_y; y++) {
    for (int8_t x = start_x; x <= end_x; x++) {
      if (Get(x, y) == MAPPING_POSITION_EMPTY) continue;

      if (distance && ABS(-x - y - z0) > distance) continue;

      // At this point we know that the coordinates are valid and fall inside
      // the map, so we can index directly into the map positions and return a
      // pointer to it.
      if (!position_handler(
              x, y, &map_.positions[y - map_.min_y][x - map_.min_x], context))
        return false;
    }
  }

  return true;
}

bool AllValidPositions(PositionHandler position_handler, void* context) {
  return bounding_box_valid_positions(0, 0, map_.min_x, map_.min_y, map_.max_x,
                                      map_.max_y, position_handler, context);
}

bool AllValidPositionsAround(int8_t x0, int8_t y0, byte distance,
                             PositionHandler position_handler, void* context) {
  return bounding_box_valid_positions(-x0 - y0, distance, x0 - distance,
                                      y0 - distance, x0 + distance,
                                      y0 + distance, position_handler, context);
}

bool Initialized() { return map_.initialized; }

void Reset() { memset(&map_, 0, sizeof(Map)); }

}  // namespace mapping
