//
// Created by 龚坤 on 2018/6/8.
//
#include "Swarm.h"
//定义减法 0需要改变值
std::vector<int>Swarm::GetMinus(std::vector<int> &pos1, std::vector<int> &pos2) {
    std::vector<int> result;
    int size = pos1.size();
    for(int i=0;i<size;i++){
        if(pos1[i] == pos2[i]){
            result.push_back(1);
        }else{
            result.push_back(0);
        }
    }
    return result;
}
//定义加法(单个粒子 用于速度更新)
std::vector<int> Swarm::GetAdd(int par_index,std::vector<int> pb_vec, std::vector<int>gb_vec)
{
    Particle &par = partices[par_index];
    std::vector<int> result(0);
    double pv = 1.0/par.pValue;
    double pb = 1.0/par.pb_value;
    double pg = 1.0/gb_value;
    double p1 = pv/(pv+pb+pg);
    double p2 = (pv+pb)/(pv+pb+pg);
    int size = par.velocity.size();
//    std::cout<<"p1: "<<p1<<" p2: "<<p2<<" rand: "<<rnd<<std::endl;

    for(int i=0;i<size;i++){
        double rnd = ((double)rand())/RAND_MAX;
        if(rnd <= p1){
            result.push_back(par.velocity[i]);
        }else if(rnd >p1 && rnd<= p2){
            result.push_back(pb_vec[i]);
        }else{
            result.push_back(gb_vec[i]);
        }
    }
    return result;
}
//更新速度
void Swarm::UpdateVelocity(int par_index) {
    Particle &par = partices[par_index];
    std::vector<int> pb_vec = GetMinus(par.pb_postion,par.position);
    std::vector<int> gb_vec = GetMinus(gb_position,par.position);

    std::vector<int> result = GetAdd(par_index,pb_vec,gb_vec);
    for(int i=0;i<par.velocity.size();i++){
        partices[par_index].velocity[i] = result[i];
    }
}
//更新位置
void Swarm::UpdatePosition(int par_index) {
    Particle &par = partices[par_index];
    int size = serv.size();
    for(int j=0; j<size;j++){       //所有的物理机
        if(par.velocity[j]==0){     //位置需要调整 并且物理机开启 0的时候调整 相同是1
            double fc = GetResource(par.posInfo[j],j);
            double fb = GetResource(par.pbInfo[j],j);
            double fg = GetResource(gbInfo[j],j);
//            std::cout<<"fc: "<<fc<<" fb: "<<fb<<" fg: "<<fg<<std::endl;

            if((fc+fb+fg)==0){
                continue;
            }
            double p1 = fc/(fc+fb+fg);
            double p2 = (fc+fb)/(fc+fb+fg);
            double rnd = ((double)rand())/RAND_MAX;
//            std::cout<<"p1: "<<p1<<" p2: "<<p2<<" rand: "<<rnd<<std::endl;
            //确定更新策略
            if(rnd <= p1){
                continue;
            }else if(rnd>p1 && rnd<=p2){  //局部更新策略
                partices[par_index].position[j] = par.pb_postion[j];
                ReSchedule(par_index,par.pbInfo[j],j);
            }else{    //全局更新策略
                partices[par_index].position[j] = gb_position[j];
                ReSchedule(par_index,gbInfo[j],j);
            }
        }
    }
//    PrintPosition(par_index);
}
//获取局部最优值
double Swarm::GetResource(std::vector<int> posInfo,int j) {
    int size = posInfo.size();
    Server sum(0.0,0.0,0.0,0.0);
    for(int i=0;i<size;i++){
        sum.pCPU += mach[posInfo[i]].rCPU/serv[j].pCPU;
        sum.pMEM += mach[posInfo[i]].rCPU/serv[j].pMEM;
        sum.pDISK += mach[posInfo[i]].rDISK/serv[j].pDISK;
        sum.pBAND += mach[posInfo[i]].rBAND/serv[j].pBAND;
    }
    double result = (sum.pCPU+sum.pMEM+sum.pDISK+sum.pBAND)/4.0;
    return result;
}
//去掉重复的虚拟机
std::vector<int> Swarm::Duplicate(int par_index, int ind) {
    std::vector<int> sche = partices[par_index].posInfo[ind];
    int size = sche.size();     //物理机上的虚拟机
    int jSize = serv.size();    //物理机
    std::vector<int> result(0);

    for(int i=0;i<size;i++){
        for(int j=0;j<jSize;j++){
            if(j !=ind){  //新赋值的物理机不管
                int k;
                for(k=0;k<partices[par_index].posInfo[j].size();k++){  //对应的虚拟机
                    if(sche[i] == partices[par_index].posInfo[j][k]){
                        for(int mm=0;mm<partices[par_index].posInfo[j].size();mm++) {
                            if(sche[i] != partices[par_index].posInfo[j][mm]) {      //不包含重复 重调度虚拟机
                                result.push_back(partices[par_index].posInfo[j][mm]);
                            }
                        }
                        partices[par_index].posInfo[j].clear();     //清空
                        partices[par_index].position[j] = 0;        //关闭物理机
                    }
                }
            }
        }
    }
    return result;
}
//判断是否有重复 只需要判断新部署的物理机上的虚拟机 其他的已经去过重复
bool Swarm::IsDuplicate(int par_index, int machId, int ind) {
    Particle &par = partices[par_index];
    if (par.position[ind] == 1) {
        for (int k = 0; k < par.posInfo[ind].size(); k++) {  //对应的虚拟机
            if (machId ==par.posInfo[ind][k]) {
                return true;
            }
        }
    }
    return false;
}
//重新调度第j台物理机上的应用
void Swarm::ReSchedule(int par_index, std::vector<int> machInfo, int j) {
    //step1: 先赋值 j位置
    std::vector<int> sch;
    sch = partices[par_index].posInfo[j];
    partices[par_index].posInfo[j] = machInfo;

    //step2: 去掉重复 关闭物理机 需要重调度部分
    std::vector<int> result = Duplicate(par_index,j);
    for(int i=0;i<sch.size();i++) {
        result.push_back(sch[i]);
    }
    //step3: 重新调度
    int size = result.size();
//    std::cout<<getCount(par_index)<<"sch size: "<<sch.size()<<"  result size: "<<result.size()<<" "<<GetActive(par_index)<<std::endl;

    for(int i=0;i<size;i++){
        if(!IsDuplicate(par_index,result[i],j)){  //虚拟机在物理机j上不重复
            int k=0;
            for(k=0;k<serv.size();k++){ //物理机开启并且满足虚拟机的需求 调度该虚拟机
                if(partices[par_index].position[k]==1 && partices[par_index].IsSatisfy(serv,mach,k,result[i])){
                    partices[par_index].posInfo[k].push_back(result[i]);
                    break;
                }
            }
            if(k == serv.size()){     //开启新的物理机
                int m=0;
                for(m=0;m<serv.size();m++){
                    if(partices[par_index].position[m] ==0){
                        partices[par_index].position[m] = 1;
                        partices[par_index].posInfo[m].push_back(result[i]);   //部署虚拟机
                        break;
                    }
                }
                if(m==serv.size()){
                    std::cout<<"物理机数量不足"<<std::endl;
                }
            }
        }
    }
}
//比较函数 findMax决定最大还是最小
bool Swarm::CompareBetter(double v1, double v2) {
    if(findMax)
        return v1>v2;
    else
        return v1<v2;
}

