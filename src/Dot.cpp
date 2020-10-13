#include "Dot.h"

Dot::Dot(Vector2 pos, double brightness) : pos{pos}, brightness{brightness} {
}

Vector2 Dot::getPos() const {
	return pos;
}

double Dot::getBrightness() const {
	return brightness;
}

void Dot::setPos(const Vector2 &pos) {
	this->pos = pos;
}

void Dot::setBrightness(double brightness) {
	this->brightness = brightness;
}
