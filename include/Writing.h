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
	Writing(double time, double brightness, double frequency_correction);

	double getTime() const;
	double getBrightness() const;
	double getFrequencyCorrection() const;
	Node *getNode() const;

	void setTime(double time);
	void setBrightness(double brightness);
	void setFrequencyCorrection(double frequency_correction);
	void setNode(Node *node);
	void deteriorate();
};

#endif //SOLVER_SRC_WRITING_H_
