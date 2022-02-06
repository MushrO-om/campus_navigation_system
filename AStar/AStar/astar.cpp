//
//  astar.cpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include "astar.hpp"

using namespace std;

int AStar::BAR = 1;
int AStar::PATH = 2;
int AStar::DIRECT_VALUE = 10;
int AStar::DIAGONAL_VALUE = 14;

AStar::AStar(GridMap gridmap):gridmap(gridmap) {
    cout<<"网格地图如下："<<endl;
    gridmap.show_map();
    int x,y;
    cout<<"请输入起点坐标"<<endl;
    cin>>x>>y;
    while(gridmap.get_value(x, y)==AStar::BAR){
        cout<<"起点不能是障碍，请重新输入起点坐标"<<endl;
        cin>>x>>y;
    }
    begin_node = new Node(x,y);
    cout<<"请输入终点坐标"<<endl;
    cin>>x>>y;
    while(gridmap.get_value(x, y)==AStar::BAR){
        cout<<"终点不能是障碍，请重新输入终点坐标"<<endl;
        cin>>x>>y;
    }
    end_node = new Node(x,y);
    openList.push_back(begin_node);
}

AStar::~AStar() {
    for(auto &item : openList) delete item;
    for(auto &item : closeList) delete item;
}

void AStar::start() {
    cout<<"开始寻路"<<endl;
    move();
    gridmap.show_map();
}

void AStar::move() {
    while(!openList.empty()) {
        Node *current = openList[openList.size()-1];
        openList.pop_back();
        closeList.push_back(current);
        addNeighbor(current);
        
        if(inClose(end_node->get_x(),end_node->get_y())) {
            drawPath();
            break;
        }
    }
    if(openList.empty()) cout<<"没有路"<<endl;
}

//void AStar::addNeighbor(Node *node) {
//    vector<pair<int, int>> directions = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
//    for(int i = 0;i<directions.size();i++) {
//        addOpen(node, node->get_x()+directions[i].first, node->get_y()+directions[i].second, i%2==0?DIRECT_VALUE:DIAGONAL_VALUE);
//        if(end_node->equalxy(node->get_x()+directions[i].first, node->get_y()+directions[i].second)) break;
//    }
//}

void AStar::addNeighbor(Node *node) {
    vector<pair<int, int>> directions = {{1,0},{0,1},{-1,0},{0,-1}};
    for(int i = 0;i<directions.size();i++) {
        addOpen(node, node->get_x()+directions[i].first, node->get_y()+directions[i].second, DIRECT_VALUE);
        if(end_node->equalxy(node->get_x()+directions[i].first, node->get_y()+directions[i].second)) break;
    }
}

static bool cmp(Node *n1, Node *n2) {
    return n1->get_f()>n2->get_f();
}

void AStar::addOpen(Node *current, int x, int y, int value) {
    if(!canOpen(x, y)) return;
    if(inOpen(x, y)) {
        //在openlist中，用g更小的替换
        Node *new_node = findOpen(x, y);
        if (new_node->get_g()>current->get_g()+value) {
            new_node->set_parent(current);
            new_node->set_g(current->get_g()+value);
        }
        openList.push_back(new_node);
        sort(openList.begin(), openList.end(), cmp);
    } else {
        //不在openlist中，计算h后加入
        Node *new_node = new Node(x,y,current,current->get_g()+value,calH(x,y));
        if(end_node->equalxy(new_node->get_x(), new_node->get_y())) {
            Node *p = end_node;
            end_node = new_node;
            delete p;
        }
        openList.push_back(new_node);
        sort(openList.begin(), openList.end(), cmp);
    }
}

void AStar::drawPath() {
    cout<<"总代价为："<<end_node->get_g()<<endl;
    Node *p = end_node;
    while(p) {
        gridmap.set_value(p->get_x(), p->get_y(), PATH);
        p = p->get_parent();
    }
}

bool AStar::canOpen(int x, int y) const {
    //在地图范围内
    if(!gridmap.inMap(x,y)) return false;
    //不是墙
    else if(gridmap.get_value(x, y)==BAR) return false;
    //不在close里
    else if(inClose(x,y)) return false;
    return true;
}

bool AStar::inClose(Node *node) const {
    for(auto item : closeList) {
        if(item->equals(*node)) return true;
    }
    return false;
}

bool AStar::inClose(int x, int y) const {
    for(auto item : closeList) {
        if(item->equalxy(x,y)) return true;
    }
    return false;
}

int AStar::calH(Node *node) {
    return abs(end_node->get_x()-node->get_x())+abs(end_node->get_y()-node->get_y());
}

int AStar::calH(int x, int y) {
    return abs(end_node->get_x()-x)+abs(end_node->get_y()-y);
}

//找到并返回，同时在openList中删除
Node* AStar::findOpen(int x, int y) {
    Node *n = nullptr;
    for(int i = 0;i<openList.size();i++) {
        if(openList[i]->get_x()==x&&openList[i]->get_y()==y)
        {
            n = openList[i];
            openList.erase(openList.begin()+i);
            break;
        }
    }
    return n;
}

bool AStar::inOpen(int x, int y) {
    for(auto node : openList) {
        if(node->get_x()==x&&node->get_y()==y) return true;
    }
    return false;
}
