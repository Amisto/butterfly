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
  void SetPos(const Vector2 &pos);
  void SetVelocity(const Vector2 &velocity);
  void SetMaterial(int material);
  void SetIntensity(double intensity);
  void SetTEncounter(double t_encounter);
  void SetObstacleNumber(int obstacle_number);
  void SetVerticeNumber(int vertice_number);
  void SetLeft(Node *left);
  void SetRight(Node *right);
  void SetNeighborsLeft(const std::vector<Node *> &neighbors_left);
  void SetNeighborsRight(const std::vector<Node *> &neighbors_right);
  void SetMarkedForTheKill(int marked_for_the_kill);

  const Vector2 GetPos() const;
  const Vector2 GetVelocity() const;
  int GetMaterial() const;
  double GetIntensity() const;
  double GetTEncounter() const;
  int GetObstacleNumber() const;
  int GetVerticeNumber() const;
  Node *GetLeft() const;
  Node *GetRight() const;
  const std::vector<Node *> GetNeighborsLeft() const;
  const std::vector<Node *> GetNeighborsRight() const;
  int GetMarkedForTheKill() const;

};

#endif //SOLVER_NODE_H
