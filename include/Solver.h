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

	int OBSTACLES = 0;
	int DOTS = 0;
	int rays_num = 0;
	int nodesNum = 0;
	double focus = 0;
	double PIES = 0;

	int checkObstacles(int node);

 public:
	Solver();
	void initObstacles();
	void initDots();
	void init();
	void propagate();
	void initExplosion(Vector2 pos);
	void step();
};

#endif //SOLVER_SRC_SYSTEM_H_
