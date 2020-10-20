#include "Solver.h"
#include <fstream>
#include <iostream>

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
		sensors[i].clearWriting();    //probably redundant, writing already empty
		initExplosion(Vector2(x, y));
	}
}

void Solver::initExplosion(Vector2 pos) {
	nodesNum = 0;
	int n = POINTS_IN_DOT_WAVEFRONT * 2;

	for (int i = 0; i < n; i++) {
		double angle = 2.0 * M_PI * i / (double) n;
		Vector2 velocity(cos(angle), sin(angle));
		Node *temp = new Node(pos, velocity);
		nodes[nodesNum++] = temp;
	}

	for (int i = 1; i < n - 1; i++) {
		nodes[i]->setLeft(nodes[i - 1]);
		nodes[i]->setRight(nodes[i + 1]);
	}

	nodes[0]->setRight(nodes[1]);
	nodes[0]->setLeft(nodes[n - 1]);
	nodes[n - 1]->setLeft(nodes[n - 2]);
	nodes[n - 1]->setRight(nodes[0]);

}

void Solver::step() {
	for (int node = 0; node < nodesNum; node++) {
		int encounters = 0;
		encounters += checkObstacles(node);
		encounters += checkDots(node);

		if (encounters <= 0) {
			nodes[node]->setTEncounter(-1);
		}
	}

	double timeStep = DT_DIGITIZATION * T_MULTIPLIER;
	for (int node = 0; node < nodesNum; node++) {
		if (nodes[node]->getTEncounter() > -1) {
			timeStep = std::min(timeStep, nodes[node]->getTEncounter());
		}
	}

}

int Solver::checkObstacles(int node) {
	int encounters = 0;
	double dist, time = INFINITY;

	for (int i = 0; i < OBSTACLES; i++) {
		for (int j = 0; j < VERTICES - 1; j++) {
			if (doIntersect(obstacles[i].getPos(j),
							obstacles[i].getPos(j + 1),
							nodes[node]->getPos(),
							nodes[node]->getVelocity(),
							&dist)) {

				time = nodes[node]->getTime(dist, obstacles[nodes[node]->getMaterial()].getCRel());

				if (time < nodes[node]->getTEncounter()) {
					nodes[node]->setTEncounter(time);
					nodes[node]->setObstacleNumber(i);
					nodes[node]->setVerticeNumber(j);
					encounters++;
				}
			}
		}
	}
	return encounters;
}

int Solver::checkDots(int node) {
	int encounters = 0;
	double time = INFINITY;

	Vector2 nodePos = nodes[node]->getPosAfterStep(10);
	Vector2 rightNodePos = nodes[node]->getRight()->getPosAfterStep(10);
	Vector2 nodeNextPos = nodes[node]->getPosAfterStep(1000);
	Vector2 rightNodeNextPos = nodes[node]->getRight()->getPosAfterStep(1000);

	for (int i = 0; i < DOTS; i++) {
		if (isPointInRect(dots[i].getPos(), nodePos, rightNodePos, nodeNextPos, rightNodeNextPos)) {
			double dist = distanceToSegment(nodes[node]->getPos(), nodes[node]->getRight()->getPos(), dots[i].getPos());

			time = nodes[node]->getTime(dist, obstacles[nodes[node]->getMaterial()].getCRel());

			if (time < nodes[node]->getTEncounter()) {
				nodes[node]->setTEncounter(time);
				nodes[node]->setObstacleNumber(-1);
				nodes[node]->setVerticeNumber(i);
				encounters++;
			}
		}
	}
	for (int i = 0; i < SENSORS; i++) {
		if (isPointInRect(sensors[i].getPos(), nodePos, rightNodePos, nodeNextPos, rightNodeNextPos)) {
			double dist = distanceToSegment(nodes[node]->getPos(), nodes[node]->getRight()->getPos(),
											sensors[i].getPos());

			time = nodes[node]->getTime(dist, obstacles[nodes[node]->getMaterial()].getCRel());

			std::vector<Writing> writing = sensors[i].getWriting();
			if (time < nodes[node]->getTEncounter()) {
				writing.push_back(Writing(-time, nodes[node]->getIntensity(),
										  1.0 / nodes[node]->getVelocity().getY()));
			}
			sensors[i].setWriting(writing);
		}
	}

	return encounters;
}
