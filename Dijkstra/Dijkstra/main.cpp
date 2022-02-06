//
//  main.cpp
//  Dijkstra
//
//  Created by Ice Bear on 2021/4/10.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include <iostream>
#include "node.hpp"
#include "dijkstra.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    int x;
    cout<<"请输入起点编号："<<endl;
    cin>>x;
    Node *begin = new Node(x);
    cout<<"请输入终点编号："<<endl;
    cin>>x;
    Node *end = new Node(x);
    double ma = numeric_limits<double>::max();
    vector<vector<double>> adj = {{0,12,ma,ma,ma,16,14},{12,0,10,ma,ma,7,ma},{ma,10,0,3,5,6,ma},{ma,ma,3,0,4,ma,ma},{ma,ma,5,4,0,2,8},{16,7,6,ma,2,0,9},{14,ma,ma,ma,8,9,0}};
    Dijkstra dij(adj,begin,end);
    dij.start();
    return 0;
}
