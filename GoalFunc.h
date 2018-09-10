//
// Created by 龚坤 on 2018/6/7.
// 任何不同的问题只需要更改目标函数
//
#ifndef PSO_GOALFUNC_H
#define PSO_GOALFUNC_H
#include <vector>
#include <math.h>
#include <iostream>
#include "Server.h"
#include "Machine.h"
class GoalFunc
{
    public:
        GoalFunc(){}
        //粒子 粒子位置(服务器开启状态) 服务器 负载均衡的值作为衡量标准
        double  operator() (std::vector<std::vector<int>> &posInfo,std::vector<int> &pos,std::vector<Server> &serv,std::vector<Machine> &mach){
            int m = pos.size();
            double rubd_c=0.0;
            for(int i=0;i<m;i++){
                Server ud(0.0,0.0,0.0,0.0);
                double avg = 0.0,ruv=0.0,rb=0.0,rubd=0.0;
                int size = posInfo[i].size();
                if(pos[i] == 1 && size > 0) {  //服务器是开启的 且至少有一个虚拟机在上面运行
                    for (int j = 0; j < size; j++) {
                        ud.pCPU += mach[posInfo[i][j]].rCPU;
                        ud.pMEM += mach[posInfo[i][j]].rMEM;
                        ud.pDISK += mach[posInfo[i][j]].rDISK;
                        ud.pBAND += mach[posInfo[i][j]].rBAND;
                    }
                    //计算UD和Uavg d维资源的利用率和平均利用率
                    ud.pCPU = ud.pCPU/serv[i].pCPU;
                    ud.pMEM = ud.pMEM/serv[i].pMEM;
                    ud.pDISK = ud.pDISK/serv[i].pDISK;
                    ud.pBAND = ud.pBAND/serv[i].pBAND;
                    avg = (ud.pCPU+ud.pMEM+ud.pDISK+ud.pBAND)/4.0;
                    ruv = sqrt((pow(ud.pCPU-avg,2.0)+pow(ud.pMEM-avg,2.0)+pow(ud.pDISK-avg,2.0)+pow(ud.pBAND-avg,2.0))/4.0);
                    //get the max ud
                    double max = ud.pCPU;
                    if(ud.pMEM > max)   max = ud.pMEM;
                    if(ud.pDISK > max)  max = ud.pDISK;
                    if(ud.pBAND > max)  max = ud.pBAND;
                    //get RB
                    rb = avg / max;
                    rubd = ruv/rb;
                    rubd_c += rubd;
                }
            }
            return rubd_c;
        }
};

#endif //PSO_GOALFUNC_H








