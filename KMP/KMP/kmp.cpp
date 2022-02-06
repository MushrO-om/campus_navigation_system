//
//  kmp.cpp
//  KMP
//
//  Created by Ice Bear on 2021/4/11.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include "kmp.hpp"

int KMP::start() {
    init_next();
    int j = 0;
    for(int i = 0; i<str.size(); i++) {
        while(j!=0&&str[i]!=pattern[j]) j = next[j];
        if(str[i]==pattern[j]) j++;
        if(j==pattern.size())
            return i-(int)pattern.size()+1;
    }
    return -1;
}

void KMP::init_next() {
    next = vector<int>(pattern.size(),0);
    int j = 0;
    for(int i = 2; i<pattern.size(); i++) {
        while(j!=0&&pattern[j]!=pattern[i-1]) j = next[j];
        if(pattern[i-1]==pattern[j]) j++;
        next[i] = j;
    }
}
