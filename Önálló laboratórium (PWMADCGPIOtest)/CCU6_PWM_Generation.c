/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "CCU6_PWM_Generation.h"

#include "CCu6/PwmHl/IfxCcu6_PwmHl.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define CCU6_BASE_FREQUENCY	    150000000                                   /* CCU6 base frequency, in Hertz */
#define PWM_FREQUENCY          10000                                      /* PWM signal frequency, in Hertz */
#define PWM_PERIOD              (CCU6_BASE_FREQUENCY / PWM_FREQUENCY)       /* PWM signal period, in ticks  */

#define CHANNEL1_DUTY_CYCLE     0                                          /* PWM Signal  Duty cycle, in percent */

#define CHANNEL1_COMPARE_VALUE  ((PWM_PERIOD / 100) * (100 - CHANNEL1_DUTY_CYCLE))  /* PWM Signal  compare value */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxCcu6_TimerWithTrigger g_timer;
IfxCcu6_PwmHl g_driver;

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Function to initialize the CCU6 */
void initCCU6(void)
{
    boolean interruptState = IfxCpu_disableInterrupts();            /* Disable global interrupts */

    /* Timer configuration */
    IfxCcu6_TimerWithTrigger_Config timerConf;
    IfxCcu6_TimerWithTrigger_initConfig(&timerConf, &MODULE_CCU60);


    timerConf.base.frequency = PWM_FREQUENCY;                       /* Set the frequency for the PWM signal */
    timerConf.base.countDir = IfxStdIf_Timer_CountDir_upAndDown;    /* Configure the timer to count up and down */

    /* Initialize the timer driver */
    IfxCcu6_TimerWithTrigger_init(&g_timer, &timerConf);

    /* PWM High/Low driver configuration */
    IfxCcu6_PwmHl_Config pwmHlConf;
    IfxCcu6_PwmHl_initConfig(&pwmHlConf);                           /* Initialize the PwmHl configuration with */

    /* User PWM High/Low driver configuration */
    pwmHlConf.timer = &g_timer;                                     /* Use the Timer  */
    pwmHlConf.base.channelCount = 1;              					 /* Configure the driver to use one channel  */

    /* Output pins */
    pwmHlConf.cc0 = &IfxCcu60_CC60_P02_0_OUT;
    pwmHlConf.cout0 = &IfxCcu60_COUT60_P02_1_OUT;


    /* Initialize the PwmHl driver */
    IfxCcu6_PwmHl_init(&g_driver, &pwmHlConf);

    /* Generate center aligned PWM signals */
    IfxCcu6_PwmHl_setMode(&g_driver, Ifx_Pwm_Mode_centerAligned);

    /* Set the duty cycle */
    Ifx_TimerValue cmpValue;
    cmpValue = CHANNEL1_COMPARE_VALUE;                          /* Set the compare value */

    g_driver.update(&g_driver, &cmpValue);                          /* Apply the compare value */

    /* Update the timer */
    IfxCcu6_TimerWithTrigger_applyUpdate(g_driver.timer);

    /* Restore interrupts */
    IfxCpu_restoreInterrupts(interruptState);
}

/* Function to change the duty cycle */
void CCU6_Duty(int per)
{
	boolean interruptState = IfxCpu_disableInterrupts();            /* Disable global interrupts */
	float compv = (PWM_PERIOD / 100.0) * (100.0 - (float)per);		/* Calculate the compare value */
    Ifx_TimerValue cmpValue;
    cmpValue = compv;                         						 /* Set the compare value */
    g_driver.update(&g_driver, &cmpValue);                          /* Apply the compare value */
    IfxCcu6_TimerWithTrigger_applyUpdate(g_driver.timer);
    /* Restore interrupts */
    IfxCpu_restoreInterrupts(interruptState);
}

/* Function that starts the generation of the PWM signal */
void startPWMGeneration(void)
{
    IfxCcu6_TimerWithTrigger_run(&g_timer);
}


