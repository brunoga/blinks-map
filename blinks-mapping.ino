#include "map.h"

void setup() {
  mapping::map::Set(1, 1, 1);
  mapping::map::Set(-1, -1, 2);
  mapping::map::Set(0, 0, 3);
}

void loop() {
  if (mapping::map::Get(1, 1) == 1 && mapping ::map::Get(-1, -1) == 2 &&
      mapping ::map::Get(0, 0) == 3) {
    setColor(GREEN);
  } else {
    setColor(RED);
  }
}
