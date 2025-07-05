#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "horse.h"


PRIVATE void initBones(Body *body, Vector2 pos, const int *radii) {
  for (int i = 0; i < body->numBones; ++i) {
    Bone *bone = body->bones + i;
    bone->pos.y = pos.y;
    bone->pos.x = pos.x - (i * body->boneLength);
    bone->radius = radii[i];
  }
  for (int i = 0; i < 4; ++i) body->limbs[i] = Vector2Zero();
}

PRIVATE void updateBones(Body *body) {
  for (int i = 1; i < body->numBones; ++i) {
    Bone *prev = &body->bones[i - 1];
    Bone *curr = &body->bones[i];

    // Enforce distance
    Vector2 delta = Vector2Subtract(curr->pos, prev->pos);
    float currentLength = Vector2Length(delta);
    if (currentLength == 0) continue;

    float difference = (currentLength - body->boneLength) / currentLength;
    Vector2 correction = Vector2Scale(delta, difference);
    curr->pos = Vector2Subtract(curr->pos, correction);

    // Enforce min angle starting from bone 2
    if (i > 1) {
      Bone *beforePrev = &body->bones[i - 2];
      Vector2 v1 = Vector2Normalize(Vector2Subtract(beforePrev->pos, prev->pos));
      Vector2 v2 = Vector2Normalize(Vector2Subtract(curr->pos, prev->pos));

      float dot = Vector2DotProduct(v1, v2);
      // Clamp dot to avoid domain error in acos
      if (dot > 1.0f) dot = 1.0f;
      if (dot < -1.0f) dot = -1.0f;

      float angle = acosf(dot);

      if (angle < body->minAngle) {
        float angleToRotate = body->minAngle - angle;

        // Use cross product to determine direction (sign)
        float cross = v1.x * v2.y - v1.y * v2.x;
        float direction = (cross < 0) ? -1.0f : 1.0f;

        // Rotate v2 by (angleToRotate * direction)
        float s = sinf(angleToRotate * direction);
        float c = cosf(angleToRotate * direction);

        Vector2 rotated = {
            v2.x * c - v2.y * s,
            v2.x * s + v2.y * c
        };

        // Scale back to bone length
        rotated = Vector2Scale(rotated, body->boneLength);
        curr->pos = Vector2Add(prev->pos, rotated);
      }
    }
  }
}

