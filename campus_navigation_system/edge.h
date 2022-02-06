#ifndef EDGE_H
#define EDGE_H

#include <QtGlobal>
#include "constants.h"

class Edge
{
public:
    Edge();
    Edge(qreal dis, qreal crd, qreal v);

    qreal get_distance() {return distance;}
    qreal get_crowdness() {return crowdness;}
    qreal get_velocity() {return velocity;}

    void set_distance(qreal dis) {distance = dis;}
    void set_crowdness(qreal crd) {crowdness = crd;}
    void set_velocity(qreal v) {velocity = v;}

private:
    qreal distance;        //距离
    qreal crowdness;       //拥挤度
    qreal velocity;        //可允许的速度
};

#endif // EDGE_H
