#ifndef _HORSE
#define _HORSE

#include "cecil.h"
#include "raylib.h"

#define HORSE_BONES 5
#define HORSE_LEGS 4

typedef struct {
  int radius;
  Vector2 pos;
} Bone;

typedef struct {
  int     numBones;         // Number of bones in the body
  int     boneLength;       // The fixed length between each bone
  float   minAngle;         // Minumum angle between two bones
  Bone    *bones;
  Vector2 *limbs;
  bool    *moveLimb;
} Body;


typedef struct {
  Body     *body;
           // Constants
  float    acc;
  float    turnSpeed;
  float    maxSpeed;
  float    brakeForce;
           // Physics
  Vector2  vel;
  float    speed;
  float    rot;
} Horse;

Entity *HorseInit(Vector2 pos);

#endif
