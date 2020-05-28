/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Timer_interrupt.h"
#include "Ifx_Types.h"
#include "IfxGpt12.h"
#include "IfxPort.h"
#include "VadcBackgroundScan.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

#define RELOAD_VALUE                48828u                  /* Reload value to have an interrupt each 500ms         */
#define LED                         &MODULE_P23,3           /* LED to blink in the Interrupt Service Routine   */

/*********************************************************************************************************************/
/*--------------------------------------------Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/
/* Interrupt Service Routine */
IFX_INTERRUPT(interruptGpt12, 0, ISR_PRIORITY_GPT12_TIMER);

/* Function to initialize the GPT12 */
void initGpt12Timer(void)
{
    /* Initialize the GPT12 module */
    IfxGpt12_enableModule(&MODULE_GPT120);                                          /* Enable the GPT12 */
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_16); /* Set prescaler */

    /* Initialize Timer T3 */
    IfxGpt12_T3_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);                       /* Set T3 to timer mode */
    IfxGpt12_T3_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);    /* Set T3 count direction */
    IfxGpt12_T3_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_64); /* Set T3 prescaler */
    IfxGpt12_T3_setTimerValue(&MODULE_GPT120, RELOAD_VALUE);                        /* Set T3 start value */

    /* Initialize the Timer T2 */
    IfxGpt12_T2_setMode(&MODULE_GPT120, IfxGpt12_Mode_reload);                      /* Set T2 to reload mode */
    IfxGpt12_T2_setReloadInputMode(&MODULE_GPT120, IfxGpt12_ReloadInputMode_bothEdgesTxOTL); /* Set reload trigger  */
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, RELOAD_VALUE);                        /* Set T2 reload value */

    /* Initialize the interrupt */
    volatile Ifx_SRC_SRCR *src = IfxGpt12_T3_getSrc(&MODULE_GPT120);                /* Get the interrupt address */
    IfxSrc_enable(src);                                                             /* Enable GPT12 interrupt */

    /* Initialize the LED */
    IfxPort_setPinModeOutput(LED, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    IfxGpt12_T3_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);                       /* Start the timer*/
}

/* Interrupt Service Routine of the GPT12 */
void interruptGpt12(void)
{
    IfxPort_togglePin(LED);                                                         /* Toggle LED state*/
    runVadcBackgroundScan();                                                        /* Run ADC background scan */
}


