#ifndef SOLVER_VECTOR2_H
#define SOLVER_VECTOR2_H

class Vector2 {
 private:
  double x, y;

 public:
  Vector2(double x = 0, double y = 0);

  void setX(double x);
  void setY(double y);

  double getX() const;
  double getY() const;
};

#endif //SOLVER_VECTOR2_H
