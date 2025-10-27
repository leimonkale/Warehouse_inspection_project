#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "control.h"
#include "printf.h"
#include "hcsr04.h"
#include "engin.h"
#include "motor.h"
#include "math.h"

#include <string.h>

extern int distance;       // ʼ��Ϊ��������ʾƫ����Ļ���ĵ����ؾ���
char moter_com = 0x00;
int32_t mod;   //ģʽ
char cmd_line;
char find_line[100] = "00202222022L";

uint32_t turn_l = 0;
uint32_t turn_r = 0;
uint32_t turn_z = 0;

//================== PID�������޷����� ==================
PID_Controller turn_pid = {0.15, 0.005, 0.55, 0, 0,0};  // PID���������ֳ����ԣ�

//#define STRAIGHT_THRESHOLD 20   // С�ڸ�ƫ����Ϊֱ��
#define MAX_ADJUST         50   // ����ٶ�������
#define BASE_L_SPEED         37   // �����ٶ�
#define BASE_R_SPEED         45 
#define INTEGRAL_LIMIT     30.0f


//================== PID���㺯�� ==================
float PID_Update(PID_Controller* pid, float error)
{
    pid->integral += error;
    if(pid->integral > INTEGRAL_LIMIT) pid->integral = INTEGRAL_LIMIT;
    if(pid->integral < -INTEGRAL_LIMIT) pid->integral = -INTEGRAL_LIMIT;

    float derivative = error - pid->last_error;
    pid->last_error = error;

    return pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
}

//================== PID���ƺ��� ==================
void Motor_ctr_PID_Distance(int error)
{
    // ����PID���
    float adjust = PID_Update(&turn_pid, error);

    // ���Ƶ�������
    if(adjust > MAX_ADJUST)  adjust = MAX_ADJUST;
    if(adjust < -MAX_ADJUST) adjust = -MAX_ADJUST;
	//printf("adjust: %.4f\r\n",adjust);
    // ���ݵ������ı��������ٶ�
    int left_speed  = BASE_L_SPEED + adjust;
    int right_speed = BASE_R_SPEED - adjust;

    // �����ٶȷ�Χ
    if(left_speed  > 100) left_speed = 100;
    if(right_speed > 100) right_speed = 100;
    if(left_speed  < 0)   left_speed = 30;
    if(right_speed < 0)   right_speed = 30;
	//printf("left_speed: %d\r\n",left_speed);
	//printf("right_speed: %d\r\n",right_speed);
    Motor_SetSpeedWithDirection(&motor_L, left_speed);
    Motor_SetSpeedWithDirection(&motor_R, right_speed);
    Motor_Start(&motor_L);
    Motor_Start(&motor_R);

}
void motor_mod(void)
{
	switch(mod)
	{
		case 1:
			Analysis_wires();	break;				//����wires�е�����,�����Ƶ��
		case 2:
			Analysis_hcsro4();  break;                //�������� ,�����Ƶ��
		case 3:
			Analysis_Find(); break;
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
               // printf("0\r\n");
				
                Motor_ctr_LINE();
                // ����PID������
                turn_pid.integral = 0;
				//printf("ֱ�� dist=%d\r\n", distance);
				Motor_SetSpeedWithDirection(&motor_L, BASE_L_SPEED);
				Motor_SetSpeedWithDirection(&motor_R, BASE_R_SPEED);
				Motor_Start(&motor_L);
				Motor_Start(&motor_R);
				
            }
            else if(wires & SLIGHTLY_LEFT) {
                //printf("-1\r\n");
                Motor_ctr_PID_Distance(-distance); // ��΢��ƫ
            }
            else if(wires & SLIGHTLY_RIGHT) {
                //printf("1\r\n");
                Motor_ctr_PID_Distance(distance); // ��΢��ƫ
            }
            else if(wires & TURN_LEFT) {
                //printf("-2\r\n");
                turn_l = 35; // ���ٹ̶�ת��ʱ��
                Motor_ctr_TURNLEFT();
            }
            else if(wires & TURN_RIGHT) {
                //printf("2\r\n");
                turn_r = 35; // ���ٹ̶�ת��ʱ��
                Motor_ctr_TURNRIGHT();
            }
        }
    }
}


void Analysis_hcsro4(void)                  //�������Ͻ���
{
	if(hcsr04_flag)
	{
		//wires &= ~LINE_INIT; 
		switch(hcsr04_flag)
		{
			case HCSR04_IS_LE:
				 //printf("��ת\r\n");
				break;
			case HCSR04_IS_RI:
				//printf("��ת\r\n");
				break;
			case HCSR04_IS:
				//printf("�����ת\r\n");
				break;
			case HCSR04_IS_RI_LE:
				//printf("ֹͣ\r\n");
				break;
		}

	}

}

void Find_Line(void)
{
	cmd_line = received_msg[0];
	if(cmd_line == 'L')
	{
		mod = 3;
		strcpy(find_line, received_msg+1);
		printf("find_line: %s\r\n",find_line);
	}
}

void Analysis_Find(void)
{
    static int i = 0;
    if(turn_l || turn_r || turn_z) {
        if(turn_l) {
            Motor_ctr_TURNLEFT();
            turn_l--;
            if(turn_l == 0) {
                i++;  // ֻ��ת����������
            }
            return;
        }
        if(turn_r) {
            Motor_ctr_TURNRIGHT();
            turn_r--;
            if(turn_r == 0) {
                i++;  // ֻ��ת����������
            }
            return;
        }
		if(turn_z) {
            Motor_ctr_LINE();
            turn_z--;
            if(turn_z == 0) {
                i++;  // ֻ��ת����������
            }
            return;
        }
    } else {
        if(wires & LINE_INIT) {
            if(wires & STRAIGHT_LINE) {
                Motor_ctr_LINE();
                turn_pid.integral = 0;
                Motor_SetSpeedWithDirection(&motor_L, BASE_L_SPEED);
                Motor_SetSpeedWithDirection(&motor_R, BASE_R_SPEED);
                Motor_Start(&motor_L);
                Motor_Start(&motor_R);
                printf("ֱ��\r\n");
            }else if(wires & SLIGHTLY_LEFT) {
                //printf("-1\r\n");
                Motor_ctr_PID_Distance(-distance); // ��΢��ƫ
            }
            else if(wires & SLIGHTLY_RIGHT) {
                //printf("1\r\n");
                Motor_ctr_PID_Distance(distance); // ��΢��ƫ
            } 
			
			else if((wires & TURN_LEFT) || (wires & TURN_RIGHT) || (wires & SLIGHTLY_TT)){
                if(find_line[i] == '1') {  // ��ת
                    turn_l = 35;
                    Motor_ctr_TURNLEFT();
                    printf("��\r\n");
                } else if(find_line[i] == '2') {  // ��ת
                    turn_r = 40;
                    Motor_ctr_TURNRIGHT();
                    printf("��\r\n");
                } else if(find_line[i] == '0') {  // ֱ��
					turn_z = 20;
                    Motor_ctr_LINE();
					printf("ֱ\r\n");
                } else if(find_line[i] == 'L') {
                    i = 0;  // ���������������� -1��
                } 
            }
        }
    }
    printf("i == %d\r\n", i);
}

