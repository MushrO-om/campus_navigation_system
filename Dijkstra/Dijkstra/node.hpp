//
//  node.hpp
//  Dijkstra
//
//  Created by Ice Bear on 2021/4/10.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef node_hpp
#define node_hpp

#include <stdio.h>
#include <vector>

using namespace std;

class Node {
public:
    Node(int number):
    number(number),dis(numeric_limits<double>::max()),parent(nullptr){}
    
    Node(int number, double dis):
    number(number),dis(dis),parent(nullptr){}
    
    Node(int number, double dis, Node *parent):
    number(number),dis(dis),parent(parent){}
    
    Node(const Node &node) {
        number = node.get_number();
        dis = node.get_dis();
        parent = node.get_parent();
    }
    
    void set_dis(double dis) { this->dis = dis; }
    void set_parent(Node *parent) {this->parent = parent; }
    
    int get_number() const { return number; }
    double get_dis() const { return dis; }
    Node *get_parent() const { return parent; }
    
private:
    int number;
    double dis;
    Node *parent;
};

#endif /* node_hpp */
