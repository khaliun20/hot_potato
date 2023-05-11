#ifndef __POTATO_HPP__
#define __POTATO_HPP__

#include <iostream>
#include <ostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#define L_NUM 1024

class Hot_Potato { 

public:
int hops;
int trace[L_NUM];
int hops_done;
int done;

  Hot_Potato() : hops(0), hops_done(0), done(0) { 
    memset(trace, 0, sizeof(trace)); }


~Hot_Potato(){}

};





#endif