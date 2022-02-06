
#include "current.h"

//初始化默认在沙河校门（0号节点），全程只有一个current，因此也只有一个初始化，但是可以通过set等改变current
Current::Current(const QVector<QMap<int,Edge*>> &adj_map, const QVector<Vertex*> &vertices):
    velocity(STOP), priority(Pr_Distance), adj_map(adj_map), vertices(vertices),
    total_dis(0), current_dis(0), total_pass(0), current_pass(0)
{
    x = vertices[0]->get_x();
    y = vertices[0]->get_y();
    last_ver = vertices[0];
    next_ver_index = -1;
}

//判断当前在哪个地图中
bool Current::inShahe() {
    if(next_ver_index == -1) return last_ver->get_no()<=SHAHE_MAX_NO;
    else return last_ver->get_no()<=SHAHE_MAX_NO && path[next_ver_index]->get_no()<=SHAHE_MAX_NO;
}

bool Current::inBenbu() {
    if(next_ver_index == -1) return last_ver->get_no()>=BENBU_BUS_VER_NO && last_ver->get_no()<=BENBU_MAX_NO;
    else return last_ver->get_no()>=BENBU_BUS_VER_NO && last_ver->get_no()<=BENBU_MAX_NO &&
            path[next_ver_index]->get_no()>=BENBU_BUS_VER_NO && path[next_ver_index]->get_no()<=BENBU_MAX_NO;
}

bool Current::inBetween() {
    //当current既不在沙河也不在本部，返回true
    return !(inShahe()||inBenbu());
}
