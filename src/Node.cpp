#include "Node.h"

const Vector2 Node::GetPos() const {
  return pos;
}

const Vector2 &Node::GetVelocity() const {
  return velocity;
}

int Node::GetMaterial() const {
  return material;
}

double Node::GetIntensity() const {
  return intensity;
}

double Node::GetTEncounter() const {
  return t_encounter;
}

int Node::GetObstacleNumber() const {
  return obstacle_number;
}

int Node::GetVerticeNumber() const {
  return vertice_number;
}

Node *Node::GetLeft() const {
  return left;
}

Node *Node::GetRight() const {
  return right;
}

const std::vector<Node *> &Node::GetNeighborsLeft() const {
  return neighbors_left;
}

const std::vector<Node *> &Node::GetNeighborsRight() const {
  return neighbors_right;
}

int Node::GetMarkedForTheKill() const {
  return marked_for_the_kill;
}

void Node::SetPos(const Vector2 &pos) {
  this->pos = pos;
}

void Node::SetVelocity(const Vector2 &velocity) {
  this->velocity = velocity;
}

void Node::SetMaterial(int material) {
  this->material = material;
}

void Node::SetIntensity(double intensity) {
  this->intensity = intensity;
}

void Node::SetTEncounter(double t_encounter) {
  this->t_encounter = t_encounter;
}

void Node::SetObstacleNumber(int obstacle_number) {
  this->obstacle_number = obstacle_number;
}

void Node::SetVerticeNumber(int vertice_number) {
  this->vertice_number = vertice_number;
}
void Node::SetLeft(Node *left) {
  this->left = left;
}

void Node::SetRight(Node *right) {
  this->right = right;
}

void Node::SetNeighborsLeft(const std::vector<Node *> &neighbors_left) {
  this->neighbors_left = neighbors_left;
}

void Node::SetNeighborsRight(const std::vector<Node *> &neighbors_right) {
  this->neighbors_right = neighbors_right;
}

void Node::SetMarkedForTheKill(int marked_for_the_kill) {
  this->marked_for_the_kill = marked_for_the_kill;
}
