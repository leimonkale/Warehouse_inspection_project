#ifndef __HCSR04_H__
#define __HCSR04_H__

extern HCSR04_HandleTypeDef hcsr04;
extern float hcsr04_distance;
extern char hcsr04_flag;
// º¯ÊýÉùÃ÷
void Hcsr04_Init(void);
void HCSR04_Init(HCSR04_HandleTypeDef* hcsr04);
uint8_t HCSR04_Measure(HCSR04_HandleTypeDef* hcsr04);
void HCSR04_GetDistance(HCSR04_HandleTypeDef* hcsr04);
void HCSR04_GetFlag(void);

#endif
