/**
  ******************************************************************************
  * @file    PID_Control.c
  * @author  willieon
  * @version V0.1
  * @date    January-2015
  * @brief   PID控制算法函数代码
  *        
  *
  ******************************************************************************
  **/

#include "PID_Control.h"
#include "math.h"

/*************************************************************************************
*        名    称： double PIDCalc( PID *pp, double NextPoint ,double SepLimit)
*        功    能： PID控制运算
*        入口参数： PID *pp  - 定义的运算所需变量的结构体
*                           NextPoint - 负反馈输入值
*                           SepLimit  - 积分分离上限
*        出口参数： 返回PID控制量
*        说    明： 默认不进行积分分离，如果用户需要使用积分分离，需在PID_Control.h中
*                                将 #define IF_THE_INTEGRAL_SEPARATION  0  改为 
*                            #define IF_THE_INTEGRAL_SEPARATION  1
*        调用方法： 进行积分分离时入口参数为3个，具体方法如下：
*                                PID PIDControlStruct ;   //定义PID运算结构体
*                                PIDInit(50, 0.24, 0.04, 0.2, &PIDControlStruct);//结构体初始化，注意&符号不能省
*                                ControlData = PIDCalc(ReadData, 200, &PIDControlStruct);   //控制量 = PIDCalc(反馈值,积分分离上限,PID运算结构体)
*
***************************************************************************************
*/

#if IF_THE_INTEGRAL_SEPARATION

double PIDCalc(double NextPoint ,double SepLimit, PID *pp)
{
        double dError, Error,Flag;   
        Error = pp->SetPoint - NextPoint;         // 偏差 
        if(abs(Error) > SepLimit)        //当偏差大于分离上限积分分离
        {
                Flag = 0;
        }
        else       //当偏差小于分离上限，积分项不分离
        {
                Flag = 1;
                pp->SumError += Error;         // 积分  
        }
        dError = pp->LastError - pp->PrevError;         // 当前微分 
        pp->PrevError = pp->LastError; 
        pp->LastError = Error;  
        return (
                pp->Proportion                *                Error                 // 比例项 
                + Flag * pp->Integral        *                pp->SumError         // 积分项 
                + pp->Derivative                *                dError                 // 微分项 
                );
}

#else

double PIDCalc( double NextPoint, PID *pp) 
{  
        double dError, Error;   
        Error = pp->SetPoint - NextPoint;                         // 偏差 
        pp->SumError += Error;                                        // 积分  
        dError = pp->LastError - pp->PrevError;                // 当前微分 
        pp->PrevError = pp->LastError; 
        pp->LastError = Error;  
        return (pp->Proportion        *        Error                // 比例项 
                + pp->Integral                *        pp->SumError         // 积分项 
                + pp->Derivative        *        dError         // 微分项 
                ); 
} 

#endif


/*************************************************************************************
*        名    称： double PIDCalc( PID *pp, double NextPoint ,double SepLimit)
*        功    能： PID初始化设定
*        入口参数： PID *pp  - 定义的运算所需变量的结构体
*                           SetPoint - 设定的目标值
*                           Proportion，Integral ，Derivative - P,I,D系数
*        出口参数： 无
*        说    明：        
*        调用方法：  PID PIDControlStruct ;   //定义PID运算结构体
*                                PIDInit(50, 0.24, 0.04, 0.2, &PIDControlStruct);//结构体初始化，注意&符号不能省
*                                因为函数需要传入一个指针，需要对结构体取首地址传给指针
*
***************************************************************************************
*/


void PIDInit (double SetPoint, double Proportion, double Integral, double Derivative, PID *pp)
{  
        pp -> SetPoint = SetPoint; // 设定目标 Desired Value   
        pp -> Proportion = Proportion; // 比例常数 Proportional Const 
        pp -> Integral = Integral; // 积分常数 Integral Const 
        pp -> Derivative = Derivative; // 微分常数 Derivative Const   
        pp -> LastError = 0; // Error[-1] 
        pp -> PrevError = 0; // Error[-2] 
        pp -> SumError = 0; // Sums of Errors 

        //memset ( pp,0,sizeof(struct PID));   //need include "string.h"
}
