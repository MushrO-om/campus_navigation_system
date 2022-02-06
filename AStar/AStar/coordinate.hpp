//
//  Coordinate.hpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef coordinate_hpp
#define coordinate_hpp

#include <stdio.h>

class Coordinate {
public:
    Coordinate();
    Coordinate(int x, int y);
    
    int get_x() const {return x;}
    int get_y() const {return y;}
    
    void set_x(int x){this->x = x;}
    void set_y(int y){this->y = y;}

private:
    int x;
    int y;
};

#endif /* coordinate_hpp */
