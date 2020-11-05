#include "../include/Obstacle.h"

Vector2 Obstacle::getPos(int i) const {
	return pos.at(i);
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

void Obstacle::addPos(Vector2 pos) {
	this->pos.push_back(pos);
}
