#ifndef __ENGIN_H__
#define __ENGIN_H__

extern char engin_flag;
extern uint16_t angle;

void gpio_engin_init(void);
void tim1_init(void);
void servo_set_angle(void);

#endif
