

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "GtmTomPwmGenerationHl.h"

#include <stdio.h>

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

App_GtmTomPwmHl g_GtmTomPwmHl; /**< \brief Demo information */

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/


/* Timer Interrupt Service Routine */
IFX_INTERRUPT(ISR_Timer, 0, ISR_PRIORITY_TIMER);

void ISR_Timer(void)
{
    IfxCpu_enableInterrupts();

    IfxGtm_Tom_Timer_acknowledgeTimerIrq(&g_GtmTomPwmHl.drivers.timer);
}


void GtmTomPwmHl_initTimer(void)
{
    {   /* GTM TOM configuration */
        IfxGtm_Tom_Timer_Config timerConfig;
        IfxGtm_Tom_PwmHl_Config pwmHlConfig;

        IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
        timerConfig.base.frequency                  = 10000;				                        //PWM freq
        timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency)/1000 ;
        timerConfig.base.trigger.enabled            = FALSE;
        timerConfig.tom                             = IfxGtm_Tom_1;
        timerConfig.timerChannel                    = IfxGtm_Tom_Ch_0;
        timerConfig.clock                           = IfxGtm_Cmu_Clk_0;

        timerConfig.triggerOut                      = &IfxGtm_TOM1_0_TOUT9_P00_0_OUT;
        timerConfig.base.trigger.outputEnabled      = TRUE;
        timerConfig.base.trigger.enabled            = TRUE;
        timerConfig.base.trigger.triggerPoint       = 500;
        timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

        IfxGtm_Tom_Timer_init(&g_GtmTomPwmHl.drivers.timer, &timerConfig);

        IfxGtm_Tom_PwmHl_initConfig(&pwmHlConfig);
        IfxGtm_Tom_ToutMapP ccx[1]   = {&IfxGtm_TOM1_1_TOUT10_P00_1_OUT};
        IfxGtm_Tom_ToutMapP coutx[1] = {&IfxGtm_TOM1_2_TOUT12_P00_3_OUT};
        pwmHlConfig.timer                 = &g_GtmTomPwmHl.drivers.timer;
        pwmHlConfig.tom                   = timerConfig.tom;
        pwmHlConfig.base.deadtime         = 2e-6;
        pwmHlConfig.base.minPulse         = 1e-6;
        pwmHlConfig.base.channelCount     = 1;
        pwmHlConfig.base.emergencyEnabled = FALSE;
        pwmHlConfig.base.outputMode       = IfxPort_OutputMode_pushPull;
        pwmHlConfig.base.outputDriver     = IfxPort_PadDriver_cmosAutomotiveSpeed1;
        pwmHlConfig.base.ccxActiveState   = Ifx_ActiveState_high;
        pwmHlConfig.base.coutxActiveState = Ifx_ActiveState_high;
        pwmHlConfig.ccx                   = ccx;
        pwmHlConfig.coutx                 = coutx;

        IfxGtm_Tom_PwmHl_init(&g_GtmTomPwmHl.drivers.pwm, &pwmHlConfig);

        IfxGtm_Tom_Timer_run(&g_GtmTomPwmHl.drivers.timer);

    }
}

/* Function to initialize the GtmTom */
void initGtmTomPwmHl(void)
{
    /* disable interrupts */
    boolean  interruptState = IfxCpu_disableInterrupts();

    /* GTM clocks */
    Ifx_GTM *gtm = &MODULE_GTM;
    g_GtmTomPwmHl.info.gtmFreq = IfxGtm_Cmu_getModuleFrequency(gtm);
    IfxGtm_enable(gtm);

    /* Set the global clock frequencies */
    IfxGtm_Cmu_setGclkFrequency(gtm, g_GtmTomPwmHl.info.gtmFreq);
    g_GtmTomPwmHl.info.gtmGclkFreq = IfxGtm_Cmu_getGclkFrequency(gtm);

    IfxGtm_Cmu_setClkFrequency(gtm, IfxGtm_Cmu_Clk_0, g_GtmTomPwmHl.info.gtmGclkFreq);
    g_GtmTomPwmHl.info.gtmCmuClk0Freq = IfxGtm_Cmu_getClkFrequency(gtm, IfxGtm_Cmu_Clk_0, TRUE);
    g_GtmTomPwmHl.info.state          = GtmTomPwmHl_State_init;
    g_GtmTomPwmHl.info.stateDeadline  = now();

    /*Initialize GTM */
    GtmTomPwmHl_initTimer();

    //printf("Gtm Tom PwmHl is initialised\n");

    IfxGtm_Tom_PwmHl *pwmHl = &g_GtmTomPwmHl.drivers.pwm;
    IfxGtm_Tom_Timer *timer = &g_GtmTomPwmHl.drivers.timer;
    Ifx_TimerValue    tOn[1];
    IfxGtm_Cmu_enableClocks(gtm, IFXGTM_CMU_CLKEN_FXCLK | IFXGTM_CMU_CLKEN_CLK0);
    IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);                    /* Set PWM to center aligned */
    tOn[0] = IfxGtm_Tom_Timer_getPeriod(timer) / 2.0 ;                              /* Set PWM duty cycle to 50% */
    //printf("%f\n",IfxGtm_Tom_Timer_getFrequency(timer));
    IfxGtm_Tom_Timer_disableUpdate(timer);
    IfxGtm_Tom_PwmHl_setOnTime(pwmHl, &tOn[0]);
    IfxGtm_Tom_Timer_applyUpdate(timer);
    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);
}


/* Function to change the PWM duty cycle */
void setDutyCycle(int percent)
{
    /* disable interrupts */
    boolean  interruptState = IfxCpu_disableInterrupts();

    /* GTM clocks */
    Ifx_GTM *gtm = &MODULE_GTM;


    IfxGtm_Tom_PwmHl *pwmHl = &g_GtmTomPwmHl.drivers.pwm;
    IfxGtm_Tom_Timer *timer = &g_GtmTomPwmHl.drivers.timer;
    Ifx_TimerValue    tOn[1];
    IfxGtm_Cmu_enableClocks(gtm, IFXGTM_CMU_CLKEN_FXCLK | IFXGTM_CMU_CLKEN_CLK0);
    IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);                            /* Set PWM to center aligned */
    tOn[0] = IfxGtm_Tom_Timer_getPeriod(timer) * ((float)percent/100.0);              /* Set PWM duty cycle */
    IfxGtm_Tom_Timer_disableUpdate(timer);
    IfxGtm_Tom_PwmHl_setOnTime(pwmHl, &tOn[0]);
    IfxGtm_Tom_Timer_applyUpdate(timer);
    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);
}

