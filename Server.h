//
// Created by 龚坤 on 2018/6/12. 服务器的信息结构表
//

#ifndef PSO_SERVER_H
#define PSO_SERVER_H
class Server{
public:
    double pCPU;
    double pMEM;
    double pDISK;
    double pBAND;
    Server(double pCPU, double pMEM,double pDISK,double pBAND):pCPU(pCPU),pMEM(pMEM),pDISK(pDISK),pBAND(pBAND){}
};

#endif //PSO_SERVER_H
