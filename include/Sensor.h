#ifndef SOLVER_SRC_SENSOR_H_
#define SOLVER_SRC_SENSOR_H_

#include "Vector2.h"
#include "Writing.h"
#include <vector>

class Sensor {
 private:
  Vector2 pos;
  std::vector<Writing> writing;

 public:
  const Vector2 GetPos() const;
  const std::vector<Writing> GetWriting() const;
  void SetPos(const Vector2 &pos);
  void SetWriting(const std::vector<Writing> &writing);

};

#endif //SOLVER_SRC_SENSOR_H_
