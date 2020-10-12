#include "Sensor.h"

const Vector2 Sensor::GetPos() const {
  return pos;
}

const std::vector<Writing> Sensor::GetWriting() const {
  return writing;
}

void Sensor::SetPos(const Vector2 &pos) {
  this->pos = pos;
}

void Sensor::SetWriting(const std::vector<Writing> &writing) {
  this->writing = writing;
}
