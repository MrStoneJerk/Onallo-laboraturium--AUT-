#ifndef CCU6_PWM_GENERATION_H_
#define CCU6_PWM_GENERATION_H_

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

void initCCU6(void);
void startPWMGeneration(void);
void CCU6_Duty(int per);

#endif /* CCU6_PWM_GENERATION_H_ */
