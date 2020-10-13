#ifndef SOLVER_INCLUDE_DOT_H_
#define SOLVER_INCLUDE_DOT_H_

#include "Vector2.h"

class Dot {
 private:
	Vector2 pos;
	double brightness;

 public:
	Dot(Vector2 pos = Vector2(), double brightness = 0);
	Vector2 getPos() const;
	double getBrightness() const;

	void setPos();
	void setBrightness();
};

#endif //SOLVER_INCLUDE_DOT_H_
