//
//  dijkstra.cpp
//  Dijkstra
//
//  Created by Ice Bear on 2021/4/10.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include <iostream>
#include <stack>
#include "dijkstra.hpp"
#include "node.hpp"

Dijkstra::Dijkstra(vector<vector<double>> adj, Node* begin, Node* end):adj_mat(adj),begin_node(begin),end_node(end) {
    visited.push_back(begin_node);
    for(int i = 0; i<adj.size(); i++) {
        //如果是自身，跳过
        if(i==begin_node->get_number()) continue;
        if(i==end_node->get_number()) {
            end_node->set_dis(adj_mat[begin_node->get_number()][i]);
            to_visit.push_back(end_node);
        } else {
            Node *new_node = new Node(i,adj_mat[begin_node->get_number()][i]);
            to_visit.push_back(new_node);
        }
    }
    sort(to_visit.begin(), to_visit.end(), cmp);
}

Dijkstra::~Dijkstra() {
    for(auto &item : visited) delete item;
    for(auto &item : to_visit) delete item;
}

//按dis降序排列
bool Dijkstra::cmp(Node *node1, Node *node2) {
    return node1->get_dis()>node2->get_dis();
}

void Dijkstra::start() {
    while(!to_visit.empty()) {
        //取距离最小的
        Node *current = to_visit.back();
        to_visit.pop_back();
        
        //设置他的父亲
        if(current->get_parent()==nullptr)
            current->set_parent(begin_node);
        
        //放入visited
        visited.push_back(current);
        
        //如果是终点就结束
        if(current==end_node) break;
        
        //找到所有和current邻接的节点,更新距离
        for(auto &item : to_visit) {
            if(adj_mat[current->get_number()][item->get_number()]!=numeric_limits<double>::max()&&
               item->get_dis()>current->get_dis()+adj_mat[current->get_number()][item->get_number()])
            {
                item->set_dis(current->get_dis()+adj_mat[current->get_number()][item->get_number()]);
                item->set_parent(current);
            }
        }
        sort(to_visit.begin(),to_visit.end(),cmp);
    }
    print();
}

void Dijkstra::print() {
    cout<<"从起点"<<begin_node->get_number()<<"到终点"<<end_node->get_number()<<"的距离是："<<end_node->get_dis()<<endl;
    cout<<"路径是: ";
    Node *p = end_node;
    stack<int> stk;
    while(p) {
        stk.push(p->get_number());
        p = p->get_parent();
    }
    while(!stk.empty()) {
        int temp = stk.top();
        stk.pop();
        cout<<temp;
        if(!stk.empty()) cout<<" -> ";
    }
}
//
//void Dijkstra::show_to_visit() {
//    cout<<"to_visit:"<<endl;
//    for(auto item : to_visit) {
//        cout<<item->get_number()<<" "<<item->get_dis()<<" ";
//        if(item->get_parent()==nullptr) cout<<"-1"<<endl;
//        else cout<<item->get_parent()->get_number()<<endl;
//    }
//}
//
//void Dijkstra::show_visited() {
//    cout<<"visited:"<<endl;
//    for(auto item : visited) {
//        cout<<item->get_number()<<" "<<item->get_dis()<<" ";
//        if(item->get_parent()==nullptr) cout<<"-1"<<endl;
//        else cout<<item->get_parent()->get_number()<<endl;
//    }
//}
