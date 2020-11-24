#include "Sensor.h"
#define SENSORS                 32
#define DT_WIDTH                208.0e-1//37.5//0.0000021               
#define DT_CARRYING             52.0e-1//7.5//0.0000003

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

void Sensor::clearWriting() {
	this->writing.clear();
}

void Sensor::deteriorate() {
	for (int i = 0; i < writing.size(); i++) {
		writing[i].deteriorate();
	}
}

double Sensor::signal(double t, double fc) {
	return sin(M_PI * t / DT_WIDTH) * sin(M_PI * t / DT_WIDTH) * sin(2 * M_PI * t / DT_CARRYING * fc);
}

void Sensor::writeToCSV(std::ostream &fout) {
	double signal = 0;
	for (auto &w : writing) {
		if (w.getTime() > 0) {
			signal += w.getBrightness() * this->signal(w.getTime(), w.getFrequencyCorrection());
		}
		w.setTime(w.getTime() + DT_DIGITIZATION);
	}
	fout << signal;

	bool nulls_exist = true;
	while (nulls_exist) {
		nulls_exist = false;
		for (int j = 0; j < this->writing.size(); j++)
			if (this->writing[j].getTime() > DT_WIDTH) {
				this->writing.erase(this->writing.begin() + j);
				nulls_exist = true;
				break;
			}
	}
}

