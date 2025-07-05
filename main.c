#include "raylib.h"
#include "cecil.h"
#include "horse.h"

int main(void) {
  Entity *horse = HorseInit((Vector2){200,200});
  Push(horse);
  RunGame();
  Remove(horse);
  return 0;
}

