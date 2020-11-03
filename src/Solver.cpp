#include "Solver.h"
#include <fstream>
#include <iostream>

Solver::Solver() {
	init();
	propagate();
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
		Obstacle obstacle;
		for (int j = 0; j < VERTICES; j++) {
			int x, y;
			file >> x >> y;
			Vector2 myVec(x, y);
			obstacle.addPos(myVec);
		}
		double c_rel;
		file >> c_rel;
		obstacle.setCRel(c_rel);

		obstacle.addPos(obstacle.getPos(0));
		obstacles.push_back(obstacle);
	}
}

void Solver::initDots() {
	std::ifstream file("res/dots.txt");
	file >> DOTS;

	for (int i = 0; i < DOTS; i++) {
		Dot dot;
		int x, y;
		double brightness;
		file >> x >> y >> brightness;

		dot.setPos(Vector2(x, y));
		dot.setBrightness(brightness);
		dots.push_back(dot);
	}

	file.close();
}

void Solver::propagate() {
	for (int i = 0; i < SENSORS; i++) {
		double x = X / 2 - DX_SENSORS * (SENSORS / 2 - i);
		double y = Y * 0.999;
		Sensor sensor;
		sensor.setPos(Vector2(x, y));
		sensors.push_back(sensor);
	}
	for (auto &sensor : sensors) {
		initExplosion(sensor.getPos());
		step();
	}
}

void Solver::initExplosion(Vector2 pos) {
	nodesNum = 0;
	int n = POINTS_IN_DOT_WAVEFRONT * 2;

	for (int i = 0; i < n; i++) {
		double angle = 2.0 * M_PI * i / (double) n;
		Vector2 velocity(cos(angle), sin(angle));
		nodes[nodesNum++] = new Node(pos, velocity);
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

	for (int node = 0; node < nodesNum; node++) {
		nodes[node]->update(timeStep, obstacles[nodes[node]->getMaterial()].getCRel());
	}

	handleReflection();
	fixNodes();
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
	for (auto &sensor : sensors) {
		if (isPointInRect(sensor.getPos(), nodePos, rightNodePos, nodeNextPos, rightNodeNextPos)) {
			double dist = distanceToSegment(nodes[node]->getPos(), nodes[node]->getRight()->getPos(),
											sensor.getPos());

			time = nodes[node]->getTime(dist, obstacles[nodes[node]->getMaterial()].getCRel());

			std::vector<Writing> writing = sensor.getWriting();
			if (time < nodes[node]->getTEncounter()) {
				writing.push_back(Writing(-time, nodes[node]->getIntensity(),
										  1.0 / nodes[node]->getVelocity().getY()));
			}
			sensor.setWriting(writing);
		}
	}

	return encounters;
}

void Solver::handleReflection() {
	double delta = 0.5;
	for (int i = 0; i < nodesNum; i++) {
		if (nodes[i]->getTEncounter() < ZERO && nodes[i]->getTEncounter() > -delta) {
			if (nodes[i]->getMaterial() >= 0) {
				Node reflected = nodes[i]->getReflected(obstacles[nodes[i]->getObstacleNumber()]);
				Node refracted = nodes[i]->getRefracted(obstacles[nodes[i]->getObstacleNumber()]);

				if (reflected.getIntensity() == -1) {
					nodes[i]->setMarkedForTheKill(1);
				} else {
					// real neighbors always turn to ghost ones -
					// reflected go to the other direction, refracted are in another material
					if (nodes[i]->getLeft()) {
						Node *left = nodes[i]->getLeft();
						reflected.addLeftVirtualNeighbor(left);
						refracted.addLeftVirtualNeighbor(left);
						left->addRightVirtualNeighbor(&reflected);
						left->addLeftVirtualNeighbor(&refracted);
						nodes[i]->setLeft(left);
					}
					if (nodes[i]->getRight()) {
						Node *right = nodes[i]->getRight();
						reflected.addRightVirtualNeighbor(right);
						refracted.addRightVirtualNeighbor(right);
						right->addRightVirtualNeighbor(&reflected);
						right->addLeftVirtualNeighbor(&refracted);
						nodes[i]->setRight(right);
					}

					nodes[i]->restoreWavefront(reflected, refracted);
				}
			}
		}
	}
}
void Solver::fixNodes() {
	for (int node = 0; node < nodesNum; node++) {
		nodes[node]->checkInvalid();

		if (nodes[node]->getMarkedForTheKill()) {
			for (auto &sensor : sensors) {
				for (int j = 0; j < sensor.getWriting().size(); j++) {
					if (sensor.getWriting()[j].getNode() == nodes[node]) {
						sensor.clearWriting();
					}
				}
			}
			delete nodes[node];
			nodes[node] = NULL;
		}
	}

	bool cleared = false;
	while (!cleared) {
		while (!nodes[nodesNum - 1] && nodesNum) {
			nodesNum--;
		}
		cleared = true;
		for (int i = 0; i < nodesNum; i++) {
			if (!nodes[i]) {
				nodes[i] = nodes[--nodesNum];
				cleared = false;
				break;
			}
		}
	}

	for (int node = 0; node < nodesNum; node++) {
		if (nodes[node]) {
			nodes[node]->clearNeighbours();
		}
	}
}
Solver::~Solver() {
	for (int i = 0; i < nodesNum; i++) {
		if (nodes[i] != NULL) {
			delete nodes[i];
		}
	}
}


