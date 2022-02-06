//
//  kmp.hpp
//  KMP
//
//  Created by Ice Bear on 2021/4/11.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#ifndef kmp_hpp
#define kmp_hpp

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class KMP {
public:
    KMP(string str, string pattern):str(str),pattern(pattern) {}
    int start();    //返回pattern在str中第一次出现的位置,-1代表没有
    void init_next();
    
private:
    string str;
    string pattern;
    vector<int> next;
};

#endif /* kmp_hpp */
