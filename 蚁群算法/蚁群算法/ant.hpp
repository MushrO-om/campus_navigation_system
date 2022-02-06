//
//  ant.hpp
//  蚁群算法
//
//  Created by Ice Bear on 2021/4/8.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef ant_hpp
#define ant_hpp

#include <stdio.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

class Ant {
public:
    Ant(vector<vector<double>> adj_mat, vector<int> spots);
    
    void start();
    void shuffle();
    void move();
    void record();
    void print();
    void update();
    
    void init_ph();
    void init_visibility();
    void init_delta_ph();
    void init_tabu();
    void clear_tabu();
    
    void show_tabu();
    void show_ph();
    
    double rnd(double lower, double uper);
    
private:
    vector<int> spots;
    //需要途经的城市（用作ant算法内地点编号与全部地点中编号的映射）
    vector<vector<double>> adj_mat;   //邻接矩阵
    vector<vector<int>> tabu;    //禁忌表(一次迭代对应一个禁忌表）
    vector<vector<double>> pheromone;   //信息素浓度
    vector<vector<double>> visibility;   //能见度
    vector<vector<double>> delta_ph;   //信息素浓度变化量
    vector<double> ave_dis;         //记录每次迭代的平均距离
    vector<double> min_dis;         //记录每次迭代的最小距离
    vector<vector<int>> min_rout;
    //记录每次迭代最短路径(min_rout[i][j]的值表示第i次迭代的第j个到达地）
    
    vector<double> dis;     //每次迭代记录第i只蚂蚁走的距离
    double min_value;       //一次迭代中的蚂蚁走的最小距离
    double sum_value;       //一次迭代中的所有蚂蚁走的距离和
    int min_index;          //一次迭代中走的最小距离对应的蚂蚁编号
    
    double min_dis_inall;   //所有迭代中的最短距离
    double min_index_inall; //所有迭代中最短距离的下标
    
    const int ants_num = 58;            //蚂蚁总数
    const double ph_ratio = 0.8;       //信息素比例因子
    const double vis_ratio = 4.2;      //能见度比例因子
    const int Nc_max = 100;        //最大迭代次数
    const double ph_de_ratio = 0.75;    //信息素减少比例
    const double ph_in_ratio = 500;        //信息素增加强度系数
    
    int nc;         //当前迭代轮次
    
};

Ant::Ant(vector<vector<double>> adj_mat, vector<int> spots) {
    this->adj_mat = adj_mat;
    this->spots = spots;
    init_ph();
    init_delta_ph();
    init_visibility();
    init_tabu();
}

void Ant::init_visibility() {
    for(int i = 0; i<spots.size(); i++) {
        vector<double> temp;
        for(int j = 0; j<spots.size(); j++) {
            temp.push_back(1.0/(double)adj_mat[spots[i]][spots[j]]);
        }
        visibility.push_back(temp);
    }
}

void Ant::init_ph() {
    for(int i = 0; i<spots.size(); i++) {
        vector<double> temp;
        for(int j = 0; j<spots.size(); j++) {
            temp.push_back(1.0);
        }
        pheromone.push_back(temp);
    }
}

void Ant::init_delta_ph() {
    for(int i = 0; i<spots.size(); i++) {
        vector<double> temp;
        for(int j = 0; j<spots.size(); j++) {
            temp.push_back(0);
        }
        delta_ph.push_back(temp);
    }
}

void Ant::init_tabu() {
    for(int i = 0; i<ants_num; i++) {
        vector<int> temp;
        for(int j = 0; j<spots.size(); j++) {
            temp.push_back(0);
        }
        tabu.push_back(temp);
    }
}

void Ant::clear_tabu() {
    for (int i = 0; i < ants_num; i++) {
        for (int j = 0; j < spots.size(); j++) {
            tabu[i][j] = 0;
        }
    }
}

void Ant::show_tabu() {
    for (int i = 0; i < ants_num; i++) {
        for (int j = 0; j < spots.size(); j++) {
            cout << tabu[i][j] << ' ';
        }
        cout << endl;
    }
}

void Ant::show_ph() {
    for (int i = 0; i < spots.size(); i++) {
        for (int j = 0; j < spots.size(); j++) {
            cout << pheromone[i][j] << ' ';
        }
        cout << endl;
    }
}

