#include "Writing.h"

double Writing::GetTime() const {
  return time;
}

double Writing::GetBrightness() const {
  return brightness;
}

double Writing::GetFrequencyCorrection() const {
  return frequency_correction;
}

Node *Writing::GetNode() const {
  return node;
}

void Writing::SetTime(double time) {
  this->time = time;
}

void Writing::SetBrightness(double brightness) {
  this->brightness = brightness;
}

void Writing::SetFrequencyCorrection(double frequency_correction) {
  this->frequency_correction = frequency_correction;
}

void Writing::SetNode(Node *node) {
  this->node = node;
}
