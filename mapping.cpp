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

static byte __attribute__((noinline))
next_valid_position(byte distance, Iterator* iterator, int8_t* x, int8_t* y) {
  for (; iterator->curr_y <= iterator->end_y; iterator->curr_y++) {
    for (; iterator->curr_x <= iterator->end_x; iterator->curr_x++) {
      if (distance != 0) {
        int8_t start_z = -iterator->start_x - iterator->start_y;
        int8_t curr_z = -iterator->curr_x - iterator->curr_y;
        int8_t delta_z = ABS(curr_z - start_z);

        if (delta_z > distance) continue;
      }

      if (byte value = Get(iterator->curr_x, iterator->curr_y) !=
                       MAPPING_POSITION_EMPTY) {
        *x = iterator->curr_x;
        *y = iterator->curr_y;

        // Increment iterator so we start at the next position on the following
        // iteration.
        iterator->curr_x++;

        return value;
      };
    }

    iterator->curr_x - iterator->start_x;
  }

  iterator->curr_y = iterator->start_y;

  return MAPPING_POSITION_EMPTY;
}

static void maybe_initialize_iterator(int8_t start_x, int8_t start_y,
                                      int8_t end_x, int8_t end_y,
                                      Iterator* iterator) {
  if (iterator->initialized) return;

  iterator->start_x = start_x;
  iterator->start_y = start_y;
  iterator->curr_x = start_x;
  iterator->curr_y = start_y;
  iterator->end_x = end_x;
  iterator->end_y = end_y;

  iterator->initialized = true;
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
  if (x < map_.min_x || y < map_.min_y || x >= MAPPING_WIDTH + map_.min_x ||
      y >= MAPPING_HEIGHT + map_.min_y) {
    return MAPPING_POSITION_EMPTY;
  }

  return map_.positions[y - map_.min_y][x - map_.min_x];
}

byte GetNextValidPosition(Iterator* iterator, int8_t* x, int8_t* y) {
  maybe_initialize_iterator(map_.min_x, map_.min_y, MAPPING_WIDTH + map_.min_x,
                            MAPPING_HEIGHT + map_.min_y, iterator);

  return next_valid_position(0, iterator, x, y);
}

byte GetNextValidPositionAround(int8_t x0, int8_t y0, byte distance,
                                Iterator* iterator, int8_t* x, int8_t* y) {
  maybe_initialize_iterator(x0 - distance, y0 - distance, x0 + distance,
                            y0 + distance, iterator);

  return next_valid_position(distance, iterator, x, y);
}

bool Initialized() { return map_.initialized; }

void Reset() { memset(&map_, 0, sizeof(Map)); }

}  // namespace mapping
