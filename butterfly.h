#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

using namespace std;

// Acoustics => only P-waves, refracting and reflecting.
// No fixed timestep is used, we iterate between ray-obstacle encounters. Sensors will be also considered obstacles.
// On each time step, we calculate the closest encounter of a ray and an obstacle fragment, move all rays accordingly
// and split the ray on an obstacle.

//==================================================================================================================
//=== calc consts

#define OBSTACLES_TOTAL         10
#define DOTS_TOTAL              100
#define VERTICES                5
#define ZERO                    0.000001
#define MINLEN                  0.1//2.25
#define PI                      3.14159265
#define VISIBILITY_THRESHOLD    0.08
#define POINTS_IN_DOT_WAVEFRONT 4000

#define C0                      1.0

#define SENSORS                 32
#define DX_SENSORS              4.6875//5.625
#define DT_DIGITIZATION         16.0e-1//0.0000001
#define DT_CARRYING             52.0e-1//7.5//0.0000003
#define DT_WIDTH                208.0e-1//37.5//0.0000021
#define T_MULTIPLIER            0.005
#define DT_DETERIORATION        1.0
#define DETERIORATION           0.9999

double CURRENT_DT_DETERIORATION = 0;

#define X            500.0
#define Y            500.0

double T_START_BASE = SENSORS * DX_SENSORS * 0.1 / C0, T_FINISH_BASE = 2.0 * Y / C0;
double T_START = T_START_BASE, T_FINISH = T_FINISH_BASE;

double PIES = 6.0;

//==================================================================================================================
//=== basic data structs

struct V2 {
    double x, y;
};

struct Obstacle     // a closed polyline with a different material inside
{
    V2 pos[VERTICES];     // vertices of polyline
    double c_rel;   // c / c_0 - relation between basic
} obstacles[OBSTACLES_TOTAL];
int OBSTACLES;

struct Dot {
    V2 pos;
    double brightness;
} dots[DOTS_TOTAL];
int DOTS;

struct Node         // a ray
{
    V2 vel;                                 // the velocity is normalized
    int material;                           // which obstacle material to use, -1 - background material
    V2 pos;
    double intensity;

    // next obstacle parameters
    double t_encounter;                     // next obstacle encounter time, if t < 0 - no encounters expected
    int obstacle_number, vertice_number;    // directions to next obstacle - this will depend on Obstacle organization
    // negative obstacle number is a dot number

    // to adjust performance dynamically, we want to track wavefronts and adjust the rays number
    Node *left, *right;                     // basic, "real" neighbors - they are used to maintain the uniformness of the ray front
    vector<Node *> neighbors_left, neighbors_right;
    // additional, "virtual", "ghost" neighbors - they are used to track reflected/refracted wavefronts

    int kill_marked;
} 
*nodes[300000] = {NULL};
int n_nodes;

struct Writing {
    double time;
    double brightness;
    double frequency_correction; //if a ray falls at a target at an angle
    Node *node;
};

struct Sensor {
    V2 pos;
    vector <Writing> writing;
} sensors[SENSORS];

double total_time = 0;
unsigned long int written = 0;

//==================================================================================================================
//=== basic math

double scalar(V2 a, V2 b) //done.
{
    return a.x * b.x + a.y * b.y;
}

double length(V2 a, V2 b) //done.
{
    V2 tmp = {a.x - b.x, a.y - b.y};
    return sqrt(scalar(tmp, tmp));
}

double det(double a, double b, double c, double d) { //gonna die.
    return a * d - b * c;
}

