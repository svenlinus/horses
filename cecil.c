#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "cecil.h"

// Global doubly linked list of Entity
PRIVATE Entity *Entities = NULL;
PRIVATE int numEnts = 0;
PRIVATE int frameCount = 0;

void Push(Entity *ent) {
  if (Entities == NULL) {
    ent->next = ent;
    ent->prev = ent;
  }
  else {
    ent->next = Entities;
    ent->prev = Entities->prev;
    Entities->prev->next = ent;
    Entities->prev = ent;
  }
  Entities = ent;
  numEnts++;
  if (ent->setup) ent->setup(ent);
}

void Remove(Entity *ent) {
  ent->prev->next = ent->next;
  ent->next->prev = ent->prev;
  numEnts--;
  if (ent->destroy) ent->destroy(ent);
}

PRIVATE void UpdateEntities() {
  Entity *curr = Entities;
  for (int i = 0; i < numEnts; ++i) {
    if (curr->update) curr->update(curr->data, frameCount);
    curr = curr->next;
  }
}

void RunGame() {
  InitWindow(1600, 1000, "Horse Racing Game");
  SetTargetFPS(60); 
  HideCursor();

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(DARKBROWN);
    UpdateEntities();
    EndDrawing();
    frameCount++;
  }
  CloseWindow();
}

