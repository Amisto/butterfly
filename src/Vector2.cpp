#include "Vector2.h"
#include <cmath>
#define ZERO                    0.000001

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

double scalar(const Vector2 &A, const Vector2 &B){
    return A.getX()*B.getX() + A.getY()*B.getY();
}

double length(const Vector2 &A, const Vector2 &B){
    Vector2 Tmp = {A.getX() - B.getX(), A.getY() - B.getY()};
    return sqrt(scalar(Tmp, Tmp));
}

double area (const Vector2 &A, const Vector2 &B, const Vector2 &C){
    return (B.getX() - A.getX()) * (C.getY() - A.getY()) - (B.getY() - A.getY()) * (C.getX() - A.getX());
}

bool doIntersect (const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &D, Vector2* Result){
    if (!(area(A, D, B)*area(A, B, C) > 0 && area(D, B, C)*area(D, C, A) > 0))
        return false;
    double Temp = ( (C.getY()-A.getY())*(B.getX()-A.getX()) -
                    (C.getX()-A.getX())*(B.getY()-A.getY()) )/
                    ( (C.getX()-D.getX())*(B.getY()-A.getY()) -
                    (C.getY()-D.getY())*(B.getX()-A.getX()) );

    Result->setX(C.getX() + (C.getX() - D.getX())*Temp);
    Result->setY(C.getY() + (C.getY() - D.getY())*Temp);
    return true;
}

bool doIntersect (const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &V, double *distanceToIntersection) { // segment AB, ray CD
    Vector2 D = {C.getX() + V.getX() * 1000, C.getY() + V.getY() * 1000};
    if (!(area(A, D, B)*area(A, B, C) > 0 && area(D, B, C)*area(D, C, A) > 0))
        return false;
    *distanceToIntersection = ( (C.getY()-A.getY())*(B.getX()-A.getX()) -
                                (C.getX()-A.getX())*(B.getY()-A.getY()) )/
                                ( (C.getX()-D.getX())*(B.getY()-A.getY()) -
                                (C.getY()-D.getY())*(B.getX()-A.getX()) ) * length(C, D);
    return true;
}

bool isPointInRect (const Vector2 &X, const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &D){
    bool Area1 = area(X, A, B) > - ZERO,
            Area2 = area(X, B, C) > - ZERO,
            Area3 = area(X, C, D) > - ZERO,
            Area4 = area(X, D, A) > - ZERO;
    return (Area1 == Area2 && Area2 == Area3 && Area3 == Area4);
}