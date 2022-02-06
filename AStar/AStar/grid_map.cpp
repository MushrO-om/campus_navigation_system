//
//  grid_map.cpp
//  AStar
//
//  Created by Ice Bear on 2021/4/7.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include "grid_map.hpp"
#include <fstream>
#include <iostream>

GridMap::GridMap() {
    cout<<"正在载入地图"<<endl;
    ifstream fin;
    fin.open("/Users/tom/gridmap.dic",ios::out);
    if(!fin.is_open()) {
        cout<<"打开文件失败"<<endl;
        exit(1);
    }
    int x;
    char ch;
    while (fin.peek()!=EOF) {
        fin>>x;
        ch = fin.get();
        vector<int> temp;
        while(fin.peek()!=EOF&&ch!='\n') {
            temp.push_back(x);
            fin>>x;
            ch = fin.get();
        }
        temp.push_back(x);
        grid_map.push_back(temp);
    }
    fin.close();
    cout<<"从文件中读取地图完成"<<endl;
}


GridMap::~GridMap() {
//    ofstream fout;
//    fout.open("/Users/tom/gridmap.dic",ios::out);
//    if(!fout.is_open()) {
//        cout<<"打开文件失败"<<endl;
//        exit(1);
//    }
//    for(int i = 0;i<grid_map.size();i++) {
//        for(int j = 0; j<grid_map[i].size(); j++) {
//            fout<<grid_map[i][j];
//            if(i!=grid_map.size()-1||j!=grid_map[i].size()-1) {
//                if(j!=grid_map[i].size()-1) fout<<" ";
//                else fout<<endl;
//            }
//        }
//    }
//    fout.close();
//    cout<<"地图存入文件成功"<<endl;
}


void GridMap::show_map() const {
    cout<<"网格地图如下:"<<endl;
    for(int i = 0;i<grid_map.size();i++) {
        for(int j = 0; j<grid_map[i].size(); j++) {
            cout<<grid_map[i][j];
            if(j!=grid_map[i].size()-1) cout<<" ";
            else cout<<endl;
        }
    }
}

bool GridMap::inMap(int x, int y) const {
    if(x>=grid_map[0].size()||x<0||y>=grid_map.size()||y<0) return false;
    return true;
}
