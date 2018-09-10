//
// Created by 龚坤 on 2018/6/7.
//单个粒子需要保存的信息
//
#ifndef PSO_PARTICLE_H
#define PSO_PARTICLE_H
#include <vector>
#include "Server.h"
#include "Machine.h"
#include <iostream>
class Particle{
public:
    std::vector<int> velocity;  //速度 用于确定物理机是否调整
    std::vector<int> position;  //位置 索引号就是ID号
    std::vector<std::vector<int>> posInfo;  //物理机的编号里面存储的虚拟机串 粒子信息 二维数组
    double pValue;              //当前的适应值

    std::vector<int> pb_velocity;    //局部最佳速度
    std::vector<int> pb_postion;     //局部最优位置
    std::vector<std::vector<int>> pbInfo;   //局部最优的虚拟机放置策略
    double pb_value;                //局部最佳值 在粒子群中位置 所有的都初始化后获得

    /*
     * 生成策略：假设所有的物理机都是随机启动的
     * 每次从满足需求的物理机上随机选择一个运行虚拟机
     * step1: select physical server
     * step2: random placement
     */
    Particle (std::vector<Server> serv,std::vector<Machine> mach){
        int m = serv.size();
        int n = mach.size();

        for(int i=0;i<m;i++){  //position and velocity  初始化
            int vd = rand()%2;
            int nd = rand()%2;
            std::vector<int> tmp(0);
            position.push_back(nd);
            velocity.push_back(vd);
            posInfo.push_back(tmp);

            pb_postion.push_back(nd);
            pb_velocity.push_back(vd);
            pbInfo.push_back(tmp);
        }
        for(int j=0;j<n;j++){
            std::vector<int>sat(0);    //满足条件的物理机
            for(int i=0;i<m;i++){
                if(position[i] == 1 && IsSatisfy(serv,mach,i,j)){
                    sat.push_back(i);
                }
            }
            if(sat.size() == 0){  //没有找到满足条件的物理机
                for(int i=0;i<m;i++){
                    if(position[i] == 0){  //开启新的物理机 放置虚拟机 退出
                        position[i] = 1;
                        posInfo[i].push_back(j);
                        pbInfo[i].push_back(j);
                        break;
                    }
                }
            }else {
                int mm = rand() % sat.size();  //决定放在哪个server上 所有虚拟机完成
                posInfo[sat[mm]].push_back(j);
                pbInfo[sat[mm]].push_back(j);
            }
        }
        pb_value = 0.0;
        pValue = 0.0;
    }
    //判断是否满足资源需求 i物理机编号 j虚拟机编号
    bool IsSatisfy(std::vector<Server> serv,std::vector<Machine> mach,int i,int j){
        Server sum(0.0,0.0,0.0,0.0);
        float beta = 0.85;      //资源限制参数
        int size = posInfo[i].size();
        for(int k=0;k<size;k++){
            sum.pCPU += mach[posInfo[i][k]].rCPU;
            sum.pMEM += mach[posInfo[i][k]].rMEM;
            sum.pDISK += mach[posInfo[i][k]].rDISK;
            sum.pBAND += mach[posInfo[i][k]].rBAND;
        }
        sum.pCPU += mach[j].rCPU;
        sum.pMEM += mach[j].rMEM;
        sum.pDISK += mach[j].rDISK;
        sum.pBAND += mach[j].rBAND;
        return ((serv[i].pCPU>sum.pCPU*beta) && (serv[i].pMEM>sum.pMEM*beta) && (serv[i].pDISK>sum.pDISK*beta) && (serv[i].pBAND>sum.pBAND*beta));
    }
};

#endif //PSO_PARTICLE_H
