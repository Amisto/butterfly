#include "Solver.h"
#include <fstream>
#include <iostream>
#include <cmath>

Solver::Solver() {
	init();
}

void Solver::init() {
	initObstacles();
	initDots();

	std::ifstream setupFile("res/setup_base.txt");
	setupFile >> rays_num >> focus >> PIES;
	setupFile.close();
}

void Solver::initObstacles() {
	std::ifstream file("res/obstacles.txt");
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
}

void Solver::initDots() {
	std::ifstream file("res/dots.txt");
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

void Solver::propagate() {
	for (int i = 0; i < SENSORS; i++) {
		double x = X / 2 - DX_SENSORS * (SENSORS / 2 - i);
		double y = Y * 0.999;

		sensors[i].setPos(Vector2(x, y));
		sensors[i].clearWriting();	//probably redundant, writing already empty
		initExplosion(Vector2(x, y));
	}
}

void Solver::initExplosion(Vector2 pos) {
	int n_nodes = 0;
	int n = POINTS_IN_DOT_WAVEFRONT * 2;
	for (int i=0; i<n; i++)
	{
		Node *temp = new Node;
		temp->setMaterial(-1);
		temp->setNeighborsLeft(std::vector<Node*>());
		temp->setNeighborsRight(std::vector<Node*>());
		temp->setTEncounter(INFINITY);
		temp->setLeft(NULL);
		temp->setRight(NULL);
		nodes[n_nodes++] = temp;
		temp->setIntensity(1.0);
		temp->setMarkedForTheKill(0);

		double angle = 0;
		angle = 2*M_PI * i/(double)n;
		temp->setVelocity(Vector2(cos(angle), sin(angle)));
		temp->setPos(pos);
	}
	for (int i=1; i<n-1; i++)
	{
		nodes[i]->setLeft(nodes[i-1]);
		nodes[i]->setRight(nodes[i+1]);
	}
	nodes[0]->setRight(nodes[1]);
	nodes[0]->setLeft(nodes[n-1]);
	nodes[n-1]->setLeft(nodes[n-2]);
	nodes[n-1]->setRight(nodes[0]);

}
