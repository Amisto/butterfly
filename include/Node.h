#ifndef SOLVER_NODE_H
#define SOLVER_NODE_H

#include "Vector2.h"
#include <vector>
#include <cmath>

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
	Node *real_left_neighbors, *real_right_neighbors;  // basic, "real" neighbors - they are used to maintain the uniformness of the ray front
	std::vector<Node *> virtual_neighbors_left,
		virtual_neighbors_right;  // additional, "virtual", "ghost" neighbors - they are used to track reflected/refracted wavefronts

	int marked_for_the_kill;

 public:
	Node(const Vector2 &pos,
		 const Vector2 &velocity,
		 int material=-1,
		 double intensity=1.0,
		 double t_encounter=INFINITY,
		 int obstacle_number=0,
		 int vertice_number=0,
		 Node *real_left_neighbors=NULL,
		 Node *real_right_neighbors=NULL,
		 int marked_for_the_kill=0,
		 const std::vector<Node *> &virtual_neighbors_left=std::vector<Node *>(),
		 const std::vector<Node *> &virtual_neighbors_right=std::vector<Node *>());

	void setPos(const Vector2 &pos);
	void setVelocity(const Vector2 &velocity);
	void setMaterial(int material);
	void setIntensity(double intensity);
	void setTEncounter(double t_encounter);
	void setObstacleNumber(int obstacle_number);
	void setVerticeNumber(int vertice_number);
	void setLeft(Node *real_left_neighbors);
	void setRight(Node *real_right_neighbors);
	void setNeighborsLeft(const std::vector<Node *> &virtual_neighbors_left);
	void addLeftVirtualNeighbor(Node *neighbor);
	void setNeighborsRight(const std::vector<Node *> &virtual_neighbors_right);
	void addRightVirtualNeighbor(Node *neighbor);
	void setMarkedForTheKill(int marked_for_the_kill);

	const Vector2 getPos() const;
	const Vector2 getPosAfterStep(double step) const;
	const Vector2 getVelocity() const;
	int getMaterial() const;
	double getIntensity() const;
	double getTEncounter() const;
	int getObstacleNumber() const;
	int getVerticeNumber() const;
	Node *getRealLeft() const;
	Node *getRealRight() const;
	const std::vector<Node*> getVirtualLeft() const;
	const std::vector<Node*> getVirtualRight() const;
	int getMarkedForTheKill() const;

	double getTime(double dist, double c_rel) const;
	void update(double timeStep, double c_rel);
	Node getReflected(const Obstacle obstacle);
	Node getRefracted(const Obstacle obstacle);

};

bool isOutside(const Node &Node);

#endif //SOLVER_NODE_H
