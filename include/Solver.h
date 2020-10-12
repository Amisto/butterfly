#ifndef SOLVER_SRC_SYSTEM_H_
#define SOLVER_SRC_SYSTEM_H_

#include "Obstacle.h"
#include "Node.h"
#include "Sensor.h"
#include "Dot.h"

class Solver {
 private:
  Obstacle obstacles[OBSTACLES_TOTAL];
  Dot dots[DOTS_TOTAL];
  Sensor sensors[SENSORS];
  Node *nodes[300000];

};

#endif //SOLVER_SRC_SYSTEM_H_
