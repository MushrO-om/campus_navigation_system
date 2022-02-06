#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <QVector>
#include <QMap>
#include <QSet>

#include "edge.h"
#include "vertex.h"
#include "constants.h"

class Edge;
class Vertex;

class Dijkstra
{
public:
    Dijkstra();

    //一旦中途改变优先度,速度或者终点等等，都需要重新构造dijkstra进行寻路
    //r为搜索半径，ldis为当前点到左端节点的距离，rdis为到右端节点的距离

    void singleSearch(int begin, qreal r, qreal v);
    void doubleSearch(int begin1, int begin2, qreal r, qreal v, qreal ldis, qreal rdis);
    void singleFind(int begin, int end, Priority priority, qreal v);
    void doubleFind(int begin1, int begin2, int end, Priority priority, qreal v, qreal ldis, qreal rdis);

    void start();
    void search();

    void insert_to_visit(int x);

    //两点寻路总路径
    QVector<Vertex*> get_dij_path() const {return dij_path;}
    //两点寻路总距离
    qreal get_total_dis() const {return dis[end];}
    //两点寻路总时间，输出还需要格式化
    qreal get_total_time() const {return time[end];}

    //返回搜索结果（点、距离、路径）
    QMap<int,qreal> get_dis() const {return dis;}
    QMap<int,qreal> get_time() const {return time;}
    QMap<int,QVector<Vertex*>> get_paths() const {return paths;}

    void set_adj_map(const QVector<QMap<int,Edge*>> &adj_map) {this->adj_map = adj_map;}
    void set_vertices(const QVector<Vertex*> &vs) {vertices = vs;}
    void set_dij_path(const QVector<Vertex*> &path) {dij_path = path;}
    void set_begin(int bg) {begin = bg;}
    void set_end(int ed) {end = ed;}
    void set_total_dis(qreal dis) {this->dis[end] = dis;}
    void set_total_time(qreal t) {this->time[end] = t;}
    void set_priority(const Priority &pri) {priority = pri;}

private:
    QVector<QMap<int,Edge*>> adj_map;
    QVector<Vertex*> vertices;
    QVector<int> to_visit;      //下标并不对应节点编号
    QMap<int,int> in_visited;
    QMap<int,int> parent;       //parent[i]表示节点i的上一节点

    QVector<Vertex*> dij_path;  //返回的路径数组，应包括起点和终点

    //存储搜到点的路径(用于searchNear)
    QMap<int,QVector<Vertex*>> paths;

    //根据priority改变dis的值，dis[i]表示节点i到起点的距离
    QMap<int,qreal> dis;
    //time[i]表示节点i到起点的时间，= dis[i]/velocity*crd
    QMap<int,qreal> time;

    int begin;
    int end;
//    int dont_go;
    Priority priority;
    qreal velocity;            //用哪种交通工具（涉及通行性问题）（FOOT,Bike)

    qreal radius;               //单点的搜索半径

    //用于位于节点时的查询
    int single_begin;
};

#endif // DIJKSTRA_H