double area(V2 a, V2 b, V2 c) { //done.
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool intersect(V2 a, V2 b, V2 c, V2 d, V2 *res) { //done.
    if (!(area(a, d, b) * area(a, b, c) > 0 && area(d, b, c) * area(d, c, a) > 0))
        return false;
    double t = ((c.y - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y)) /
               ((c.x - d.x) * (b.y - a.y) - (c.y - d.y) * (b.x - a.x));
    res->x = c.x + (c.x - d.x) * t;
    res->y = c.y + (c.y - d.y) * t;
    return true;
}

bool intersect(V2 a, V2 b, V2 c, V2 v, double *dist) { // segment ab, ray cd done.
    V2 d = {c.x + v.x * 1000, c.y + v.y * 1000};
    if (!(area(a, d, b) * area(a, b, c) > 0 && area(d, b, c) * area(d, c, a) > 0))
        return false;
    *dist = ((c.y - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y)) /
            ((c.x - d.x) * (b.y - a.y) - (c.y - d.y) * (b.x - a.x)) * length(c, d);
    return true;
}

bool point_in_rect(V2 x, V2 a, V2 b, V2 c, V2 d) //done.
{
    bool a1 = area(x, a, b) > -ZERO,
            a2 = area(x, b, c) > -ZERO,
            a3 = area(x, c, d) > -ZERO,
            a4 = area(x, d, a) > -ZERO;
    return (a1 == a2 && a2 == a3 && a3 == a4);
}

void get_reflected(V2 a, V2 b, V2 pos, V2 vel, V2 *res) //done.
{
    double sina = -vel.y, cosa = vel.x, sinb = (b.y - a.y) / length(a, b), cosb = (b.x - a.x) / length(a, b);
    res->x = cosa * (cosb * cosb - sinb * sinb) - 2.0 * sina * sinb * cosb;
    res->y = sina * (cosb * cosb - sinb * sinb) + 2.0 * cosa * sinb * cosb;
}

void get_refracted(V2 a, V2 b, V2 pos, V2 vel, double c_rel, V2 *res, double *intensity_reflected,
                   double *intensity_refracted) //done.
{
    double sing = (b.y - a.y) / length(a, b), cosg = (b.x - a.x) / length(a, b);
    double sinf = vel.y, cosf = vel.x;
    double cosa = cosg * cosf + sing * sinf;
    double sina = sing * cosf - sinf * cosg;
    double cosb = c_rel * cosa;
    if (cosb > 1.0 || cosb < -1.0) {
        //printf("cos exceeded 1 - full inner reflection?\n");
        //cosb = 1;
        *intensity_reflected = -1;
        return;
        //exit(0);
    }
    double sinb = sqrt(1 - cosb * cosb);
    if (sina < 0) sinb *= -1;
    res->x = cosg * cosb + sing * sinb;
    res->y = sing * cosb - cosg * sinb;

    double z1 = 1 * cosa, z2 = c_rel * cosb;
    *intensity_refracted *= fabs(2 * z2 / (z2 + z1));//0.5;//(sina - c_rel * sinb) / (sina + c_rel * sinb);
    *intensity_reflected *= fabs((z2 - z1) / (z2 + z1));//0.5;//sina / (sina + c_rel * sinb);
    //printf("%lf %lf \n", *intensity_reflected, *intensity_refracted);
}

double dist_to_segment(V2 a, V2 b, V2 va, V2 vb, V2 c) //seems to be done.
{
    double t2 = (va.x * (a.y - b.y) + va.y * (b.x - a.x)) / (va.x * vb.y - va.y * vb.y);
    V2 o = {(a.x + b.x) / 2, (a.y + b.y) / 2};//{b.x + vb.x * t2, b.y + vb.y * t2};
    double dist = length(o, c);
    return dist;
}

bool outside(Node *node) //done.
{
    return node->pos.x > X || node->pos.x < 0 || node->pos.y > Y || node->pos.y < 0;
}

//==================================================================================================================
//=== csv drawing

FILE *f_csv = NULL;

double signal(double t, double fc) {
    return sin(M_PI * t / DT_WIDTH) * sin(M_PI * t / DT_WIDTH) * sin(2 * M_PI * t / DT_CARRYING * fc);
}

void write_to_csv() { //old
    for (int i = 0; i < SENSORS; i++) {
        //instead sensors[i].writeToCSV();
        double _signal = 0;
        for (int j = 0; j < sensors[i].writing.size(); j++) {
            if (sensors[i].writing[j].time > 0)
                _signal += sensors[i].writing[j].brightness *
                           signal(sensors[i].writing[j].time, sensors[i].writing[j].frequency_correction);
            sensors[i].writing[j].time += DT_DIGITIZATION;
        }
        fprintf(f_csv, "%5.2lf ", _signal);

        bool nulls_exist = true;
        while (nulls_exist) {
            nulls_exist = false;
            for (int j = 0; j < sensors[i].writing.size(); j++)
                if (sensors[i].writing[j].time > DT_WIDTH) {
                    sensors[i].writing.erase(sensors[i].writing.begin() + j);
                    nulls_exist = true;
                    break;
                }
        }
    }

    fprintf(f_csv, "\n");
    written++;
}
void WholeWriteToCSV() { //new
    for (int i = 0; i < SENSORS; i++)
    {
        sensors[i].writeToCSV();
    }
    
    fprintf(f_csv, "\n");
    written++;
}

//==================================================================================================================
//=== calc core

int calculation_split_step = 0;     // just for a more convenient visualization
int step = 0;

void connect(Node *left, Node *right) {
    if (!left || !right) {
        printf("it's a null fucking pointer, meat bastard\n");
        return;
    }
    left->right = right;
    right->left = left;
}

int consistency_check() {
    //TODO: fix this
    for (int i = 0; i < n_nodes; i++) {
        if (nodes[i]->left)
            if (nodes[i] != nodes[i]->left->right) {
                nodes[i]->left = nodes[i]->left->right = NULL;
                //return 0;
            }
        if (nodes[i]->right)
            if (nodes[i] != nodes[i]->right->left) {
                nodes[i]->right = nodes[i]->right->left = NULL;
                //return 0;
            }
    }
    return 1;
}

void refine() {
    // refine the front based on real neighbors
    int n_nodes_old = n_nodes;
    bool coarse = true;
    while (coarse) {
        coarse = false;
        n_nodes_old = n_nodes;
        for (int i = 0; i < n_nodes_old; i++) {
            if (!nodes[i]) printf("!!! mesh holes !!!\n");
            if (nodes[i]->right) {
                Node *l = nodes[i], *r = nodes[i]->right;
                if (length(l->pos, r->pos) > MINLEN * 2) {
                    Node *node = new Node;
                    node->pos.x = (l->pos.x + r->pos.x) / 2;
                    node->pos.y = (l->pos.y + r->pos.y) / 2;
                    node->vel.x = (l->vel.x + r->vel.x) / 2;
                    node->vel.y = (l->vel.y + r->vel.y) / 2;
                    connect(node, r);
                    connect(l, node);
                    l->t_encounter = INFINITY;
                    node->kill_marked = 0;
                    node->material = l->material;
                    node->neighbors_left.clear();
                    node->neighbors_right.clear();
                    node->t_encounter = INFINITY;
                    node->intensity = (l->intensity + r->intensity) / 2;
                    nodes[n_nodes++] = node;
                    coarse = true;
                }
            }
        }
    }
}

void deteriorate() {
    for (int n = 0; n < n_nodes; n++)
        nodes[n]->intensity *= DETERIORATION;
    for (int i = 0; i < SENSORS; i++)
        for (int j = 0; j < sensors[i].writing.size(); j++)
            sensors[i].writing[j].brightness *= DETERIORATION;
}

int nonidiocy = 1;

void calc_a_step() {
    step++;
    if (nonidiocy && !consistency_check()) {
        nonidiocy = 0;
        printf("I'm with an idiot %d\n", step);
    }

    refine();
    double dist, time = INFINITY;
    //if (!calculation_split_step)
    {

        //== calculating next encounters

        int node = -1, encountered;
        for (int n = 0; n < n_nodes; n++) {
            if (nodes[n]->t_encounter == INFINITY) {
                encountered = 0;

                // primarily, we check continuous obstacles
                for (int i = 0; i < OBSTACLES; i++)
                    for (int j = 0; j < VERTICES - 1; j++) {
                        if (intersect(obstacles[i].pos[j], obstacles[i].pos[j + 1], nodes[n]->pos, nodes[n]->vel,
                                      &dist)) {
                            time = fabs(dist / (nodes[n]->material >= 0 ? obstacles[nodes[n]->material].c_rel : 1.0));
                            if (time < nodes[n]->t_encounter) {
                                nodes[n]->t_encounter = time;
                                nodes[n]->obstacle_number = i;
                                nodes[n]->vertice_number = j;
                                encountered++;
                            }
                        }
                    }

                // then we check dots, but only if the node has a segment attached - we have no better way to treat wavefront segments
                if (nodes[n]->right) {
                    // checking dots
                    V2 n_now = {nodes[n]->pos.x + nodes[n]->vel.x * 10, nodes[n]->pos.y + nodes[n]->vel.y * 10};
                    V2 r_now = {nodes[n]->right->pos.x + nodes[n]->right->vel.x * 10,
                                nodes[n]->right->pos.y + nodes[n]->right->vel.y * 10};
                    V2 n_next = {nodes[n]->pos.x + nodes[n]->vel.x * 1000, nodes[n]->pos.y + nodes[n]->vel.y * 1000};
                    V2 r_next = {nodes[n]->right->pos.x + nodes[n]->right->vel.x * 1000,
                                 nodes[n]->right->pos.y + nodes[n]->right->vel.y * 1000};
                    for (int i = 0; i < DOTS; i++) {
                        if (point_in_rect(dots[i].pos, n_now, r_now, r_next, n_next)) {
                            double dist = dist_to_segment(nodes[n]->pos, nodes[n]->right->pos, nodes[n]->vel,
                                                          nodes[n]->right->vel, dots[i].pos);
                            time = fabs(dist / (nodes[n]->material >= 0 ? obstacles[nodes[n]->material].c_rel : 1.0));
                            if (time < nodes[n]->t_encounter) {
                                nodes[n]->t_encounter = time;
                                nodes[n]->obstacle_number = -1;
                                nodes[n]->vertice_number = i;
                                encountered++;
                                //printf("ENC DOT: %d %lf %lf %lf\n", n, nodes[n]->pos.x, nodes[n]->pos.y, dist);
                            }
                        }
                    }

                    // checking sensors
                    for (int i = 0; i < SENSORS; i++) {
                        if (point_in_rect(sensors[i].pos, n_now, r_now, r_next, n_next)) {
                            double dist = dist_to_segment(nodes[n]->pos, nodes[n]->right->pos, nodes[n]->vel,
                                                          nodes[n]->right->vel, sensors[i].pos);
                            time = fabs(dist / (nodes[n]->material >= 0 ? obstacles[nodes[n]->material].c_rel : 1.0));
                            if (time < nodes[n]->t_encounter)
                                sensors[i].writing.push_back(
                                        Writing{-time, nodes[n]->intensity, 1.0 / nodes[n]->vel.y});
                        }
                    }
                }
                if (!encountered) {
                    nodes[n]->t_encounter = -1;
                    //nodes[n]->kill_marked = 1;
                }
            }
        }

        time = INFINITY;
        for (int i = 0; i < n_nodes; i++)
            if (nodes[i]->t_encounter > -0.5 && nodes[i]->t_encounter < time)
                time = nodes[i]->t_encounter; // there's our time step

        if (time > DT_DIGITIZATION * T_MULTIPLIER) time = DT_DIGITIZATION * T_MULTIPLIER;

        //== moving all the nodes

        for (int i = 0; i < n_nodes; i++) {
            nodes[i]->pos.x +=
                    nodes[i]->vel.x * time * C0 * (nodes[i]->material >= 0 ? obstacles[nodes[i]->material].c_rel : 1.0);
            nodes[i]->pos.y +=
                    nodes[i]->vel.y * time * C0 * (nodes[i]->material >= 0 ? obstacles[nodes[i]->material].c_rel : 1.0);
            if (nodes[i]->t_encounter < -0.5 || nodes[i]->t_encounter == INFINITY) continue;
            nodes[i]->t_encounter -= time;
        }
    }

    //if (calculation_split_step == 1)
    {

        //== dealing with reflection/refraction

        for (int i = 0; i < n_nodes; i++) {              // considering each node
            if (nodes[i]->t_encounter < ZERO &&
                nodes[i]->t_encounter > -0.5) {      // processing all the nodes are contacting on this time step
                if (nodes[i]->obstacle_number >= 0) { // encountering a continuous obstacle
                    // velocity and intensity calculations
                    V2 vel_0, vel_1;
                    double i_0, i_1;
                    get_reflected(obstacles[nodes[i]->obstacle_number].pos[nodes[i]->vertice_number],
                                  obstacles[nodes[i]->obstacle_number].pos[nodes[i]->vertice_number + 1], nodes[i]->pos,
                                  nodes[i]->vel, &vel_0);
                    i_0 = i_1 = nodes[i]->intensity;
                    get_refracted(obstacles[nodes[i]->obstacle_number].pos[nodes[i]->vertice_number],
                                  obstacles[nodes[i]->obstacle_number].pos[nodes[i]->vertice_number + 1], nodes[i]->pos,
                                  nodes[i]->vel,
                                  nodes[i]->material == -1 ? obstacles[nodes[i]->obstacle_number].c_rel : 1.0 /
                                                                                                          obstacles[nodes[i]->obstacle_number].c_rel,
                                  &vel_1, &i_0, &i_1);
                    if (i_0 == -1) {
                        nodes[i]->kill_marked = 1;
                    } else {
                        Node *reflected = new Node;
                        Node *refracted = new Node;
                        reflected->pos = nodes[i]->pos;
                        refracted->pos = nodes[i]->pos;
                        reflected->vel = vel_0;
                        refracted->vel = vel_1;
                        reflected->intensity = i_0;
                        refracted->intensity = i_1;
                        // a kind of summoning sickness - new fronts appear at a little distance from the border
                        reflected->pos.x -= 1.00015 * nodes[i]->vel.x;
                        reflected->pos.y -= 1.00015 * nodes[i]->vel.y;
                        refracted->pos.x += 1.00015 * nodes[i]->vel.x;
                        refracted->pos.y += 1.00015 * nodes[i]->vel.y;
                        nodes[n_nodes++] = reflected;
                        nodes[n_nodes++] = refracted;
                        reflected->material = nodes[i]->material;           // reflected material is the same as node's
                        refracted->material = (nodes[i]->material >= 0 ? -1 : nodes[i]->obstacle_number);
                        //refracted material is either background or obstacle number
                        reflected->t_encounter = INFINITY;                  // INFINITY => marked for collision processing on the next time step
                        refracted->t_encounter = INFINITY;

                        refracted->kill_marked = reflected->kill_marked = 0;
                        nodes[i]->kill_marked = 1;           // original node is marked for the kill on the next step

                        //== managing neighbors - the cornerstone of the algorithm

                        reflected->neighbors_left.clear();
                        reflected->neighbors_right.clear();
                        refracted->neighbors_left.clear();
                        refracted->neighbors_right.clear();
                        refracted->left = refracted->right = reflected->left = reflected->right = NULL;

                        if (nodes[i]->left)  // real neighbors always turn to ghost ones -
                        {                   // reflected go to the other direction, refracted are in another material
                            reflected->neighbors_left.push_back(nodes[i]->left);
                            nodes[i]->left->neighbors_right.push_back(reflected);
                            refracted->neighbors_left.push_back(nodes[i]->left);
                            nodes[i]->left->neighbors_right.push_back(refracted);
                        }
                        if (nodes[i]->right) {
                            reflected->neighbors_right.push_back(nodes[i]->right);
                            nodes[i]->right->neighbors_left.push_back(reflected);
                            refracted->neighbors_right.push_back(nodes[i]->right);
                            nodes[i]->right->neighbors_left.push_back(refracted);
                        }

                        // managing ghost neighbors is trickier
                        // ghost neighbor turn into a real one in only one case - if they share a material id
                        // and they have approximately coinciding velocities
                        // and not too far away
                        //
                        // the algorithm is designed to restore a wavefront after reflection

                        for (int j = 0; j < nodes[i]->neighbors_left.size(); j++) {

                            if (reflected->material == nodes[i]->neighbors_left[j]->material
                                && scalar(reflected->vel, nodes[i]->neighbors_left[j]->vel) > 0
                                && length(reflected->pos, nodes[i]->neighbors_left[j]->pos) < 5 * MINLEN
                                    ) {
                                if (!nodes[i]->neighbors_left[j]->right)   // if a wavefront is already restored, we don't bifurcate it
                                {
                                    connect(nodes[i]->neighbors_left[j], reflected);
                                    nodes[i]->neighbors_left[j]->t_encounter = INFINITY;
                                }
                                //else printf("Wavefront bifurcation prevented in %d node, materials %d %d\n", i, reflected->material, refracted->material);
                            }
                            if (refracted->material == nodes[i]->neighbors_left[j]->material
                                && scalar(refracted->vel, nodes[i]->neighbors_left[j]->vel) > 0
                                && length(refracted->pos, nodes[i]->neighbors_left[j]->pos) < 5 * MINLEN
                                    ) {
                                if (!nodes[i]->neighbors_left[j]->right) {
                                    connect(nodes[i]->neighbors_left[j], refracted);
                                    nodes[i]->neighbors_left[j]->t_encounter = INFINITY;
                                }
                                //else printf("Wavefront bifurcation prevented in %d node, materials %d %d\n", i, reflected->material, refracted->material);
                            }
                        }
                        for (int j = 0; j < nodes[i]->neighbors_right.size(); j++) {

                            if (reflected->material == nodes[i]->neighbors_right[j]->material
                                && scalar(reflected->vel, nodes[i]->neighbors_right[j]->vel) > 0
                                && length(reflected->pos, nodes[i]->neighbors_right[j]->pos) < 5 * MINLEN
                                    ) {
                                if (!nodes[i]->neighbors_right[j]->left)   // if a wavefront is already restored, we don't bifurcate it
                                    connect(reflected, nodes[i]->neighbors_right[j]);
                                //else printf("Wavefront bifurcation prevented in %d node, materials %d %d\n", i, reflected->material, refracted->material);
                            }
                            if (refracted->material == nodes[i]->neighbors_right[j]->material
                                && scalar(refracted->vel, nodes[i]->neighbors_right[j]->vel) > 0
                                && length(refracted->pos, nodes[i]->neighbors_right[j]->pos) < 5 * MINLEN
                                    ) {
                                if (!nodes[i]->neighbors_right[j]->left)
                                    connect(refracted, nodes[i]->neighbors_right[j]);
                                //else printf("Wavefront bifurcation prevented in %d node, materials %d %d\n", i, reflected->material, refracted->material);
                            }
                        }
                    }
                } else if (nodes[i]->obstacle_number == -1) // encountering a dot obstacle
                {
                    double sina = -nodes[i]->vel.y, cosa = -nodes[i]->vel.x, alpha, dalpha;
                    if (cosa > 0) alpha = asin(sina);
                    else alpha = M_PI - asin(sina);
                    alpha += M_PI / 2;
                    dalpha = M_PI / (POINTS_IN_DOT_WAVEFRONT - 1);
                    int old_n_nodes = n_nodes;
                    for (int j = 0; j < POINTS_IN_DOT_WAVEFRONT; j++) {
                        Node *n = new Node;
                        if (!n) {
                            printf("no fucking space \n");
                            exit(-1);
                        }
                        nodes[n_nodes++] = n;
                        n->intensity = nodes[i]->intensity * dots[nodes[i]->vertice_number].brightness;
                        n->kill_marked = 0;
                        n->material = nodes[i]->material;
                        n->neighbors_left.clear();
                        n->neighbors_right.clear();
                        n->obstacle_number = n->vertice_number = 0;
                        n->t_encounter = INFINITY;
                        n->vel.x = cos(alpha);
                        n->vel.y = sin(alpha);
                        alpha -= dalpha;
                        n->pos.x = dots[nodes[i]->vertice_number].pos.x + n->vel.x * 0.01;
                        n->pos.y = dots[nodes[i]->vertice_number].pos.y + n->vel.y * 0.01;
                        n->right = n->left = NULL;
                    }
                    for (int j = 1; j < POINTS_IN_DOT_WAVEFRONT - 1; j++) {
                        nodes[old_n_nodes + j]->left = nodes[old_n_nodes + j - 1];
                        nodes[old_n_nodes + j]->right = nodes[old_n_nodes + j + 1];
                    }
                    nodes[old_n_nodes]->right = nodes[old_n_nodes + 1];
                    nodes[n_nodes - 1]->left = nodes[n_nodes - 2];
                    nodes[i]->t_encounter = INFINITY;
                }
            }
        }
    }

    //if (calculation_split_step == 2)
    {
        // killing rampage
        for (int i = 0; i < n_nodes; i++)
            //nodes[i].marking(); //instead
            if (nodes[i]->intensity < VISIBILITY_THRESHOLD
                || outside(nodes[i])
                || (!nodes[i]->left && !nodes[i]->right && !(nodes[i]->neighbors_left.size()) &&
                    !(nodes[i]->neighbors_right.size()))
                    )// || nodes[i]->t_encounter < -0.5)
                nodes[i]->kill_marked = 1;

        for (int i = 0; i < n_nodes; i++) {
            //nodes[i].checkInvalid(); //instead
            if (nodes[i]->kill_marked) {
                if (nodes[i]->left) nodes[i]->left->right = NULL;
                if (nodes[i]->right) nodes[i]->right->left = NULL;
                // nodes[i].killRught();
                // nodes[i].killLeft();
                //instead
                for (int j = 0; j < nodes[i]->neighbors_left.size(); j++)
                    if (nodes[i]->neighbors_left[j])
                        for (int k = 0; k < nodes[i]->neighbors_left[j]->neighbors_right.size(); k++)
                            if (nodes[i]->neighbors_left[j]->neighbors_right[k] == nodes[i])
                                nodes[i]->neighbors_left[j]->neighbors_right[k] = NULL; // dead neighbors are marked as NULLs and deleted later
                for (int j = 0; j < nodes[i]->neighbors_right.size(); j++)
                    if (nodes[i]->neighbors_right[j])
                        for (int k = 0; k < nodes[i]->neighbors_right[j]->neighbors_left.size(); k++)
                            if (nodes[i]->neighbors_right[j]->neighbors_left[k] == nodes[i])
                                nodes[i]->neighbors_right[j]->neighbors_left[k] = NULL;
                nodes[i]->neighbors_left.clear();
                nodes[i]->neighbors_right.clear();
                for (int s = 0; s < SENSORS; s++)
                    for (int j = 0; j < sensors[s].writing.size(); j++)
                        if (sensors[s].writing[j].node == nodes[i])
                            sensors[s].writing[j].node = NULL;
                delete nodes[i];
                nodes[i] = NULL;
            }
        }
    //stayed here
        bool cleared = false;
        while (!cleared) {
            while (!nodes[n_nodes - 1] && n_nodes) n_nodes--;
            cleared = true;
            for (int i = 0; i < n_nodes; i++) {
                if (!nodes[i]) {
                    nodes[i] = nodes[--n_nodes];
                    cleared = false;
                    break;
                }
            }
        }

        for (int i = 0; i < n_nodes; i++)       // fancy clearing of neighbors' vecros
        {
            //nodes[i].clearNeighbours() instead
            bool nulls_exist = true;

            while (nulls_exist) {
                nulls_exist = false;
                for (int j = 0; j < nodes[i]->neighbors_left.size() && !nulls_exist; j++)
                    if (!nodes[i]->neighbors_left[j]) {
                        nodes[i]->neighbors_left.erase(nodes[i]->neighbors_left.begin() + j);
                        nulls_exist = true;
                    }
            }

            nulls_exist = true;
            while (nulls_exist) {
                nulls_exist = false;
                for (int j = 0; j < nodes[i]->neighbors_right.size() && !nulls_exist; j++)
                    if (!nodes[i]->neighbors_right[j]) {
                        nodes[i]->neighbors_right.erase(nodes[i]->neighbors_right.begin() + j);
                        nulls_exist = true;
                    }
            //out there
            }
        }
    }
    //if (calculation_split_step == 3)

    //printf("%d %d\n", calculation_split_step, n_nodes);
    //for (int i=0; i<n_nodes; i++)
    //    printf("%d %d %lf %lf %lf\n", i, nodes[i]->material, nodes[i]->pos.x, nodes[i]->pos.y, nodes[i]->t_encounter);

    //calculation_split_step = calculation_split_step == 3 ? 0 : calculation_split_step + 1;

    CURRENT_DT_DETERIORATION += time;
    while (CURRENT_DT_DETERIORATION > DT_DETERIORATION) {
        deteriorate();
        CURRENT_DT_DETERIORATION -= DT_DETERIORATION;
    }

    total_time += time;
    while (total_time > DT_DIGITIZATION) {
        if (T_START < 0) write_to_csv();
        total_time -= DT_DIGITIZATION;
        T_START -= DT_DIGITIZATION;
        T_FINISH -= DT_DIGITIZATION;
    }

}

//==================================================================================================================
//=== general management

void init_explosion(double _x, double _y) {
    n_nodes = 0;
    int n = POINTS_IN_DOT_WAVEFRONT * 2;
    for (int i = 0; i < n; i++) {
        Node *temp = new Node;
        temp->material = -1;
        temp->neighbors_left.clear();
        temp->neighbors_right.clear();
        temp->t_encounter = INFINITY;
        temp->right = temp->left = NULL;
        nodes[n_nodes++] = temp;
        temp->intensity = 1.0;
        temp->kill_marked = 0;

        double angle = 0;
        angle = 2 * M_PI * i / (double) n;
        //if (i < n/2)
        //    angle = - M_PI/20 - (M_PI*18.0/20.0) * i/(double)n*2;
        //else
        //    angle = M_PI/20 + (M_PI*18.0/20.0) * i/(double)n*2;
        temp->vel.x = cos(angle);
        temp->vel.y = sin(angle);
        temp->pos.x = _x;
        temp->pos.y = _y;
    }
    for (int i = 1; i < n - 1; i++) {
        nodes[i]->left = nodes[i - 1];
        nodes[i]->right = nodes[i + 1];
    }
    nodes[0]->right = nodes[1];
    nodes[0]->left = nodes[n - 1];
    nodes[n - 1]->left = nodes[n - 2];
    nodes[n - 1]->right = nodes[0];
}

void init_from_file(char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        printf("No obstacle data file\n");
        exit(-1);
    }

    fscanf(f, "%d", &OBSTACLES);
    for (int i = 0; i < OBSTACLES; i++) {
        for (int j = 0; j < VERTICES - 1; j++)
            if (fscanf(f, "%lf%lf", &obstacles[i].pos[j].x, &obstacles[i].pos[j].y) != 2) {
                printf("Not enough obstacle data\n");
                exit(-1);
            }
        if (fscanf(f, "%lf", &obstacles[i].c_rel) != 1) {
            printf("Not enough obstacle data\n");
            exit(-1);
        }

        /*int SIN_VERTICES = VERTICES - 5;
        double L = obstacles[i].pos[0].x - obstacles[i].pos[3].x;
        double DL = L/SIN_VERTICES;
        double AMP = 2.5;
        double l;
        for (int j=4; j<VERTICES - 1; j++)
        {
            l = DL*(j-3);
            obstacles[i].pos[j].x = obstacles[i].pos[3].x + l; 
            obstacles[i].pos[j].y = obstacles[i].pos[3].y + AMP*sin(l/L*M_PI*PIES);
        }
        */
        obstacles[i].pos[VERTICES - 1].x = obstacles[i].pos[0].x;
        obstacles[i].pos[VERTICES - 1].y = obstacles[i].pos[0].y;
    }

    fscanf(f, "%d", &DOTS);
    for (int i = 0; i < DOTS; i++)
        if (fscanf(f, "%lf%lf%lf", &dots[i].pos.x, &dots[i].pos.y, &dots[i].brightness) != 3)
            //if(fscanf(f, "%lf%lf", &dots[i].pos.x, &dots[i].pos.y) != 2)
        {
            printf("Not enough dots data\n");
            exit(-1);
        } else dots[i].brightness = 0.1;

    fclose(f);

    // Round obstacles generation
    /*
    OBSTACLES = 1;
    obstacles[OBSTACLES-1].c_rel = 3.0;
    double ox = A*0.5, oy = A*0.3, r = A*0.1, da = 2*M_PI/(VERTICES-1);
    for (int i=0; i<VERTICES - 1; i++)
    {
        obstacles[OBSTACLES-1].pos[i].x = ox + r*cos(da*i);
        obstacles[OBSTACLES-1].pos[i].y = oy + r*sin(da*i);
    }
    obstacles[OBSTACLES-1].pos[VERTICES - 1].x = obstacles[OBSTACLES-1].pos[0].x;
    obstacles[OBSTACLES-1].pos[VERTICES - 1].y = obstacles[OBSTACLES-1].pos[0].y;
    */
}

void finalize() {
    for (int i = 0; i < n_nodes; i++) {
        nodes[i]->neighbors_left.clear();
        nodes[i]->neighbors_right.clear();
        delete nodes[i];
    }
    n_nodes = 0;
}

