//
//  grid_map.hpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef grid_map_hpp
#define grid_map_hpp

#include <stdio.h>
#include <vector>
#include "node.hpp"

using namespace std;

class Node;

class GridMap {
public:
    GridMap();
    ~GridMap();
    
    int get_value(int x, int y) const {return grid_map[y][x];}
    int get_value(Node node) const {return grid_map[node.get_y()][node.get_x()];}
    
    void set_value(int x, int y, int value){grid_map[y][x] = value;}
    void set_value(Node node, int value){grid_map[node.get_y()][node.get_x()] = value;}

    void show_map() const;
    
    bool inMap(int x, int y) const;
    
private:
    vector<vector<int>> grid_map;
};

#endif /* grid_map_hpp */
