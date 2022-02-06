//
//  main.cpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>

#include "coordinate.hpp"
#include "node.hpp"
#include "grid_map.hpp"
#include "astar.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    GridMap gridmap;
    AStar astar(gridmap);
    astar.start();
    return 0;
}
