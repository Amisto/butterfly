#ifndef SOLVER_OBSTACLE_H
#define SOLVER_OBSTACLE_H

#include "Constants.h"
#include "Vector2.h"

class Obstacle {
 private:
  Vector2 pos[VERTICES];
  double c_rel;

 public:
  Vector2 getPos(int i) const;
  double getCRel() const;

};

#endif //SOLVER_OBSTACLE_H
