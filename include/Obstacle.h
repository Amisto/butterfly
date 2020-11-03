#ifndef SOLVER_OBSTACLE_H
#define SOLVER_OBSTACLE_H

#include "Constants.h"
#include "Vector2.h"

class Obstacle {
 private:
	std::vector<Vector2> pos;
	double c_rel;

 public:
	Vector2 getPos(int i) const;
	double getCRel() const;

	void addPos(Vector2 pos);
	void setPos(int j, Vector2 pos);
	void setCRel(double c_rel);
};

#endif //SOLVER_OBSTACLE_H