PRIVATE void drawHorse(Body *body, int time, float speed, float omega, float freq) {
  Color bc = {219, 219, 219, 255};
  Color bc2 = {210, 210, 210, 255};
  Color nc = {190, 190, 190, 255};
  Color hc = {150, 150, 150, 255};
  Color hc2 = {170, 170, 170, 255};
  Color fc = {100, 100, 100, 255};
  float amp = speed * 0.5;

  // Legs
  for (int i = 0; i < 4; ++i) {
    Vector2 check, base, target, foot = body->limbs[i];
    float rot;
    int j;
    if (i > 1) j = 4;
    else j = 2;
    Bone prev = body->bones[j-1];
    Bone curr = body->bones[j];
    Vector2 mid = Vector2Scale(Vector2Add(prev.pos, curr.pos), 0.5);
    Vector2 delta = Vector2Subtract(prev.pos, curr.pos);
    rot = atan2f(delta.y, delta.x);
    float r = 10;
    float rc = i > 1 ? 0 : -8;
    float rt = i > 1 ? 8 : 8;
    float at = i > 1 ? 1.0 : 0.9;
    if (i == 2 || i == 0) {
      base = Vector2Add(mid, (Vector2){cosf(rot + 1.6)*r, sinf(rot + 1.6)*r});
      check = Vector2Add(base, (Vector2){cosf(rot + 3.14)*rc, sinf(rot + 3.14)*rc});
      target = Vector2Add(
        Vector2Scale(Vector2Add(body->bones[j-2].pos, body->bones[j-1].pos), 0.5),
        (Vector2){cosf(rot + at)*rt, sinf(rot + at)*rt}
      );
    }
    else {
      base = Vector2Add(mid, (Vector2){cosf(rot - 1.6)*r, sinf(rot - 1.6)*r});
      check = Vector2Add(base, (Vector2){cosf(rot + 3.14)*rc, sinf(rot + 3.14)*rc});
      target = Vector2Add(
        Vector2Scale(Vector2Add(body->bones[j-2].pos, body->bones[j-1].pos), 0.5),
        (Vector2){cosf(rot - at)*rt, sinf(rot - at)*rt}
      );
    }

    float db = Vector2DistanceSqr(foot, check);
    float maxd = 30;
    float mind = 10;

    //if (i == 2) {
      //DrawCircleLinesV(check, (maxd), RED);
      //DrawCircleLinesV(target, (mind), RED);
    //}
   
    float legSpeed = 3;
    if (body->moveLimb[i]) {
      Vector2 d = Vector2Subtract(target, foot);
      if (Vector2LengthSqr(d) < mind * mind) body->moveLimb[i] = false;
      Vector2 vel = Vector2Scale(Vector2Normalize(d), legSpeed + speed);
      body->limbs[i] = Vector2Add(foot, vel);
    }
    bool adjacent = i%2 == 0 ? body->moveLimb[i+1] : body->moveLimb[i-1];
    if (db >= maxd * maxd && !adjacent) body->moveLimb[i] = true;
    float leg_off = 20 - 2 * speed;
    if (db >= maxd * maxd + leg_off * leg_off) body->moveLimb[i] = true;

    float headRot = atan2f(body->bones[1].pos.y - body->bones[2].pos.y, body->bones[1].pos.x - body->bones[2].pos.x);
    if (i <= 1) headRot = atan2f(body->bones[2].pos.y - body->bones[1].pos.y, body->bones[2].pos.x - body->bones[1].pos.x);
    float thighLen = Vector2Distance(foot, base);
    float hoofR = 0.25 * (60 - thighLen);
    if (i <= 1) hoofR = 0.35 * thighLen;
    if (hoofR < 0) hoofR = 0;
    Vector2 hoof = Vector2Add(
      foot,
      (Vector2){cos(headRot)*hoofR, sin(headRot)*hoofR}
    );
    DrawCircleV(hoof, 7, hc);
    DrawLineEx(foot, hoof, 12, hc);
    DrawCircleV(foot, 6, hc2);
    DrawLineEx(base, foot, 12, hc2);
    DrawCircleV(base, 6, hc2);
  }

  // Lead
  Vector2 wl_target = omega < 0 
    ? (Vector2){10, -15} 
    : omega > 0
    ? (Vector2){30, -10}
    : (Vector2){25, -13};
  body->limbs[4] = Vector2Lerp(body->limbs[4], wl_target, 0.2);
  Vector2 wr_target = omega > 0 
    ? (Vector2){10, 15} 
    : omega < 0
    ? (Vector2){30, 10}
    : (Vector2){25, 13};
  body->limbs[5] = Vector2Lerp(body->limbs[5], wr_target, 0.2);

  Vector2 head = body->bones[0].pos;
  Vector2 neck = body->bones[1].pos;
  Vector2 delta2 = Vector2Subtract(head, neck);
  float rh = atan2f(delta2.y, delta2.x);
  Vector2 sho = body->bones[2].pos;
  Vector2 hip = body->bones[3].pos;
  Vector2 med = Vector2Scale(Vector2Add(sho, hip), 0.5);
  Vector2 delta = Vector2Subtract(sho, hip);
  float rot = atan2f(delta.y, delta.x);
  Vector2 wl = body->limbs[4];
  Vector2 wr = body->limbs[5]; // {25, -13};
  float rlha = rot + atan2f(wl.y, wl.x);
  float rrha = rot + atan2f(wr.y, wr.x);
  float llha = Vector2Length(wl);
  float lrha = Vector2Length(wr);
  Vector2 leftLead = {
    head.x + cosf(rh - 1.5) * 10,
    head.y + sinf(rh - 1.5) * 10
  };
  Vector2 leftHand = {
    med.x + cosf(rlha) * llha,
    med.y + sinf(rlha) * llha,
  };
  Vector2 rightLead = {
    head.x + cosf(rh + 1.5) * 10,
    head.y + sinf(rh + 1.5) * 10
  };
  Vector2 rightHand = {
    med.x + cosf(rrha) * lrha,
    med.y + sinf(rrha) * lrha,
  };
  DrawLineEx(leftLead, leftHand, 2, BLACK);
  DrawLineEx(rightLead, rightHand, 2, BLACK);

  // Body
  for (int i = body->numBones-1; i >= 1; --i) {
    Bone prev = body->bones[i-1];
    Bone curr = body->bones[i];
    Vector2 mid = Vector2Scale(Vector2Add(prev.pos, curr.pos), 0.5);
    Vector2 delta = Vector2Subtract(prev.pos, curr.pos);
    float rot = atan2f(delta.y, delta.x);

    rlPushMatrix();
    rlTranslatef(mid.x, mid.y, 0);
    rlRotatef(rot * RAD2DEG, 0, 0, 1);
    if (i == 1) {
      rlPushMatrix();
      rlTranslatef(sinf(time * freq) * amp * 1.25, 0, 0);
      Rectangle nose = {8, -8, 22, 16};
      DrawRectangleRounded(nose, 5, 6, nc);
      Rectangle neck = {-20, -9, 38, 18};
      DrawRectangleRounded(neck, 5, 6, bc);

      rlPushMatrix();
      rlTranslatef(30, 0, 0);
      rlRotatef(-30, 0, 0, 1);
      DrawEllipse(-22, -3, 5, 3, bc);
      rlRotatef(60, 0, 0, 1);
      DrawEllipse(-22, 3, 5, 3, bc);
      rlPopMatrix();

      float hairoff = sinf(time * freq + 3) * speed + speed;
      Rectangle hair = {-28 - hairoff / 2, -4, 44 + hairoff, 8};
      DrawRectangleRounded(hair, 5, 6, nc);
      rlPopMatrix();
    }
    else if (i == 2) {
      rlPushMatrix();
      rlTranslatef(sinf(time * freq + 1) * amp, 0, 0);
      // Shoulders
      DrawEllipse(0, 0, prev.radius, prev.radius-2, bc);
      rlPopMatrix();
    }
    else if (i == 3) {
      rlPushMatrix();
      rlTranslatef(sinf(time * freq + 2) * amp, 0, 0);
      // Abdomen
      DrawEllipse(5, 0, prev.radius + 8, prev.radius, bc2);
      rlPopMatrix();
    }
    rlPopMatrix();
  }

  for (int i = body->numBones-1; i >= 1; --i) {
    Bone prev = body->bones[i-1];
    Bone curr = body->bones[i];
    Vector2 mid = Vector2Scale(Vector2Add(prev.pos, curr.pos), 0.5);
    Vector2 delta = Vector2Subtract(prev.pos, curr.pos);
    float rot = atan2f(delta.y, delta.x);
    rlPushMatrix();
    rlTranslatef(mid.x, mid.y, 0);
    if (i == 4) {
      rlRotatef(rot * RAD2DEG, 0, 0, 1);
      rlPushMatrix();
      rlTranslatef(sinf(time * freq + 3) * amp, 0, 0);
      // Rear
      float hairoff = sinf(time * freq + 4) * speed + speed;
      DrawEllipse(-prev.radius-6 - hairoff, 0, 7 + hairoff, 6, fc);
      DrawEllipse(0, 0, prev.radius + 4, prev.radius, bc);
      rlPopMatrix();
    }
    else if (i == 3) {
      Color hc = {40, 40, 50, 255};
      Color hc2 = {80, 80, 100, 255};
      Color sc = {107, 86, 59, 255};
      Vector2 s = {0, -8};
      Vector2 wl = body->limbs[4];
      Vector2 wr = body->limbs[5];
      Vector2 el = {(wl.x - 30) / 2 - 2.5, -15};
      Vector2 er = {(wr.x - 30) / 2 - 2.5, -15};
      
      rlRotatef(rot * RAD2DEG, 0, 0, 1);
      rlPushMatrix();
      rlTranslatef(sinf(time * freq + 2) * amp, 0, 0);
      rlPushMatrix();
      rlTranslatef(sinf(time * freq - 1) * amp * 0.75, 0, 0);
      DrawLineEx(wl, wr, 3, BLACK);
      // Left arm
      DrawLineEx(el, wl, 9, sc);
      DrawCircleV(wl, 5, BEIGE);
      DrawLineEx(s, el, 8, BROWN);
      DrawCircleV(el, 4, BROWN);
      // Right arm
      DrawLineEx((Vector2){er.x, -er.y}, wr, 8, sc);
      DrawCircleV(wr, 5, BEIGE);
      DrawLineEx((Vector2){s.x, -s.y}, (Vector2){er.x, -er.y}, 8, BROWN);
      DrawCircle(er.x, -er.y, 4, BROWN);
      rlPopMatrix();
      // Hat
      rlPushMatrix();
      float clmp = speed < 5 ? 5 : speed;
      rlTranslatef(sinf(time * freq - 2) * amp * 1, cosf(time * freq - 2) * 2 * sqrt(speed) / (0.01 + clmp), 0);
      DrawEllipse(0, 0, 16, 14, hc);
      DrawEllipse(0, 0, 9, 7, hc2);
      rlPopMatrix();
      
      rlPopMatrix();
    }
    rlPopMatrix();
  } 
}