//计算粒子的适应值
void Swarm::EvalueFunc(int par_index) {
    Particle &par = partices[par_index];
    par.pValue = (*goalfunc)(par.posInfo,par.position,serv,mach);
//    std::cout<<"par index: "<<par_index<<" parMach: "<<getCount(par_index)<<" active: "<<GetActive(par_index)<<" pValue: "<<par.pValue<<" pb_value: "<<par.pb_value<<" gb_value: "<<gb_value<<std::endl;

//局部最优和全局最优值
    if(CompareBetter(par.pValue,par.pb_value)){
        partices[par_index].pb_value = par.pValue;
        partices[par_index].pb_postion = par.position;
        partices[par_index].pbInfo = par.posInfo;       //位置 值 速度 服务器上虚拟机的信息
        partices[par_index].pb_velocity = par.velocity;
    }
    if(CompareBetter(par.pValue,gb_value)){
        std::cout<<par_index<<"  "<<curr_iter<<" ";
        gb_value = par.pValue;
        gb_position = par.position;
        gbInfo = par.posInfo;
        gb_velocity = par.velocity;
        PrintPos();
    }
}

//构造函数
Swarm::Swarm(int particle_size, int max_iter, GoalFunc *golfuction, bool findMax, int serNum, int machNum) {
    this->particle_size = particle_size;
    this->curr_iter = 0;  //从0开始
    this->max_iter = max_iter;
    this->findMax = findMax;  //确定求取最大值还是最小值

    //初始化服务器和虚拟机 暂时使用同的方式
    Server serv_S(3200.0,16000.0,1024.0,1000.0);
    Machine mach_S[4]={{640.0,800.0,30.0,40.0},{320.0,4096.0,50,30.0},{200.0,1024.0,200,20.0},{300.0,600.0,40.0,200.0}};
    for(int i=0;i<serNum;i++){
        this->serv.push_back(serv_S);
    }
    //虚拟机
//    srand((unsigned)time(NULL));
    for(int i=0;i<machNum;i++){
        Machine tmp_mach{0.0,0.0,0.0,0.0};
        tmp_mach.rCPU = (((double)rand())/RAND_MAX)*320.0+300.0;
        tmp_mach.rMEM = (((double)rand())/RAND_MAX)*1024.0+2000.0;
        tmp_mach.rDISK = (((double)rand())/RAND_MAX)*100.0+60.0;
        tmp_mach.rBAND = (((double)rand())/RAND_MAX)*100.0+100.0;

        this->mach.push_back(tmp_mach);
    }

//    std::cout<<"size: "<<particle_size<<" iter_max: "<<this->max_iter<<" serNum: "<<serNum<<" machNum: "<<this->mach.size()<<std::endl;
    //粒子群
    Particle par(this->serv,this->mach);  //构造单个粒子群 更新全局值
    this->gb_position = par.position;
    this->gbInfo = par.posInfo;
    this->gb_velocity = par.velocity;
    this->gb_value = (*golfuction)(par.posInfo,par.position,this->serv,this->mach);  //最优值

    par.pb_value = this->gb_value;
    par.pValue = this->gb_value;
    this->partices.push_back(par);

    for(int i=1;i<particle_size;i++){
        Particle par(this->serv,this->mach);
        par.pValue = (*golfuction)(par.posInfo,par.position,this->serv,this->mach);
        par.pb_value = par.pValue;
        this->partices.push_back(par);
        EvalueFunc(i);
//        PrintInfo(i);
//        PrintPosition(i);
    }
}
//粒子群算法运行 暂时不考虑参数可变的情况 迭代轮数和每一个粒子
void Swarm::RunSwarm() {
    int size = partices.size();
    for(int curr_iter=0;curr_iter<max_iter;curr_iter++){
        this->curr_iter = curr_iter;
        for(int par_index=0;par_index<size;par_index++){
            UpdateVelocity(par_index);
            UpdatePosition(par_index);
            EvalueFunc(par_index);
//            if(par_index ==2) PrintInfo(par_index);
        }
    }
}
//打印位置信息
void Swarm::PrintPosition(int par_index) {
    Particle &par = partices[par_index];
    std::cout<<par_index<<"  Par index position: "<<std::endl;
    for(int i=0;i<par.position.size();i++){
        std::cout<<par.position[i]<<" ";
    }
    std::cout<<std::endl;
    for(int i=0;i<par.pb_postion.size();i++){
        std::cout<<par.pb_postion[i]<<" ";
    }
    std::cout<<std::endl;
    for(int i=0;i<gb_position.size();i++){
        std::cout<<gb_position[i]<<" ";
    }
    std::cout<<std::endl;
}
//打印速度信息
void Swarm::PrintVelocity(int par_index) {
    Particle &par = partices[par_index];
    std::cout<<par_index<<"  Par index velocity: ";
    for(int i=0;i<par.velocity.size();i++){
        std::cout<<par.velocity[i]<<" ";
    }
    std::cout<<std::endl;
}
//dayin
void Swarm::PrintPos() {
    std::cout<<"postion:  ";
    for(int i=0;i<gb_position.size();i++){
        std::cout<<gb_position[i]<<" ";
    }
    std::cout<<std::endl;
    std::cout<<gb_value<<" "<<std::endl;
}
//统计粒子的虚拟机数量
int Swarm::getCount(int par_index) {
    Particle &par = partices[par_index];
    int size = serv.size();
    int sum = 0;
    for(int i=0;i<size;i++){
        if(par.position[i] == 1){   //物理机开启
            sum += par.posInfo[i].size();
        }
    }
    return sum;
}
//打印虚拟机的部署情况
void Swarm::PrintInfo(int par_index) {
    Particle &par = partices[par_index];
    int sum = 0;
    std::cout<<"par_index: "<<par_index<<"  ";
    for(int i=0;i<serv.size();i++){
        if(par.position[i] == 1){   //物理机开启
            int size = par.posInfo[i].size();
            std::cout<<i<<"{";
            for(int j=0;j<size;j++){
                std::cout<<par.posInfo[i][j]<<",";
            }
            std::cout<<"} ";
        }
    }
    std::cout<<std::endl;
}
int Swarm::GetActive(int par_index) {
    Particle &par = partices[par_index];
    int size = serv.size();
    int sum = 0;
    for(int i=0;i<size;i++){
        if(par.position[i] == 1 && par.posInfo[i].size()>0){
            sum++;
        }
    }
    return sum;
}


