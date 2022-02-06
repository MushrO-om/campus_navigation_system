//
//  main.cpp
//  蚁群算法
//
//  Created by Ice Bear on 2021/4/8.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include <vector>
#include <cmath>
#include "ant.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
//    const int n = 31;
//    vector<vector<double>> D;
//    double C[n][2] =        //各个城市的坐标数据
//    {    { 1304, 2312 },
//        { 3639, 1315 },
//        { 4177, 2244 },
//        { 3712, 1399 },
//        { 3488, 1535 },
//        { 3326, 1556 },
//        { 3238, 1229 },
//        { 4196, 1004 },
//        { 4312, 790 },
//        { 4386, 570 },
//        { 3007, 1970 },
//        { 2562, 1756 },
//        { 2788, 1491 },
//        { 2381, 1676 },
//        { 1332, 695 },
//        { 3715, 1678 },
//        { 3918, 2179 },
//        { 4061, 2370 },
//        { 3780, 2212 },
//        { 3676, 2578 },
//        { 4029, 2838 },
//        { 4263, 2931 },
//        { 3429, 1908 },
//        { 3507, 2367 },
//        { 3394, 2643 },
//        { 3439, 3201 },
//        { 2935, 3240 },
//        { 3140, 3550 },
//        { 2545, 2357 },
//        { 2778, 2826 },
//        { 2370, 2975 }
//    };
//    for (int i = 0; i < n; i++)            //初始化 D[n][n]
//    {
//        vector<double> temp;
//        for (int j = 0; j < n; j++)
//        {
//            //通过勾股定理计算城市距离
//            if (i != j)
//                temp.push_back(pow(pow((C[i][0] - C[j][0]), 2) + pow((C[i][1] - C[j][1]), 2), 0.5));
//            //自身到自身的距离设为浮点数double能表示的最小值
//            else
//                temp.push_back(numeric_limits<double>::epsilon());
//        }
//        D.push_back(temp);
//    }
//    vector<int> spots;
//    for(int i = 0; i<n;i++) spots.push_back(i);
//    Ant ant(D, spots);
    vector<vector<double>> adj_mat = {{0,30,400,20000,22000},{30,0,400,20030,22030},{400,400,0,20400,22400},{20000,20030,20400,0,2000},{22000,22030,22400,2000,0}};
    vector<int> spots = {0,1,2,3,4};
    Ant ant(adj_mat,spots);
    ant.start();
    return 0;
}
