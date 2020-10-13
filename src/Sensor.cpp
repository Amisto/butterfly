#include "Sensor.h"

const Vector2 Sensor::getPos() const {
  return pos;
}

const std::vector<Writing> Sensor::getWriting() const {
  return writing;
}

void Sensor::setPos(const Vector2 &pos) {
  this->pos = pos;
}

void Sensor::setWriting(const std::vector<Writing> &writing) {
  this->writing = writing;
}
