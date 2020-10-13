#include "Dot.h"

Dot::Dot(Vector2 pos, double brightness) : pos{pos}, brightness{brightness} {
}

Vector2 Dot::getPos() const {
	return pos;
}

double Dot::getBrightness() const {
	return brightness;
}

void Dot::setPos() {
	this->pos = pos;
}

void Dot::setBrightness() {
	this->brightness = brightness;
}
