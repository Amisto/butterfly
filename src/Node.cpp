#include "Node.h"
#define VISIBILITY_THRESHOLD    0.08 //defining constant necessary in 1 method
#define SENSORS                 32 //number of sensors

void Kill()

Node::Node(const Vector2 &pos,
		   const Vector2 &velocity,
		   int material,
		   double intensity,
		   double t_encounter,
		   int obstacle_number,
		   int vertice_number,
		   Node *left, 
		   Node *right,
		   int kill_marked,
		   const std::vector<Node *> &virtual_neighbors_left,
		   const std::vector<Node *> &virtual_neighbors_right) {

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

const std::vector<Node *> Node::getVirtualLeft() const {
	return virtual_neighbors_left;
}

const std::vector<Node *> Node::getVirtualRight() const {
	return virtual_neighbors_right;
}

int Node::getMarkedForTheKill() const {
	return kill_marked;
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

void Node::setVirtualLeft(const std::vector<Node *> &virtual_neighbors_left) {
	this->virtual_neighbors_left = virtual_neighbors_left;
}

void Node::setVirtualRight(const std::vector<Node *> &virtual_neighbors_right) {
	this->virtual_neighbors_right = virtual_neighbors_right;
}

void Node::setMarkedForTheKill(int kill_marked) {
	this->kill_marked = kill_marked;
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

void Node::update(double timeStep, double c_rel) {
	pos.setX(velocity.getX() * timeStep * (material >= 0 ? c_rel : 1.0));
	pos.setY(velocity.getX() * timeStep * (material >= 0 ? c_rel : 1.0));

	if ((t_encounter <= INFINITY) && (t_encounter != -1)) {
		t_encounter -= timeStep;
	}
}

Node Node::getReflected(Obstacle obstacle) {
	double i = intensity;
	return Node(Vector2(1.00015 * velocity.getX(), 1.00015 * velocity.getY()),
				::getReflected(obstacle.getPos(vertice_number), obstacle.getPos(vertice_number + 1),
							   velocity, obstacle.getCRel(), &i),
				i);
}

Node Node::getRefracted(Obstacle obstacle) {
	double i = intensity;
	return Node(Vector2(1.00015 * velocity.getX(), 1.00015 * velocity.getY()),
				::getRefracted(obstacle.getPos(vertice_number), obstacle.getPos(vertice_number + 1),
							   velocity, obstacle.getCRel(), &i),
				i);
}

void Node::addLeftVirtualNeighbor(Node *neighbor) {
	virtual_neighbors_left.push_back(neighbor);
}

void Node::addRightVirtualNeighbor(Node *neighbor) {
	virtual_neighbors_right.push_back(neighbor);
}

bool isOutside(Node *Node) {
	return Node->getPos().getX() > X || Node->getPos().getX() < 0 || Node->getPos().getY() > Y || Node->getPos().getY() < 0;
}

void Node::virtualHandler(Node &ray, bool isRightNeighbor) {
	// boolean "isRightNeighbor" allows to avoid code duplication

	// managing ghost neighbors is trickier
	// ghost neighbor turn into a real one in only one case - if they share a material id
	// and they have approximately coinciding velocities and not too far away

	if (ray.getMaterial() == this->getMaterial()
		&& scalar(ray.getVelocity(), this->getVelocity()) > 0
		&& length(ray.getPos(), this->getPos()) < 5 * MINLEN) {
		// if a wavefront is already restored, we don't bifurcate it
		if (!this->getLeft() && isRightNeighbor) {
			ray.setRight(this);
			this->setLeft(&ray);
		} else if (!this->getRight() && !isRightNeighbor) {
			ray.setLeft(this);
			this->setRight(&ray);
			this->setTEncounter(INFINITY);
		}
	}
}
void Node::restoreWavefront(Node &reflected, Node &refracted) {
	for (int j = 0; j < virtual_neighbors_left.size(); j++) {
		virtual_neighbors_left[j]->virtualHandler(reflected, false);
		virtual_neighbors_left[j]->virtualHandler(refracted, false);
	}
	for (int j = 0; j < virtual_neighbors_right.size(); j++) {
		virtual_neighbors_right[j]->virtualHandler(reflected, true);
		virtual_neighbors_right[j]->virtualHandler(refracted, true);
	}
}

void Node::killLeft()
{
	for (int j = 0; j < this->virtual_neighbors_left.size(); j++)
                    if (this->virtual_neighbors_left[j])
                        for (int k = 0; k < this->virtual_neighbors_left[j]->virtual_neighbors_right.size(); k++)
                            if (this->virtual_neighbors_left[j]->virtual_neighbors_right[k] ==this)
                                this->virtual_neighbors_left[j]->virtual_neighbors_right[k] = NULL; 
};
void Node::killRight(){
	for (int j = 0; j < this->virtual_neighbors_right.size(); j++)
            if (this->virtual_neighbors_right[j])
                for (int k = 0; k < this->virtual_neighbors_right[j]->virtual_neighbors_left.size(); k++)
                    if (this->virtual_neighbors_right[j]->virtual_neighbors_left[k] == this)
                        this->virtual_neighbors_right[j]->virtual_neighbors_left[k] = NULL;
};

void Node::marking()
{
	if (intensity < VISIBILITY_THRESHOLD
                || isOutside(this)
                || (!this->left && !this->right && !(this->virtual_neighbors_left.size()) &&
                    !(this->virtual_neighbors_right.size()))
                    )
                this->kill_marked = 1;
};

void Node::checkInvalid(Sensor sensors[SENSORS]){
    if (this->kill_marked) {
        if (this->left) this->left->right = NULL;
        if (this->right) this->right->left = NULL;
		killLeft();
		killRight();
        this->virtual_neighbors_left.clear();
        this->virtual_neighbors_right.clear();
        for (int s = 0; s < SENSORS; s++)
            for (int j = 0; j < sensors[s].getWriting().size(); j++)
                if (sensors[s].getWriting()[j].getNode() == this){
                        sensors[s].clearWriting();}
        delete this;
        }

    }
};

