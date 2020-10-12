#include "../include/Obstacle.h"
Vector2 Obstacle::getPos(int i) const {
  return pos[i];
}
double Obstacle::getCRel() const {
  return c_rel;
}
