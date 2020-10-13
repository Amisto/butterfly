#ifndef SOLVER_NODE_H
#define SOLVER_NODE_H

#include "Vector2.h"
#include <vector>

class Node {
 private:
	Vector2 pos;
	Vector2 velocity;                       // the velocity is normalized

	int material;                           // which obstacle material to use, -1 - background material
	double intensity;

	// next obstacle parameters
	double t_encounter; // next obstacle encounter time, if t < 0 - no encounters expected
	int obstacle_number, vertice_number;    // directions to next obstacle - this will depend on Obstacle organization
	// negative obstacle number is a dot number

	// to adjust performance dynamically, we want to track wavefronts and adjust the rays number
	Node *left, *right;  // basic, "real" neighbors - they are used to maintain the uniformness of the ray front
	std::vector<Node *> neighbors_left,
		neighbors_right;  // additional, "virtual", "ghost" neighbors - they are used to track reflected/refracted wavefronts

	int marked_for_the_kill;

 public:
	void setPos(const Vector2 &pos);
	void setVelocity(const Vector2 &velocity);
	void setMaterial(int material);
	void setIntensity(double intensity);
	void setTEncounter(double t_encounter);
	void setObstacleNumber(int obstacle_number);
	void setVerticeNumber(int vertice_number);
	void setLeft(Node *left);
	void setRight(Node *right);
	void setNeighborsLeft(const std::vector<Node *> &neighbors_left);
	void setNeighborsRight(const std::vector<Node *> &neighbors_right);
	void setMarkedForTheKill(int marked_for_the_kill);

	const Vector2 getPos() const;
	const Vector2 getVelocity() const;
	int getMaterial() const;
	double getIntensity() const;
	double getTEncounter() const;
	int getObstacleNumber() const;
	int getVerticeNumber() const;
	Node *getLeft() const;
	Node *getRight() const;
	const std::vector<Node *> getNeighborsLeft() const;
	const std::vector<Node *> getNeighborsRight() const;
	int getMarkedForTheKill() const;

};

#endif //SOLVER_NODE_H
