#ifndef SOLVER_SRC_SENSOR_H_
#define SOLVER_SRC_SENSOR_H_

#include "Vector2.h"
#include "Writing.h"
#include <fstream>
#include <iomanip>

class Sensor {
 private:
	Vector2 pos;
	std::vector<Writing> writing;

 public:
	const Vector2 getPos() const;
	const std::vector<Writing> getWriting() const;
	void setPos(const Vector2 &pos);
	void setWriting(const std::vector<Writing> &writing);
	void clearWriting();
	void deteriorate();
	double signal(double t, double fc);
	void addWriting(Writing &w);
	void writeToCSV(std::ostream &fout);

};

#endif //SOLVER_SRC_SENSOR_H_
