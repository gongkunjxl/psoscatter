/*
 * 粒子群算法测试 在x(-20,20) y(-30,30)中寻找离（10，20)最近的点
 */
#include <iostream>
#include "Swarm.h"
using namespace std;
int main() {
    bool findMax = false;   //寻找最小值

    //参数设置
    int particle_size = 20;
    int max_iter = 100;
    int serNum = 400;
    int machNum = 1000;
    GoalFunc *golfunction;

    std::cout<<"start"<<std::endl;
    Swarm swarm(particle_size,max_iter,golfunction,findMax,serNum,machNum);
    swarm.RunSwarm();
    return 0;
}