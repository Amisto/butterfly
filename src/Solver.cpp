#include "Solver.h"
#include <fstream>
#include <iostream>

Solver::Solver() {
	init();
}

void Solver::init() {
	std::ifstream file("baseline.txt");
	file >> OBSTACLES;

	for (int i = 0; i < OBSTACLES; i++) {
		for (int j = 0; j < VERTICES; j++) {
			int x, y;
			file >> x >> y;
			obstacles[i].setPos(j, Vector2(x, y));
		}
		double c_rel;
		file >> c_rel;
		obstacles[i].setCRel(c_rel);

		obstacles[i].setPos(VERTICES, obstacles[i].getPos(0));
	}

	file >> DOTS;
	for (int i = 0; i < DOTS; i++) {
		int x, y;
		double brightness;
		file >> x >> y >> brightness;

		dots[i].setPos(Vector2(x, y));
		dots[i].setBrightness(brightness);
	}

	file.close();
}
