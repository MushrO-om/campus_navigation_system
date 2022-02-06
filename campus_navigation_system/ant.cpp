
#include "ant.h"

using namespace std;

Ant::Ant()
{

}

//开始寻路
void Ant::start(const QVector<int> &spots, Priority priority) {
    this->spots = spots;
    this->priority = priority;

    clearAll();

    init_ph();
    init_delta_ph();
    init_visibility();
    init_tabu();
    nc = 0;

    while(nc<Nc_max) {
        clear_tabu();
        shuffle();
        move();
        record();
        update();
        nc++;
    }
    record_inall();
}

//清空（初始化）
void Ant::clearAll() {
    ant_path.clear();

    tabu.clear();
    pheromone.clear();
    visibility.clear();
    delta_ph.clear();

    min_dis_vector.clear();
    min_time_vector.clear();
    min_rout_vector.clear();

    ants_dis_inone.clear();
    ants_time_inone.clear();
    min_rout_inall.clear();
}

//优先度是时间的话，能见度就是时间的倒数
//通行度不足的道路能见度为0，之后计算概率也会为0，轮盘赌不会选择该道路
void Ant::init_visibility() {
    for(int i = 0; i<spots.size(); i++) {
        QVector<qreal> temp;
        for(int j = 0; j<spots.size(); j++) {
            if(i==j) {
                temp.append(0);
                continue;
            }
            //距离优先，能见度为距离
            if(priority==Pr_Distance)
                temp.push_back(1.0/abmap[spots[i]][spots[j]].get_total_dis());
            //时间优先，相当于权值变为了时间
            else if(priority==Pr_Time)
                temp.push_back(1.0/abmap[spots[i]][spots[j]].get_total_time());
        }
        visibility.push_back(temp);
    }
}

void Ant::init_ph() {
    for(int i = 0; i<spots.size(); i++) {
        QVector<qreal> temp;
        for(int j = 0; j<spots.size(); j++) {
            temp.push_back(1.0);
        }
        pheromone.push_back(temp);
    }
}

void Ant::init_delta_ph() {
    for(int i = 0; i<spots.size(); i++) {
        QVector<qreal> temp;
        for(int j = 0; j<spots.size(); j++) {
            temp.push_back(0);
        }
        delta_ph.push_back(temp);
    }
}

void Ant::init_tabu() {
    for(int i = 0; i<ants_num; i++) {
        QVector<int> temp;
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

qreal Ant::rnd(qreal lower, qreal uper)    //生成lower和uper之间的一个double类型随机数
{
    srand((unsigned)time(nullptr));
    return  (rand() / (qreal)RAND_MAX) * (uper - lower) + lower;
}

//初始化每只蚂蚁的起点
void Ant::shuffle() {
    vector<int> temp;
    for(int i = 0; i<=ants_num/spots.size(); i++) {
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
            QVector<int> visited;
            QVector<int> to_visit;
            QVector<qreal> p;
            qreal psum = 0;
            int next = 0;

            //初始化visited
            for(int k = 0; k<j; k++) {
                visited.push_back(tabu[i][k]);
            }

            //初始化to_visit
            for(int k = 0; k<spots.size(); k++) {
                //如果k不在visited中，加入to_visit
                if(find(visited.begin(), visited.end(), k) == visited.end()) {
                    to_visit.push_back(k);
                    p.push_back(0.0);
                }
            }

            //计算概率（p对应tovisit）
            for(int k = 0; k<p.size(); k++) {
                //能见度是不变的，信息素是会更新的
                p[k] = pow(pheromone[visited.back()][to_visit[k]],ph_ratio)
                *pow(visibility[visited.back()][to_visit[k]],vis_ratio);
                psum += p[k];
            }

            //轮盘赌决定下一个目的地
            qreal rate = rnd(0.0, psum);
            qreal choose = 0.0;
            for(int k = 0; k<p.size(); k++) {
                choose += p[k];
                if(choose>rate) {
                    next = to_visit[k];
                    break;
                }
            }
            //走向下一个地点（加入禁忌表）
            tabu[i][j] = next;
        }
    }
}

void Ant::update() {
    //计算每条路径的信息素增量
    for(int i = 0; i<ants_num; i++) {
        for(int j = 0; j<spots.size()-1; j++) {
            if(priority==Pr_Distance)
                delta_ph[tabu[i][j]][tabu[i][j+1]] += ph_in_ratio/ants_dis_inone[i];
            else if(priority==Pr_Time)
                delta_ph[tabu[i][j]][tabu[i][j+1]] += ph_in_ratio/ants_time_inone[i];
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
    min_dis_inone = numeric_limits<double>::max();
    min_time_inone = numeric_limits<double>::max();
    min_index_inone = -1;
    for(int i = 0; i<ants_num; i++) {
    //遍历每只蚂蚁的距离和时间
        qreal temp = 0;
        qreal tmptime = 0;
        for(int j = 0; j<spots.size()-1; j++) {
            temp += abmap[spots[tabu[i][j]]][spots[tabu[i][j+1]]].get_total_dis();
            tmptime += abmap[spots[tabu[i][j]]][spots[tabu[i][j+1]]].get_total_time();
        }
        //记录本次迭代中每只蚂蚁走的距离
        ants_dis_inone.push_back(temp);
        ants_time_inone.push_back(tmptime);
        //取单次迭代中最小的距离/时间
        if((priority==Pr_Distance && temp < min_dis_inone)||
                (priority==Pr_Time && tmptime < min_time_inone))
        {
            min_dis_inone = temp;
            //对应最小距离的时间
            min_time_inone = tmptime;
            min_index_inone = i;
        }
    }
    min_dis_vector.push_back(min_dis_inone);
    min_time_vector.push_back(min_time_inone);
    //temp记录本次迭代的最好路径
    QVector<int> temp;
    //temp[j]表示本次迭代中最好路径的第j个到达点(编号为虚拟编号)
    for(int j = 0; j<spots.size(); j++) {
       temp.push_back(tabu[min_index_inone][j]);
    }
    //min_rout_vector记录了每次迭代最好的路径，min_rout_vector[i][j]表示第i次迭代的第j个到达点
    min_rout_vector.push_back(temp);
}

void Ant::record_inall() {
    min_dis_inall = numeric_limits<double>::max();
    min_time_inall = numeric_limits<double>::max();
    //获取所有迭代中最小的距离及其对应迭代次数（下标）
    for(int i = 0; i<Nc_max; i++) {
        if((priority==Pr_Distance && min_dis_vector[i] < min_dis_inall)||
                (priority==Pr_Time && min_time_vector[i] < min_time_inall)) {
            min_dis_inall = min_dis_vector[i];
            min_time_inall = min_time_vector[i];
            min_index_inall = i;
        }
    }
    //遍历距离最短的那次迭代路径，转换为真实编号，存到结果路径中
    for(int i = 0; i<spots.size(); i++) {
        min_rout_inall.push_back(spots[min_rout_vector[min_index_inall][i]]);
    }
}
