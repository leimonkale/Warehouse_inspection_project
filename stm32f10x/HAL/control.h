#ifndef __CONTROL_H__
#define __CONTROL_H__

extern char moter_com;
extern int32_t mod;
extern char cmd_line;
extern char find_line[100];


void Analysis_wires(void);
void Analysis_hcsro4(void);
void Analysis_Find(void);
void Find_Line(void);
void motor_mod(void);

#endif