PRIVATE void horseUpdate(void *ent, int fc) {
  Horse *this = (Horse*)ent;
  Body *body = this->body;
  float omega = 0;

  if (IsKeyDown(KEY_ENTER) && this->speed < this->maxSpeed) this->speed += this->acc;
  if (IsKeyDown(KEY_RIGHT_SHIFT) && this->speed > 0) this->speed *= 0.98;
  if (IsKeyDown(KEY_A)) omega = -this->turnSpeed;
  if (IsKeyDown(KEY_S)) omega = this->turnSpeed;
  this->rot += omega;

  float freq = this->speed < 10 ? 0.25 : 0.3;
  float s = this->speed + sinf(fc * freq) * (this->speed * 0.15);
  this->vel.x = cos(this->rot) * s;
  this->vel.y = sin(this->rot) * s;
  body->bones[0].pos = Vector2Add(body->bones[0].pos, this->vel);
  updateBones(body);
  float mag = Vector2Length(this->vel);
  drawHorse(body, fc, mag, omega, freq);
}

PRIVATE void horseOnDestroy(Entity *ent) {
  Horse *this = (Horse *)ent->data;
  free(this->body->bones);
  free(this->body->limbs);
  free(this->body->moveLimb);
  free(this->body);
  free(ent->data);
  free(ent);
}

Entity *HorseInit(Vector2 pos) {
  Bone *bones = malloc(sizeof(Bone) * HORSE_BONES);
  Vector2 *limbs = malloc(sizeof(Vector2) * (2 + HORSE_LEGS));
  bool *move = malloc(sizeof(bool) * HORSE_LEGS);
  Body *body = malloc(sizeof(Body));
  Horse *horse = malloc(sizeof(Horse));
  Entity *ent = malloc(sizeof(Entity));

  *body = (Body){
    .numBones = HORSE_BONES, 
    .boneLength = 24, 
    .minAngle = 165 * DEG2RAD,
    .bones = bones,
    .limbs = limbs,
    .moveLimb = move
  };
  initBones(body, pos, (int[]){15, 15, 16, 17});
  *horse = (Horse){
    .body = body,
    .acc = 0.1,
    .turnSpeed = 0.08,
    .maxSpeed = 15.0,
    .brakeForce = 0.98,
    .vel = Vector2Zero(),
    .speed = 0,
    .rot = 0
  };

  *ent = (Entity){
    .setup = NULL,
    .update = horseUpdate,
    .destroy = horseOnDestroy,
    .data = horse,
    .type = T_HORSE
  };

  return ent;
}
