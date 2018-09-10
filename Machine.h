//
// Created by 龚坤 on 2018/6/12. 虚拟机信息结构表 对各种资源的需求
//

#ifndef PSO_MACHINE_H
#define PSO_MACHINE_H
class Machine{
public:
    double rCPU;
    double rMEM;
    double rDISK;
    double rBAND;
    Machine(double rCPU,double rMEM,double rDISK,double rBAND):rCPU(rCPU),rMEM(rMEM),rDISK(rDISK),rBAND(rBAND){}
};

#endif //PSO_MACHINE_H
