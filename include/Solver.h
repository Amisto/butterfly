#ifndef SOLVER_SRC_SYSTEM_H_
#define SOLVER_SRC_SYSTEM_H_

#include "Obstacle.h"
#include "Node.h"
#include "Sensor.h"

class Solver {
 private:
	std::vector<Obstacle> obstacles;
	std::vector<Dot> dots;
	std::vector<Sensor> sensors;
	Node *nodes[300000] = {NULL};

	int OBSTACLES = 0;
	int DOTS = 0;
	int rays_num = 0;
	int nodesNum = 0;
	double focus = 0;
	double PIES = 0;

	double deteriorationTime = 0;
	double totalTime = 0;
	double startTime = 0;
	double finishTime = 0;

	int checkObstacles(int node);
	int checkDots(int node);
	void handleReflection();
	void deteriorate();
	void resetTime();

 public:
	Solver();
	void initObstacles();
	void initDots();
	void init();
	void propagate();
	void initExplosion(Vector2 pos);
	void step();
	void fixNodes();
	void writeToCSV();

	~Solver();
};

#endif //SOLVER_SRC_SYSTEM_H_
