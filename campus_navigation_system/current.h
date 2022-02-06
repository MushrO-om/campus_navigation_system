#ifndef CURRENT_H
#define CURRENT_H

#include <QString>
#include <QVector>
#include <QMap>
#include "constants.h"
#include "vertex.h"
#include "edge.h"

class Edge;
class Vertex;

class Current
{
public:
    //初始化默认在沙河校门（0号节点），全程只有一个current，因此也只有一个初始化，但是可以通过set等改变current
    //不能跳转到路中间，因此不存在current_dis等状态跳变的情况
    Current(const QVector<QMap<int,Edge*>> &adj_map, const QVector<Vertex*> &vertices);

//    Current(qreal x, qreal y, qreal v, Priority pri, const QVector<QMap<int,Edge*>> &adj_map,
//            const QVector<Vertex*> &vertices, const QVector<Vertex*> &path, qreal total_dis);

    void set_x(qreal x) {this->x = x;}
    void set_y(qreal y) {this->y = y;}
    void set_priority(Priority pri) {priority = pri;}
    void set_velocity(qreal v) {velocity = v;}
    void set_last_ver(Vertex *ver) {last_ver = ver;}
    void set_next_ver_index(int ver) {next_ver_index = ver;}
    void set_total_dis(qreal dis) {total_dis = dis;}
    void set_current_dis(qreal dis) {current_dis = dis;}
    void set_total_pass(qreal pass) {total_pass = pass;}
    void set_current_pass(qreal pass) {current_pass = pass;}
    void set_total_time(qreal t) {total_time = t;}
    void set_total_pass_time(qreal t) {total_pass_time = t;}
    void set_path(const QVector<Vertex*> &path) {this->path = path;}

    qreal get_x() {return x;}
    qreal get_y() {return y;}
    qreal get_velocity() {return velocity;}
    Priority get_priority() {return priority;}
    Vertex* get_last_ver() {return last_ver;}
    Vertex* get_next_ver() {return next_ver_index==-1?nullptr:path[next_ver_index];}
    int get_next_ver_index() {return next_ver_index;}
    qreal get_total_dis() {return total_dis;}
    qreal get_current_dis() {return current_dis;}
    qreal get_total_pass() {return total_pass;}
    qreal get_current_pass() {return current_pass;}
    qreal get_total_time() {return total_time;}
    qreal get_total_pass_time() {return total_pass_time;}
    QVector<Vertex*> get_path() {return path;}

    bool inShahe();
    bool inBenbu();
    bool inBetween();

private:
    qreal x;
    qreal y;

    //当前速度由current.velocity*edge.crowdness得到
    qreal velocity;             //速度(STOP/FOOT/BIKE/BUS/SHUTTLE)
    Priority priority;          //选择策略

    QVector<QMap<int,Edge*>> adj_map;
    QVector<Vertex*> vertices;

    //path包括起点
    QVector<Vertex*> path;      //途径数组

    //当current_pass==0时，last_ver为当前节点
    Vertex* last_ver;           //上一节点

    int next_ver_index;

    qreal total_dis;            //总距离
    qreal current_dis;          //当前边的长度
    qreal total_pass;           //总已走距离
    qreal current_pass;         //当前已走距离（距离上一个节点的距离）

    qreal total_time;           //总所需时间
    qreal total_pass_time;      //总已经历的时间
};

#endif // CURRENT_H
