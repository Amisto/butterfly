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
		for (int j = 0; j < VERTICES - 1; j++) {
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

int s = 0;
int h = 0;
void Solver::propagate() {
	for (int i = 0; i < SENSORS; i++) {
		double x = X / 2 - DX_SENSORS * (SENSORS / 2 - i);
		double y = Y * 0.999;
		Sensor sensor;
		sensor.setPos(Vector2(x, y));
		sensors.push_back(sensor);
	}
	for (auto &sensor : sensors) {
		std::cout << "SENSOR " << s << std::endl;
		std::ofstream csvFile;
		csvFile.open("data/baseline/Sensor" + std::to_string(s) + "/raw" + std::to_string(s) + ".csv");
		csvFile << "";
		csvFile.close();
		initExplosion(sensor.getPos());
		resetTime();
		while (finishTime > 0) {
//			std::cout << "iter: " << h << std::endl;
			step();
			h++;
//			if (h > 10) {
//				break;
//			}
//			break;
		}
		s++;
		break;
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

	for (auto &sensor : sensors) {
		sensor.clearWriting();
	}

}

void Solver::step() {
	for (int node = 0; node < nodesNum; node++) {
		int encounters = 0;
		if (nodes[node]->getTEncounter() >= INFINITY) {
			encounters += checkObstacles(node);
			if (nodes[node]->getRight()) {
				encounters += checkDots(node);
			}

			if (encounters == 0) {
				nodes[node]->setTEncounter(-1);
			}
		}
	}

	double timeStep = DT_DIGITIZATION * T_MULTIPLIER;
	for (int node = 0; node < nodesNum; node++) {
		if (nodes[node]->getTEncounter() > ZERO && nodes[node]->getTEncounter() < timeStep) {
			timeStep = nodes[node]->getTEncounter();
		}
	}
	if (timeStep > DT_DIGITIZATION * T_MULTIPLIER) {
		timeStep = DT_DIGITIZATION * T_MULTIPLIER;
	}
	for (int node = 0; node < nodesNum; node++) {
		nodes[node]->update(timeStep, obstacles[nodes[node]->getMaterial()].getCRel());
	}
	handleReflection();
	fixNodes();
	deteriorationTime += timeStep;
	while (deteriorationTime > DT_DETERIORATION) {
		deteriorate();
		deteriorationTime -= DT_DETERIORATION;
	}

	totalTime += timeStep;
	while (totalTime > DT_DIGITIZATION) {
		if (startTime < 0) {
			writeToCSV();
		}
		totalTime -= DT_DIGITIZATION;
		startTime -= DT_DIGITIZATION;
		finishTime -= DT_DIGITIZATION;
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

				if ((time < nodes[node]->getTEncounter()) && (time > ZERO)) {
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
		if (isPointInRect(dots[i].getPos(), nodePos, rightNodePos, rightNodeNextPos, nodeNextPos)) {

			double dist = distanceToSegment(nodes[node]->getPos(),
											nodes[node]->getRight()->getPos(),
											nodes[node]->getVelocity(),
											nodes[node]->getRight()->getVelocity(),
											dots[i].getPos());

			time = nodes[node]->getTime(dist, obstacles[nodes[node]->getMaterial()].getCRel());

			if ((time < nodes[node]->getTEncounter()) && (time > ZERO)) {
				nodes[node]->setTEncounter(time);
				nodes[node]->setObstacleNumber(-1);
				nodes[node]->setVerticeNumber(i);
				encounters++;
			}
		}
	}
	for (auto &sensor : sensors) {
		if (isPointInRect(sensor.getPos(), nodePos, rightNodePos, rightNodeNextPos, nodeNextPos)) {
			double dist = distanceToSegment(nodes[node]->getPos(),
											nodes[node]->getRight()->getPos(),
											nodes[node]->getVelocity(),
											nodes[node]->getRight()->getVelocity(),
											sensor.getPos());

			time = nodes[node]->getTime(dist, obstacles[nodes[node]->getMaterial()].getCRel());
			if (time < nodes[node]->getTEncounter()) {
				Writing w(-time, nodes[node]->getIntensity(), 1.0 / nodes[node]->getVelocity().getY());
				std::cout << std::setprecision(2) << "writing: " << nodes[node]->getIntensity() << " "
						  << 1.0 / nodes[node]->getVelocity().getY() << std::endl;
				sensor.addWriting(w);
			}
		}
	}

	return encounters;
}

void Solver::handleReflection() {
	double delta = 0.5;
	for (int i = 0; i < nodesNum; i++) {
		if (nodes[i]->getTEncounter() < ZERO && nodes[i]->getTEncounter() > -delta) {
			if (nodes[i]->getObstacleNumber() >= 0) {
				Node reflected = nodes[i]->getReflected(obstacles[nodes[i]->getObstacleNumber()]);
				Node refracted = nodes[i]->getRefracted(obstacles[nodes[i]->getObstacleNumber()]);
				reflected.setMaterial(nodes[i]->getMaterial());
				refracted.setMaterial(nodes[i]->getMaterial() >= 0 ? -1 : nodes[i]->getObstacleNumber());
				if (reflected.getIntensity() == -1) {
					nodes[i]->setMarkedForTheKill(1);
				} else {
					// real neighbors always turn to ghost ones -
					// reflected go to the other direction, refracted are in another material
					reflected.setTEncounter(INFINITY);
					refracted.setTEncounter(INFINITY);

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
//						refracted.setIntensity(nodes[i]->getIntensity());
						right->addRightVirtualNeighbor(&reflected);
						right->addLeftVirtualNeighbor(&refracted);
						nodes[i]->setRight(right);
					}
					nodes[i]->restoreWavefront(reflected, refracted);
				}
			} else if (nodes[i]->getObstacleNumber() == -1) {    // encountering a dot obstacle
				double sina = -nodes[i]->getVelocity().getY();
				double cosa = -nodes[i]->getVelocity().getX();
				double alpha = cosa > 0 ? asin(sina) : M_PI - asin(sina);
				double dalpha = M_PI / (POINTS_IN_DOT_WAVEFRONT - 1);

				alpha += M_PI / 2;
				int oldNodesNum = nodesNum;
				for (int j = 0; j < POINTS_IN_DOT_WAVEFRONT; j++) {
					Node *n = new Node(dots[nodes[i]->getVerticeNumber()], alpha);
					n->setTEncounter(INFINITY);
					n->setMaterial(nodes[i]->getMaterial());
					nodes[nodesNum++] = n;
					alpha -= dalpha;
				}
				for (int j = 1; j < POINTS_IN_DOT_WAVEFRONT - 1; j++) {
					nodes[oldNodesNum + j]->setLeft(nodes[oldNodesNum + j - 1]);
					nodes[oldNodesNum + j]->setRight(nodes[oldNodesNum + j + 1]);
				}
				nodes[oldNodesNum]->setRight(nodes[oldNodesNum + 1]);
				nodes[nodesNum - 1]->setLeft(nodes[nodesNum - 2]);
			}
			//should be 2 lines above
			nodes[i]->setTEncounter(INFINITY);

		}
	}
}
void Solver::fixNodes() {
	for (int node = 0; node < nodesNum; node++) {
		nodes[node]->checkInvalid();

		if (nodes[node]->getMarkedForTheKill() > 0) {
			for (auto &sensor : sensors) {
				for (int j = 0; j < sensor.getWriting().size(); j++) {
					if (sensor.getWriting()[j].getNode() == nodes[node]) {
						std::vector<Writing> wr = sensor.getWriting();
						wr[j].setNode(NULL);
						sensor.setWriting(wr);
						sensor.clearWriting();
					}
				}
			}
//			delete nodes[node];
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
			if ((!nodes[i]) || (nodes[i]->getMarkedForTheKill() > 0)) {
				nodes[i] = nodes[--nodesNum];
				cleared = false;
				break;
			}
		}
	}


	for (int node = 0; node < nodesNum; node++) {
		nodes[node]->clearNeighbours();
	}
}
Solver::~Solver() {
	for (int i = 0; i < nodesNum; i++) {
		if (nodes[i] != NULL) {
//			delete nodes[i];
		}
	}
}

void Solver::resetTime() {
	startTime = SENSORS * DX_SENSORS * 0.1;
	finishTime = 2.0 * Y;
}

void Solver::deteriorate() {
	for (int n = 0; n < nodesNum; n++) {
		nodes[n]->deteriorate();
	}
	for (auto &sensor : sensors) {
		sensor.deteriorate();
	}
}

void Solver::writeToCSV() {
	std::ofstream csvFile;
	csvFile.open("data/baseline/Sensor" + std::to_string(s) + "/raw" + std::to_string(s) + ".csv", std::ios_base::app);
	for (int i = 0; i < sensors.size(); i++) {
		sensors[i].writeToCSV(csvFile);
	}
	csvFile << std::endl;
	csvFile.close();
}

