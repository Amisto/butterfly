#include "Vector2.h"

Vector2::Vector2(double x, double y) : x{x}, y{y} {
}

void Vector2::setX(double x) {
	this->x = x;
}

void Vector2::setY(double y) {
	this->y = y;
}

double Vector2::getX() const {
	return x;
}

double Vector2::getY() const {
	return y;
}
