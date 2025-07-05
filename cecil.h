#ifndef _GOBJ
#define _GOBJ

#define PRIVATE static

typedef enum EntityType {
  T_HORSE = 0
} EntityType;

typedef struct Entity {
  void          (*setup)(void *obj);
  void          (*update)(void *obj, int frameCount);
  void          (*destroy)(struct Entity *obj);
  void          *data;
  EntityType    type;
  struct Entity *next;
  struct Entity *prev;
} Entity;

void Push(Entity*);
void Remove(Entity*);
void RunGame();

#endif
