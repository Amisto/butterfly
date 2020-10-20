#include "Node.h"
#include "Constants.h"


Node::Node(const Vector2 &pos,
		   const Vector2 &velocity,
		   int material,
		   double intensity,
		   double t_encounter,
		   int obstacle_number,
		   int vertice_number,
		   Node *left,
		   Node *right,
		   int marked_for_the_kill,
		   const std::vector<Node *> &neighbors_left,
		   const std::vector<Node *> &neighbors_right) {

}

const Vector2 Node::getPos() const {
	return pos;
}

const Vector2 Node::getVelocity() const {
	return velocity;
}

int Node::getMaterial() const {
	return material;
}

double Node::getIntensity() const {
	return intensity;
}

double Node::getTEncounter() const {
	return t_encounter;
}

int Node::getObstacleNumber() const {
	return obstacle_number;
}

int Node::getVerticeNumber() const {
	return vertice_number;
}

Node *Node::getLeft() const {
	return left;
}

Node *Node::getRight() const {
	return right;
}

const std::vector<Node *> Node::getNeighborsLeft() const {
	return neighbors_left;
}

const std::vector<Node *> Node::getNeighborsRight() const {
	return neighbors_right;
}

int Node::getMarkedForTheKill() const {
	return marked_for_the_kill;
}

void Node::setPos(const Vector2 &pos) {
	this->pos = pos;
}

void Node::setVelocity(const Vector2 &velocity) {
	this->velocity = velocity;
}

void Node::setMaterial(int material) {
	this->material = material;
}

void Node::setIntensity(double intensity) {
	this->intensity = intensity;
}

void Node::setTEncounter(double t_encounter) {
	this->t_encounter = t_encounter;
}

void Node::setObstacleNumber(int obstacle_number) {
	this->obstacle_number = obstacle_number;
}

void Node::setVerticeNumber(int vertice_number) {
	this->vertice_number = vertice_number;
}
void Node::setLeft(Node *left) {
	this->left = left;
}

void Node::setRight(Node *right) {
	this->right = right;
}

void Node::setNeighborsLeft(const std::vector<Node *> &neighbors_left) {
	this->neighbors_left = neighbors_left;
}

void Node::setNeighborsRight(const std::vector<Node *> &neighbors_right) {
	this->neighbors_right = neighbors_right;
}

void Node::setMarkedForTheKill(int marked_for_the_kill) {
	this->marked_for_the_kill = marked_for_the_kill;
}

const Vector2 Node::getPosAfterStep(double step) const {
	return Vector2(pos.getX() + velocity.getX() * step, pos.getY() + velocity.getY() * step);
}

double Node::getTime(double dist, double c_rel) const {
	if (material >= 0) {
		return fabs(dist / c_rel);
	} else {
		return dist;
	}
}

bool isOutside(const Node &Node) {
	return Node.getPos().getX() > X || Node.getPos().getX() < 0 || Node.getPos().getY() > Y || Node.getPos().getY() < 0;
}