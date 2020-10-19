#include "../include/Obstacle.h"

Obstacle::Obstacle() : c_rel{0} {
	this->pos[VERTICES] = {Vector2()};
}

Vector2 Obstacle::getPos(int i) const {
	return pos[i];
}

double Obstacle::getCRel() const {
	return c_rel;
}

void Obstacle::setPos(int j, Vector2 pos) {
	this->pos[j] = pos;
}
void Obstacle::setCRel(double c_rel) {
	this->c_rel = c_rel;
}
