#ifndef SOLVER_SRC_WRITING_H_
#define SOLVER_SRC_WRITING_H_

#include "Node.h"

class Writing {
 private:
  double time;
  double brightness;
  double frequency_correction; //if a ray falls at a target at an angle
  Node *node;

 public:
  double GetTime() const;
  double GetBrightness() const;
  double GetFrequencyCorrection() const;
  Node *GetNode() const;

  void SetTime(double time);
  void SetBrightness(double brightness);
  void SetFrequencyCorrection(double frequency_correction);
  void SetNode(Node *node);
};

#endif //SOLVER_SRC_WRITING_H_
