//
//  astar.hpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef astar_hpp
#define astar_hpp

#include <stdio.h>
#include <queue>

#include "grid_map.hpp"
#include "node.hpp"

using namespace std;

class Node;
class GridMap;

class AStar {
public:
    AStar(GridMap gridmap);
    ~AStar();
    
    int calH(Node *node);
    int calH(int x, int y);

    void start();
    void move();
    void drawPath();
    bool inClose(Node *node) const;
    bool inClose(int x, int y) const;
    bool canOpen(int x, int y) const;
    void addNeighbor(Node *node);
    void addOpen(Node *current, int x, int y, int value);
    Node* findOpen(int x, int y);
    bool inOpen(int x, int y);
    
private:
    static int BAR;
    static int PATH;
    static int DIRECT_VALUE;
    static int DIAGONAL_VALUE;
    
    vector<Node*> openList;
    vector<Node*> closeList;
    
    Node *begin_node;
    Node *end_node;
    
    GridMap gridmap;
};

#endif /* astar_hpp */
