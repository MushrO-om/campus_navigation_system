#ifndef ANT_H
#define ANT_H

#include <QVector>
#include <QMap>
#include <ctime>
#include <math.h>
#include <stdio.h>
#include <algorithm>

#include "dijkstra.h"
#include "edge.h"
#include "vertex.h"

class Vertex;
class Dijkstra;

class Ant
{
public:
    Ant();

    void set_abmap(const QMap<int,QMap<int,Dijkstra>> &abmap) {this->abmap = abmap;}
//    void set_adj_map(const QVector<QMap<int,Edge*>> &adj_map) {this->adj_map = adj_map;}
//    void set_vertices(const QVector<Vertex*> &vs) {vertices = vs;}

    void start(const QVector<int> &spots, Priority priority);
    void clearAll();

    void shuffle();
    void move();
    void record();
    void record_inall();
    void update();

    void init_ph();
    void init_delta_ph();
    void init_visibility();
    void init_tabu();
    void clear_tabu();

    qreal rnd(qreal lower, qreal uper);

    //结果距离
    qreal get_min_dis_inall() const {return  min_dis_inall;}
    //结果时间
    qreal get_min_time_inall() const {return min_time_inall;}
    //结果路径
    QVector<int> get_min_rout_inall() const {return min_rout_inall;}

private:

    QMap<int,QMap<int,Dijkstra>> abmap;

    //需要途经的城市（用作ant算法内地点编号与全部地点中编号的映射）
    //算法内都采用虚拟编号，只有在用adj_map及输出的时候需要用到真实编号（映射）
    QVector<int> spots;

    Priority priority;

    //用于记录返回的最短路径
    QVector<Vertex*> ant_path;

    QVector<QVector<int>> tabu;    //禁忌表(一次迭代对应一个禁忌表）
    QVector<QVector<qreal>> pheromone;   //信息素浓度
    QVector<QVector<qreal>> visibility;   //能见度
    QVector<QVector<qreal>> delta_ph;   //信息素浓度变化量

    //数组记录每次迭代的信息
    QVector<qreal> min_dis_vector;         //记录每次迭代的最小距离
    QVector<qreal> min_time_vector;
    QVector<QVector<int>> min_rout_vector;
    //记录每次迭代最短路径(min_rout[i][j]的值表示第i次迭代的第j个到达地）

    //记录单次迭代的信息
    QVector<qreal> ants_dis_inone;     //每次迭代记录第i只蚂蚁走的距离
    QVector<qreal> ants_time_inone;
    qreal min_dis_inone;       //一次迭代中的蚂蚁走的最小距离
    qreal min_time_inone;
    int min_index_inone;          //一次迭代中走的最小距离对应的蚂蚁编号

    //记录所有迭代中最好一次的信息（即结果）
    qreal min_dis_inall;            //所有迭代中的最短距离
    qreal min_time_inall;
    qreal min_index_inall;          //所有迭代中最短距离的下标
    QVector<int> min_rout_inall;    //所有迭代中最短距离的路径（真实编号）

    int ants_num = 58;            //蚂蚁总数
    qreal ph_ratio = 0.8;       //信息素比例因子
    qreal vis_ratio = 4.2;      //能见度比例因子
    int Nc_max = 100;        //最大迭代次数
    qreal ph_de_ratio = 0.75;    //信息素减少比例
    qreal ph_in_ratio = 500;        //信息素增加强度系数

    int nc;         //当前迭代轮次
};

#endif // ANT_H