double Ant::rnd(double lower, double uper)    //生成lower和uper之间的一个double类型随机数
{
    srand((unsigned)time(nullptr));
    return  (rand() / (double)RAND_MAX) * (uper - lower) + lower;
}


void Ant::start() {
    nc = 0;
    while(nc<Nc_max) {
        clear_tabu();
        shuffle();
        move();
        record();
        update();
        nc++;
    }
    print();
}

void Ant::shuffle() {
    vector<int> temp;
    for(int i = 0;i<=ants_num/spots.size();i++) {
        for(int j = 0; j<spots.size(); j++) {
            temp.push_back(j);
        }
    }
    random_shuffle(temp.begin(),temp.end());
    for(int i = 0; i<ants_num; i++) {
        tabu[i][0] = temp[i];
    }
}

void Ant::move() {
    for(int j = 1; j<spots.size(); j++) {
    //j表示第j个地点(除出发点外）（即未经过的地点）
        for(int i = 0; i<ants_num; i++) {
        //i表示第i只蚂蚁
            vector<int> visited;
            vector<int> to_visit;
            vector<double> p;
            double psum = 0;
            int next = 0;
            
            //初始化visited
            for(int k = 0; k<j; k++) {
                visited.push_back(tabu[i][k]);
            }
            
            //初始化to_visit
            for(int k = 0; k<spots.size(); k++) {
                if(find(visited.begin(), visited.end(), k) == visited.end()) {
                    to_visit.push_back(k);
                    p.push_back(0.0);
                }
            }
            
            //计算概率
            for(int k = 0; k<p.size(); k++) {
                p[k] = pow(pheromone[visited.back()][to_visit[k]],ph_ratio)
                *pow(visibility[visited.back()][to_visit[k]],vis_ratio);
                psum += p[k];
            }
            
            //轮盘赌决定下一个目的地
            double rate = rnd(0.0, psum);
            double choose = 0.0;
            for(int k = 0; k<p.size(); k++) {
                choose += p[k];
                if(choose>rate) {
                    next = to_visit[k];
                    break;
                }
            }
            //走向下一个地点
            tabu[i][j] = next;
        }
    }
}

void Ant::update() {
    //计算每条路径的信息素增量
    for(int i = 0; i<ants_num; i++) {
        for(int j = 0; j<spots.size()-1; j++) {
            delta_ph[tabu[i][j]][tabu[i][j+1]] += ph_in_ratio/dis[i];
        }
    }
    
    //更新信息素，先衰退后增加
    for(int i = 0; i<spots.size(); i++) {
        for(int j = 0; j<spots.size(); j++) {
            pheromone[i][j] = pheromone[i][j]*(1-ph_de_ratio)+delta_ph[i][j];
        }
    }
}

void Ant::record() {
    min_value = numeric_limits<double>::max();
    sum_value = 0;
    min_index = -1;
    for(int i = 0; i<ants_num; i++) {
    //遍历每只蚂蚁的距离
        double temp = 0;
        for(int j = 0; j<spots.size()-1; j++) {
            temp += adj_mat[spots[tabu[i][j]]][spots[tabu[i][j+1]]];
        }
        dis.push_back(temp);
        if(temp < min_value) {
            min_value = temp;
            min_index = i;
        }
        sum_value += temp;
    }
    ave_dis.push_back(sum_value/ants_num);
    min_dis.push_back(min_value);
    vector<int> temp;
    for(int j = 0; j<spots.size(); j++) {
       temp.push_back(tabu[min_index][j]);
    }
    min_rout.push_back(temp);
    cout<<"第"<<nc<<"次迭代："<<"最短距离是："<<min_value<<"，平均距离是："<<ave_dis.back()<<endl;
}

void Ant::print() {
    min_dis_inall = numeric_limits<double>::max();
    for(int i = 0; i<Nc_max; i++) {
        if(min_dis[i]<min_dis_inall) {
            min_dis_inall = min_dis[i];
            min_index_inall = i;
        }
    }
    cout<<"最短距离是："<<min_dis_inall<<endl;
    cout<<"产生最短距离的迭代轮次是："<<min_index_inall<<endl;
    cout<<"最短路径是："<<endl<<"start";
    
    for(int i = 0; i<spots.size(); i++) {
        cout << " -> " << spots[min_rout[min_index_inall][i]];
    }
    cout<<endl;
}

#endif /* ant_hpp */
