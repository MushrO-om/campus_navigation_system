//
//  dijkstra.hpp
//  Dijkstra
//
//  Created by Ice Bear on 2021/4/10.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef dijkstra_hpp
#define dijkstra_hpp

#include <stdio.h>
#include <vector>

using namespace std;

class Node;

class Dijkstra {
public:
    Dijkstra(vector<vector<double>> adj, Node* begin, Node* end);
    ~Dijkstra();
    
    static bool cmp(Node *node1, Node *node2);
    
    void start();
    void print();
    
    void show_to_visit();
    void show_visited();
    
private:
    vector<vector<double>> adj_mat;
    vector<Node*> to_visit;
    vector<Node*> visited;
    Node *begin_node;
    Node *end_node;
    
};

#endif /* dijkstra_hpp */
