#include "Sensor.h"
#include <stdio.h>
#include <stdlib.h>
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

double signal(double t, double fc) {
    return sin(M_PI * t / DT_WIDTH) * sin(M_PI * t / DT_WIDTH) * sin(2 * M_PI * t / DT_CARRYING * fc);
}//necessary to add this func(

//void Sensor :: writeToCSV(FILE *f_csv = NULL){
//  double _signal = 0;
//  for (int j = 0; j < this->writing.size(); j++) {
//    if (this->writing[j].getTime() > 0)
//    {_signal += this->writing[j].getBrightness() * signal(this->writing[j].getTime(),
//        this->writing[j].getFrequencyCorrection());}
//            this->writing[j].setTime(this->writing[j].getTime()+=DT_DIGITIZATION);
//        }
//        fprintf(f_csv, "%5.2lf ", _signal);
//
//        bool nulls_exist = true;
//        while (nulls_exist) {
//            nulls_exist = false;
//            for (int j = 0; j < this->writing.size(); j++)
//                if (this->writing[j].getTime() > DT_WIDTH) {
//                    this->writing.erase(this->writing.begin() + j);
//                    nulls_exist = true;
//                    break;
//                }
//        }
//}

