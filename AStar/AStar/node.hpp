//
//  Node.hpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef node_hpp
#define node_hpp

#include <stdio.h>
#include <iostream>

using namespace std;

class Node;

class Node {
public:
    Node();
    Node(int x, int y);
    Node(int x, int y, Node *parent, int g, int h);
    
    bool equals(Node node) const;
    bool equalxy(int x, int y) const;
    
    int get_x(){return x;}
    int get_y(){return y;}
    int get_g(){return G;}
    int get_h(){return H;}
    int get_f(){return G+H;}
    Node* get_parent(){return parent;}
    
    void set_x(int x){this->x = x;}
    void set_y(int y){this->y = y;}
    void set_g(int g){G = g;}
    void set_h(int h){H = h;}
    void set_parent(Node *node){parent = node;}
    
    void show_node(){cout<<"("<<x<<","<<y<<")";}
    
private:
    int x;
    int y;
    Node *parent;
    int G;
    int H;
};

#endif /* node_hpp */
