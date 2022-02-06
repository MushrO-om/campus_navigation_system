#include "dijkstra.h"

Dijkstra::Dijkstra()
{
}

void Dijkstra::singleSearch(int begin, qreal r, qreal v) {
    this->single_begin = begin;
    this->radius = r;
    this->velocity = v;
    this->priority = Pr_Distance;

    to_visit.clear();
    in_visited.clear();
    parent.clear();
    paths.clear();
    dis.clear();
    time.clear();

    insert_to_visit(single_begin);
    dis[single_begin] = 0;
    time[single_begin] = 0;
    search();
}

void Dijkstra::doubleSearch(int begin1, int begin2, qreal r, qreal v, qreal ldis, qreal rdis) {
    to_visit.clear();
    in_visited.clear();
    parent.clear();
    paths.clear();
    dis.clear();
    time.clear();

    velocity = v;

    Dijkstra d1;
    d1.set_adj_map(adj_map);
    d1.set_vertices(vertices);
    Dijkstra d2;
    d2.set_adj_map(adj_map);
    d2.set_vertices(vertices);

    d1.singleSearch(begin1, r-ldis, v);
    d2.singleSearch(begin2, r-rdis, v);

    qreal crd = adj_map[begin1][begin2]->get_crowdness();

    for(QMap<int,QVector<Vertex*>>::iterator iter = d1.paths.begin(); iter!=d1.paths.end(); iter++) {
        d1.dis[iter.key()] += ldis;
        d1.time[iter.key()] += ldis/(velocity*crd);
    }

    //把结果合并存在d1中
    for(QMap<int,QVector<Vertex*>>::iterator iter = d2.paths.begin(); iter!=d2.paths.end(); iter++) {
        d2.dis[iter.key()] += rdis;
        d2.time[iter.key()] += rdis/(velocity*crd);
        //如果d1中没搜到key
        if(d1.paths.count(iter.key()) || d1.paths[iter.key()].isEmpty()) {
            d1.dis[iter.key()] = d2.dis[iter.key()];
            d1.time[iter.key()] = d2.time[iter.key()];
            d1.paths[iter.key()] = iter.value();
        }
        //如果d1中搜到了，选距离更小的，改距离，改路线
        else if(d1.dis[iter.key()]>d2.dis[iter.key()]) {
            d1.dis[iter.key()] = d2.dis[iter.key()];
            d1.time[iter.key()] = d2.time[iter.key()];
            d1.paths[iter.key()] = iter.value();
        }
    }
    //把结果存到当前对象里
    paths = d1.paths;
    dis = d1.dis;
    time = d1.time;
}

void Dijkstra::singleFind(int begin, int end, Priority priority, qreal v) {
    to_visit.clear();
    in_visited.clear();
    parent.clear();
    dij_path.clear();
    dis.clear();
    time.clear();

    this->begin = begin;
    this->end = end;
    this->priority = priority;
    this->velocity = v;

    insert_to_visit(begin);
    dis[begin] = 0;
    time[begin] = 0;
    start();
}

void Dijkstra::doubleFind(int begin1, int begin2, int end, Priority priority, qreal v, qreal ldis, qreal rdis) {
    to_visit.clear();
    in_visited.clear();
    parent.clear();
    dij_path.clear();
    dis.clear();
    time.clear();

    this->end = end;

    qreal crd = adj_map[begin1][begin2]->get_crowdness();
    Dijkstra d1;
    d1.set_adj_map(adj_map);
    d1.set_vertices(vertices);
    Dijkstra d2;
    d2.set_adj_map(adj_map);
    d2.set_vertices(vertices);

    d1.singleFind(begin1,end,priority,v);
    d2.singleFind(begin2,end,priority,v);

    //两点寻路只用得到dis[end]或time[end]
    if(d1.dis[end]+ldis<d2.dis[end]+rdis) {
        dis[end] = d1.dis[end]+ldis;
        time[end] = d1.time[end]+ldis/(crd*velocity);
        dij_path = d1.dij_path;
    } else {
        dis[end] = d2.dis[end]+rdis;
        time[end] = d2.time[end]+rdis/(crd*velocity);
        dij_path = d2.dij_path;
    }
}

void Dijkstra::insert_to_visit(int x) {
    if(to_visit.contains(x)) return;
    if(priority==Pr_Distance) {
        for(int i = 0;i<to_visit.size();i++) {
            if(dis[to_visit[i]]>dis[x]) {
                to_visit.insert(i,x);
                return;
            }
        }
    }
    else {
        for(int i = 0;i<to_visit.size();i++) {
            if(time[to_visit[i]]>time[x]) {
                to_visit.insert(i,x);
                return;
            }
        }
    }
    //如果size==0或者所有都比x小，x尾插
    to_visit.push_back(x);
}

void Dijkstra::search() {
    while(!to_visit.empty()) {
        int current = to_visit.front();
        to_visit.pop_front();
        //要是最小距离都大于radius，退出
        if(dis[current]>radius) break;
        in_visited[current] = 1;
        //遍历和current相邻的所有点
        for(QMap<int,Edge*>::const_iterator iter = adj_map[current].begin(); iter!=adj_map[current].end();iter++)
        {
            //如果点不能去、参观过，跳过
            if(in_visited[iter.key()]==1||iter.value()->get_velocity()<velocity) continue;

            //计算距离（时间）
            qreal connectdis = iter.value()->get_distance();
            qreal connecttime = connectdis/(velocity*iter.value()->get_crowdness());
            //更新距离
            if(dis.count(iter.key())==0 || dis[current]+connectdis<dis[iter.key()])
            {
                dis[iter.key()] = dis[current]+connectdis;
                time[iter.key()] = time[current]+connecttime;
                parent[iter.key()] = current;
            }
            if(parent.count(iter.key())==0) parent[iter.key()] = single_begin;
            //加入to_visit
            insert_to_visit(iter.key());
        }//for
        QVector<Vertex*> temp;
        int p = current;
        while(p!=single_begin) {
            temp.push_front(vertices[p]);
            p = parent[p];
        }
        temp.push_front(vertices[p]);
        paths[current] = temp;
    }//while
}

void Dijkstra::start() {
    while(!to_visit.empty()) {
        int current = to_visit.front();
        to_visit.pop_front();
        in_visited[current] = 1;
        if(current==end) break;
        //遍历相邻的点
        for(QMap<int,Edge*>::const_iterator iter = adj_map[current].begin(); iter!=adj_map[current].end();iter++)
        {
            //如果点不能去、参观过、路线不可通行，跳过
            if(in_visited[iter.key()]==1 || iter.value()->get_velocity()<velocity) continue;

            //计算距离
            qreal connectdis = iter.value()->get_distance();
            qreal connecttime = connectdis/(velocity*iter.value()->get_crowdness());
            //更新距离
            if((priority==Pr_Distance&&(dis.count(iter.key())==0||dis[current]+connectdis<dis[iter.key()]))||
               (priority==Pr_Time&&(time.count(iter.key())==0||time[current]+connecttime<time[iter.key()])))
            {
                dis[iter.key()] = dis[current]+connectdis;
                time[iter.key()] = time[current]+connecttime;
                parent[iter.key()] = current;
            }
            if(parent.count(iter.key())==0) parent[iter.key()] = begin;
            //加入to_visit
            insert_to_visit(iter.key());
        }//for
    }//while
    //获取路径
    int p = end;
    while(p!=begin) {
        dij_path.push_front(vertices[p]);
        p = parent[p];
    }
    dij_path.push_front(vertices[begin]);
}
