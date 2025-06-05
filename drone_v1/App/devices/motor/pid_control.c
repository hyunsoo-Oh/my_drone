///*
// * pid.c
// *
// *  Created on: May 14, 2025
// *      Author: USER
// */
//
//#include "Inc/pid_control.h"
//
//#define DT 0.001f
//#define EXTERNAL_DERIV_FILT_ENABLE 1
//#define INTERNAL_DERIV_FILT_ENABLE 1
//
////이중 제어를 통해 roll, pitch 각 제어
//void PIDDouble_Calc(PIDDouble* axis, float target_value, float current_value, float rate)
//{
//    //target_value : 조이스틱으로 통신받은 스로틀 값, current_value : ICM-20948 의 센서값, rate : ICM-20948에서의 각속도 값
//    //외부루프 제어(각도제어) 각도 입력 후 결과 값을 각속도제어기의 입력으로 넣음
//    axis->external_PID.reference=target_value;
//    axis->external_PID.measured_value=current_value;
//
//    //오차 값= 목표 값-현재 측정값
//    axis->external_PID.error=axis->external_PID.reference-axis->external_PID.measured_value;
//
//    //P제어 계산
//    axis->external_PID.p_result=axis->external_PID.error * axis->external_PID.Kp;
//
//    //=========================================================================//
//    //적분항 계산//
//    axis->external_PID.error_sum=axis->external_PID.error_sum+axis->external_PID.error*DT;
//	#define EXTERNAL_ERR_SUM_MAX 500
//	#define EXTERNAL_ERR_SUM_MIN -EXTERNAL_ERR_SUM_MAX
//    if(axis->external_PID.error_sum>EXTERNAL_ERR_SUM_MAX)
//    {
//      axis->external_PID.error_sum=EXTERNAL_ERR_SUM_MAX;
//    }
//    else if(axis->external_PID.error_sum<EXTERNAL_ERR_SUM_MIN)
//    {
//      axis->external_PID.error_sum=EXTERNAL_ERR_SUM_MIN;
//    }
//
//    //I제어 계산
//    axis->external_PID.i_result=axis->external_PID.error_sum * axis->external_PID.Ki;
//
//    //=========================================================================//
//    //미분항 계산
//    //D제어 오차는 해당 시점의 각속도 (각도의 변화율)
//    axis->external_PID.error_deriv=-rate;
//
//#if !EXTERNAL_PID_DERIV_FILT_ENABLE
//    //저역통과 필터 없이 D제어 계산
//    axis->external_PID.d_result=axis->external_PID.error_deriv * axis->external_PID.Kd;
//#else
//    //저역통과필터로 값 필터링
//    axis->external_PID.filtered_error_deriv=axis->external_PID.filtered_error_deriv*0.4f
//	                                   +axis->external_PID.error_deriv*0.6f;
//    axis->external_PID.d_result=axis->external_PID.filtered_error_deriv*axis->external_PID.Kd;
//#endif
//
//    //외부제어기 총 결과
//    axis->external_PID.pid_result=axis->external_PID.p_result + axis->external_PID.i_result+ axis->external_PID.d_result;
//
//
//    //=========================================================================//
//    //내부 제어기
//
//    axis->internal_PID.reference=axis->external_PID.pid_result;//내부제어기의 인력은 외부제어기의 결과
//    axis->internal_PID.measured_value=rate;
//    axis->internal_PID.error=axis->internal_PID.reference-axis->internal_PID.measured_value;
//
//
//    //내부 제어기 P제어 연산
//    axis->internal_PID.p_result=axis->internal_PID.error * axis->internal_PID.Kp;
//
//    //I 제어
//    //오차 누적 시키기
//    axis->internal_PID.error_sum=axis->internal_PID.error+axis->internal_PID.error*DT;
//#define INTERNAL_ERR_SUM_MAX 500
//#define INTERNAL_ERR_SUM_MIN -INTERNAL_ERR_SUM_MAX
//    if(axis->internal_PID.error_sum > INTERNAL_ERR_SUM_MAX)
//    {
//      axis->internal_PID.error_sum=INTERNAL_ERR_SUM_MAX;
//    }
//    else if(axis->internal_PID.error_sum < INTERNAL_ERR_SUM_MIN)
//    {
//      axis->internal_PID.error_sum=INTERNAL_ERR_SUM_MIN;
//    }
//    //I제어 계산
//    axis->internal_PID.i_result=axis->internal_PID.error_sum*axis->internal_PID.Ki;
//
//    //D 제어
//    //각속도 변화율 계산
//    axis->internal_PID.error_deriv=-(axis->internal_PID.measured_value-axis->internal_PID.prev_measured_value)/DT;
//    axis->internal_PID.prev_measured_value=axis->internal_PID.measured_value;
//
//#if !INTERNAL_DERIV_FILT_ENABLE
//    axis->internal_PID.d_result=axis->internal_PID.error_deriv*axis->internalPID.Kd;
//#else
//    axis->internal_PID.filtered_error_deriv=axis->internal_PID.filtered_error_deriv*0.5f+axis->internal_PID.error_deriv*0.5f;
//    axis->internal_PID.d_result=axis->internal_PID.filtered_error_deriv*axis->internal_PID.Kd;
//#endif
//
//    //최종계산
//    //PID roll pitch 관련 추력계산
//    axis->internal_PID.pid_result=axis->internal_PID.p_result+axis->internal_PID.i_result+axis->internal_PID.d_result;
//}
//
//
//
////스로틀 중립시 yaw각도 제어
//void PIDSingle_Calc_yaw_angle(PIDSingle* axis, float target_value, float current_value, float angle_velocity)//yaw, 목표 각도, 센서 측정 각도, 센서 측정 각속도
//{
//    axis->reference=target_value;
//    axis->measured_value=current_value;
//    axis->error=axis->reference-axis->measured_value;
//
//    if(axis->error>180.0f)
//    {
//      axis->error-=360.0f;
//    }
//    else if(axis->error<-180.0f)
//    {
//      axis->error+=360.f;
//    }
//    axis->p_result=axis->error*axis->Kp;
//
//    //오차 누적값 계산
//    axis->error_sum=axis->error_sum+axis->error*DT;
//    axis->d_result=axis->error_sum*axis->Ki;
//
//    //yaw의 D제어의 에러는 각속도 = 각도 변화율
//    axis->error_deriv=-angle_velocity;
//    axis->d_result=axis->error_deriv*axis->Kd;
//
//    axis->pid_result=axis->p_result + axis->i_result + axis->d_result;
//}
//
////스로틀 중립 아닐 시 yaw 각속도 제어(회전비율 제어)
//void PIDSingle_calc_yaw_angle_velocity(PIDSingle* axis, float target_value, float current_value)
//{
//   axis->reference=target_value;
//   axis->measured_value=current_value;
//
//   //P제어
//   axis->error=axis->reference-axis->measured_value;
//   axis->p_result=axis->error*axis->Kp;
//
//   //I제어
//   axis->error_sum=axis->error_sum+axis->error*DT;
//   axis->i_result=axis->error_sum*axis->Ki;
//
//   //D제어
//   axis->error_deriv=-(axis->measured_value-axis->prev_measured_value)/DT;
//   axis->prev_measured_value=axis->measured_value;
//   axis->d_result=axis->error_deriv*axis->Kd;
//
//   axis->pid_result=axis->p_result+axis->i_result+axis->d_result;
//
//}
//
////스로틀 중립일때 고도제어 추력
// void PIDSingle_altitude_Calc(PIDSingle* altitude, float target_value, float current_value)
//{
//   altitude->reference=target_value;
//   altitude->measured_value=current_value;
//
//   altitude->error=altitude->reference-altitude->measured_value;
//   //P제어
//   altitude->p_result=altitude->error*altitude->Kp;
//
//   //I제어
//   altitude->error_sum=altitude->error_sum+altitude->error*DT;
//   altitude->i_result=altitude->error_sum*altitude->Ki;
//
//   //D제어
//   altitude->error_deriv=-(altitude->measured_value-altitude->prev_measured_value)/DT;
//   altitude->prev_measured_value=altitude->measured_value;
//   altitude->d_result=altitude->error_deriv*altitude->Kd;
//
//   altitude->pid_result=altitude->p_result+altitude->i_result+altitude->d_result;
//}
// void Reset_PID_Integrator(PIDSingle* pid)
//{
//   pid->error_sum=0;
//}
//void Reset_All_PID_Integrator()
//{
//   Reset_PID_Integrator(&roll.internal_PID);
//   Reset_PID_Integrator(&roll.external_PID);
//   Reset_PID_Integrator(&pitch.internal_PID);
//   Reset_PID_Integrator(&pitch.internal_PID);
//   Reset_PID_Integrator(&yaw_angle_neutral);
//   Reset_PID_Integrator(&yaw_angle_velocity_non_neutral);
//   Reset_PID_Integrator(&altitude);
//}
//
