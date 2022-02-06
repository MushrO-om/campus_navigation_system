//
//  Node.cpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include "node.hpp"

Node::Node():x(0),y(0),parent(nullptr),G(0),H(0){}

Node::Node(int x, int y):x(x),y(y),parent(nullptr),G(0),H(0){}

Node::Node(int x, int y, Node *parent, int g, int h):x(x),y(y),parent(parent),G(g),H(h){}

bool Node::equals(Node node) const {
    return node.get_x()==x&&node.get_y()==y&&node.get_parent()==parent&&node.get_g()==G&&node.get_h()==H;
}

bool Node::equalxy(int x, int y) const {
    return x==this->x&&y==this->y;
}
