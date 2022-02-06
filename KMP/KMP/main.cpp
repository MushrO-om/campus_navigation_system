//
//  main.cpp
//  KMP
//
//  Created by Ice Bear on 2021/4/11.
//  Copyright © 2021 Mushroom. All rights reserved.
//

#include <iostream>
#include <string>
#include "kmp.hpp"

int main(int argc, const char * argv[]) {
    string str = "我爱中国";
    string pat = "中国";
    KMP kmp(str, pat);
    cout<<kmp.start();
    return 0;
}
