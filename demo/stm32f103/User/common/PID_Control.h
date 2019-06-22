/**
  ******************************************************************************
  * @file    PID_Control.h
  * @author  willieon
  * @version V0.1
  * @date    January-2015
  * @brief   PID控制算法头文件
  *                        定义结构体类型以及声明函数
  *                        #define IF_THE_INTEGRAL_SEPARATION  0/1  为积分分离标志
  ******************************************************************************
  **/
 
#ifndef __PID_CONTROL_H__
#define __PID_CONTROL_H__
 
 
 
 
#define IF_THE_INTEGRAL_SEPARATION  0    
//#define IF_THE_INTEGRAL_SEPARATION  1   //是否积分分离  0-不分离，1 -分离
 
typedef struct 
{
        double SetPoint; // 设定目标 Desired Value   
        double Proportion; // 比例常数 Proportional Const 
        double Integral; // 积分常数 Integral Const 
        double Derivative; // 微分常数 Derivative Const   
        double LastError; // Error[-1] 
        double PrevError; // Error[-2] 
        double SumError; // Sums of Errors  
}PID;
 
#if IF_THE_INTEGRAL_SEPARATION            //是否积分分离预编译开始
 
double PIDCalc(double NextPoint ,double SepLimit, PID *pp);   //带积分分离的PID运算
 
#else
 
double PIDCalc( double NextPoint, PID *pp);     //不带积分分离的PID运算
 
#endif        //是否积分分离预编译结束
 
void PIDInit (double SetPoint, double Proportion, double Integral, double Derivative, PID *pp);
 
#endif
