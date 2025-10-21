#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "control.h"
#include "printf.h"
#include "hcsr04.h"
#include "engin.h"
#include "motor.h"

char moter_com = 0x00;
int32_t mod;   //模式

uint32_t turn_l = 0;
uint32_t turn_r = 0;

PID_Controller turn_pid = {0.5, 0.01, 0.1, 0, 0}; // 需要根据实际调整参数

float PID_Update(PID_Controller* pid, float error) {
    pid->integral += error;
    float derivative = error - pid->last_error;
    pid->last_error = error;
    return pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
}

void Motor_ctr_PID(int error) {
    float adjustment = PID_Update(&turn_pid, error);
    
    // 限制调整范围
    if(adjustment > 50) adjustment = 50;
    if(adjustment < -50) adjustment = -50;
    
    // 应用调整
    int right_speed = MOTOR_R - adjustment;
    int left_speed = MOTOR_L + adjustment;
    
    // 确保速度在合理范围内
    if(right_speed < 0) right_speed = 0;
    if(left_speed < 0) left_speed = 0;
    if(right_speed > 100) right_speed = 100;
    if(left_speed > 100) left_speed = 100;
    
    Motor_SetSpeedWithDirection(&motor_R, right_speed);
    Motor_SetSpeedWithDirection(&motor_L, left_speed);
    Motor_Start(&motor_R);
    Motor_Start(&motor_L);
}

void motor_mod(void)
{
	switch(mod)
	{
		case 1:
			Analysis_wires();	break;				//解析wires中的数据,并控制电机
		case 2:
			Analysis_hcsro4();  break;                //超声避障 ,并控制电机
	}
}


void Analysis_wires(void) {
    if(turn_l || turn_r) {
        if(turn_l) {
            Motor_ctr_TURNLEFT();
            turn_l--;
            return;
        }
        if(turn_r) {
            Motor_ctr_TURNRIGHT();
            turn_r--;
            return;
        }
    } else {
        if(wires & LINE_INIT) {
            if(wires & STRAIGHT_LINE) {
                printf("0\r\n");
                Motor_ctr_LINE();
                // 重置PID积分项
                turn_pid.integral = 0;
            }
            else if(wires & SLIGHTLY_LEFT) {
                printf("-1\r\n");
                Motor_ctr_PID(-10); // 轻微左偏
            }
            else if(wires & SLIGHTLY_RIGHT) {
                printf("1\r\n");
                Motor_ctr_PID(10); // 轻微右偏
            }
            else if(wires & TURN_LEFT) {
                printf("-2\r\n");
                turn_l = 20; // 减少固定转向时间
                Motor_ctr_TURNLEFT();
            }
            else if(wires & TURN_RIGHT) {
                printf("2\r\n");
                turn_r = 20; // 减少固定转向时间
                Motor_ctr_TURNRIGHT();
            }
        }
    }
}


void Analysis_hcsro4(void)                  //超声避障解析
{
	if(hcsr04_flag)
	{
		//wires &= ~LINE_INIT; 
		switch(hcsr04_flag)
		{
			case HCSR04_IS_LE:
				 printf("右转\r\n");
				break;
			case HCSR04_IS_RI:
				printf("左转\r\n");
				break;
			case HCSR04_IS:
				printf("左或右转\r\n");
				break;
			case HCSR04_IS_RI_LE:
				printf("停止\r\n");
				break;
		}

	}

}




