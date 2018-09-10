//
// Created by 龚坤 on 2018/6/7.
//粒子群的类 主要定义粒子群的各种方法
//
#ifndef PSO_SWARM_H
#define PSO_SWARM_H
#include <vector>
#include "Particle.h"
#include "GoalFunc.h"
#include "Server.h"
#include "Machine.h"
#include <iostream>
class Swarm
{
public:
    int max_iter; //最大的迭代次数和当前迭代k
    int curr_iter;
    bool findMax;           //优化最大值还是最小值 false是最小值
    GoalFunc *goalfunc;     //目标函数

    std::vector<std::vector<int>> gbInfo;    //全局最优信息
    std::vector<int> gb_position;
    std::vector<int> gb_velocity;
    double gb_value;

    std::vector<Server> serv;   //服务器和虚拟机
    std::vector<Machine> mach;

    //离散粒子群void
    std::vector<int>GetMinus(std::vector<int> &pos1, std::vector<int> &pos2);  //减法
    std::vector<int> GetAdd(int par_index,std::vector<int> pb_vec, std::vector<int>gb_vec);   //加法

    double GetResource(std::vector<int> posInfo, int j);  //获取某台物理机的资源使用情况 局部适应值最优 传入放置信息
    void UpdateVelocity(int par_index);     //更新速度
    void UpdatePosition(int par_index);     //更新位置
    std::vector<int> Duplicate(int par_index,int ind);      //第几个物理机上的虚拟机
    bool IsDuplicate(int par_index,int machId, int ind);    //判断单个虚拟机是否重复
    void ReSchedule(int par_index,std::vector<int>machInfo, int j);      //重新调度第j个物理机上的应用
    void EvalueFunc(int par_index);     //评价函数的适应值

    int getCount(int par_index);        //统计粒子上虚拟机是够满足
    void PrintPosition(int par_index);  //输出位置信息
    void PrintVelocity(int par_index);  //
    int GetActive(int par_index);
    void PrintInfo(int par_index);  //打印粒子上虚拟机的部署情况

    //粒子群方法
    bool CompareBetter(double v1, double v2);   //比较大小
    void RunSwarm();    //粒子群算法运行q
    void PrintPos();
    //构造函数
    Swarm(int particle_size, int max_iter, GoalFunc *golfuction, bool findMax, int serNum, int machNum);

public:
    int particle_size;      //粒子群大小
    std::vector<Particle> partices; //粒子群
};


#endif //PSO_SWARM_H


















