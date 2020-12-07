#include "Vector2.h"
#include <cmath>
#include "Constants.h"

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

double scalar(const Vector2 &A, const Vector2 &B) {
	return A.getX() * B.getX() + A.getY() * B.getY();
}

double length(const Vector2 &A, const Vector2 &B) {
	Vector2 Tmp = {A.getX() - B.getX(), A.getY() - B.getY()};
	return sqrt(scalar(Tmp, Tmp));
}

double area(const Vector2 &A, const Vector2 &B, const Vector2 &C) {
	return (B.getX() - A.getX()) * (C.getY() - A.getY()) - (B.getY() - A.getY()) * (C.getX() - A.getX());
}

bool doIntersect(const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &D, Vector2 *Result) {
	if (!(area(A, D, B) * area(A, B, C) > 0 && area(D, B, C) * area(D, C, A) > 0))
		return false;
	double Temp = ((C.getY() - A.getY()) * (B.getX() - A.getX()) -
		(C.getX() - A.getX()) * (B.getY() - A.getY())) /
		((C.getX() - D.getX()) * (B.getY() - A.getY()) -
			(C.getY() - D.getY()) * (B.getX() - A.getX()));

	Result->setX(C.getX() + (C.getX() - D.getX()) * Temp);
	Result->setY(C.getY() + (C.getY() - D.getY()) * Temp);
	return true;
}

bool doIntersect(const Vector2 &A,
				 const Vector2 &B,
				 const Vector2 &C,
				 const Vector2 &V,
				 double *distanceToIntersection) { // segment AB, ray CD
	Vector2 D = {C.getX() + V.getX() * 1000, C.getY() + V.getY() * 1000};
	if (!(area(A, D, B) * area(A, B, C) > 0 && area(D, B, C) * area(D, C, A) > 0))
		return false;
	*distanceToIntersection = ((C.getY() - A.getY()) * (B.getX() - A.getX()) -
		(C.getX() - A.getX()) * (B.getY() - A.getY())) /
		((C.getX() - D.getX()) * (B.getY() - A.getY()) -
			(C.getY() - D.getY()) * (B.getX() - A.getX())) * length(C, D);
	return true;
}

bool isPointInRect(const Vector2 &P, const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &D) {
	bool Area1 = area(P, A, B) > -ZERO,
		Area2 = area(P, B, C) > -ZERO,
		Area3 = area(P, C, D) > -ZERO,
		Area4 = area(P, D, A) > -ZERO;
	return (Area1 == Area2 && Area2 == Area3 && Area3 == Area4);
}

//  nodes[node]->getPos(), nodes[node]->getRight()->getPos(), dots[i].getPos()
//double distanceToSegment(const Vector2 &A, const Vector2 &B, const Vector2 &C) {
//	Vector2 O = {(A.getX() + B.getX()) / 2, (A.getY() + B.getY()) / 2};
//	double dist = length(O, C);
//	return dist;
//}

double distanceToSegment(const Vector2 &a, const Vector2 &b, const Vector2 &va, const Vector2 &vb, const Vector2 &c){
//	double t2 = (va.x * (a.y - b.y) + va.y * (b.x - a.x)) / (va.x * vb.y - va.y * vb.y);
	Vector2 o = {(a.getX() + b.getX())/2, (a.getX() + b.getY())/2};//{b.x + vb.x * t2, b.y + vb.y * t2};
	double dist = length(o, c);
	return dist;
}

Vector2 getReflected(const Vector2 &A, const Vector2 &B, const Vector2 &velocity, double cRel, double *intensity) {
	double sinA = -velocity.getY(), cosA = velocity.getX(),
		sinB = (B.getY() - A.getY()) / length(A, B),
		cosB = (B.getX() - A.getX()) / length(A, B);
	if (cosB > 1.0 || cosB < -1.0) {
		*intensity = -1;
		return Vector2();
	}
	double z1 = 1 * cosA, z2 = cRel * cosB;
	*intensity *= fabs((z2 - z1) / (z2 + z1));

	return Vector2(cosA * (cosB * cosB - sinB * sinB) - 2.0 * sinA * sinB * cosB,
				   sinA * (cosB * cosB - sinB * sinB) + 2.0 * cosA * sinB * cosB);
}

Vector2 getRefracted(const Vector2 &A, const Vector2 &B, const Vector2 &Velocity, double cRel, double *intensity) {
	double sinG = (B.getY() - A.getY()) / length(A, B), cosG = (B.getX() - A.getX()) / length(A, B);
	double sinF = Velocity.getY(), cosF = Velocity.getX();
	double cosA = cosG * cosF + sinG * sinF;
	double sinA = sinG * cosF - sinF * cosG;
	double cosB = cRel * cosA;
	if (cosB > 1.0 || cosB < -1.0) {
		return Vector2();
	}
	double sinB = sqrt(1 - cosB * cosB);
	if (sinA < 0) sinB *= -1;

	double z1 = 1 * cosA, z2 = cRel * cosB;
	*intensity *= fabs(2 * z2 / (z2 + z1));        //0.5;//(sinA - cRel * sinB) / (sinA + cRel * sinB);

	return Vector2(cosG * cosB + sinG * sinB, sinG * cosB - cosG * sinB);
}