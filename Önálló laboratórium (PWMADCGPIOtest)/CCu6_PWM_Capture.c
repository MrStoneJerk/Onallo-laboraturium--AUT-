/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <stdio.h>
#include "CCu6_PWM_Capture.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

#define CCU6_INPUT_PIN                  IfxCcu60_CC60INA_P02_0_IN               /* Select input port pin P02.0 */
#define MAX_VAL_T12                     (0xFFFF - 1)                            /* Maximum number of ticks between overflows */

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IfxCcu6_Icu_Channel g_icuChannel;           /* CCU6 ICU channel handle */
float32 g_CCU6Frequency_Hz = 0.0;           /* CCU6 capture frequency */


float32 g_measuredPwmFreq_Hz = 0.0;         /* Calculated frequency */
float32 g_measuredPwmDutyCycle = 0.0;       /* Calculated duty cycle */
uint32  g_cntOverflowPeriod = 0;            /* Counter of timer overflows between rising edges */
uint32  g_cntOverflowHighLevel = 0;         /* Counter of timer overflows between a rising and a falling edge */
uint32  g_previousCntVal = 0;               /* Timer value of the previous interrupt */


/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

/* CCU6 Interrupt Service Routine */
IFX_INTERRUPT(CCU6_edge_detection_isr, 0, ISR_PRIORITY_CCU6_RISING_EDGE);

/* CCU6 Interrupt Service Routine */
void CCU6_edge_detection_isr(void)
{
    uint32  risingEdgeCntVal = 0;           /* Number of ticks between the last overflow and the rising edge */
    uint32  fallingEdgeCntVal = 0;          /* Number of ticks between the last overflow and the falling edge */

    /* Check the rising edge */
    if (g_icuChannel.ccu6->IS.B.ICC60R)
    {
        /* Reset interrupt status flag */
        IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_cc60RisingEdge);

        /* Rising edge counter */
        risingEdgeCntVal = IfxCcu6_getCaptureRegisterValue(g_icuChannel.ccu6, g_icuChannel.channelId);
    }

    /* Check the falling edge */
    if (g_icuChannel.ccu6->IS.B.ICC60F)
    {
        /* Reset interrupt status flag */
        IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_cc60FallingEdge);

        /* Falling edge counter */
        fallingEdgeCntVal = IfxCcu6_getCaptureShadowRegisterValue(g_icuChannel.ccu6, g_icuChannel.channelId);
    }

    /* Final timer counter value */
    uint32 finalFreqCntVal, finalDutyCycCntVal = 0;

    /* No overflow */
    if(g_cntOverflowPeriod == 0)
    {
        finalFreqCntVal = risingEdgeCntVal - g_previousCntVal;      /* Total counter value calculation          */
    }
    /* Overflow */
    else
    {
        /* Add to the current counter value, the amount of counter ticks which passed before the first overflow,
         * plus 65534 (0xFFFF - 1) for each additional overflow occurred between two rising edges.
         */
        finalFreqCntVal = (uint32)(risingEdgeCntVal + (MAX_VAL_T12 - g_previousCntVal) + ((g_cntOverflowPeriod - 1) * MAX_VAL_T12));
    }

    /* No overflows during signal high level */
    if(g_cntOverflowHighLevel == 0)
    {
        finalDutyCycCntVal = fallingEdgeCntVal - g_previousCntVal;
    }

    /* Overflows during signal high level */
    else
    {
        /* Add to the current counter value, the amount of counter ticks which passed before the first overflow,
         * plus 65534 (0xFFFF - 1) for each additional overflow occurred between the rising and the falling edge.
         */
        finalDutyCycCntVal = (uint32)(fallingEdgeCntVal + (MAX_VAL_T12 - g_previousCntVal) + ((g_cntOverflowHighLevel - 1) * MAX_VAL_T12));
    }

    g_measuredPwmFreq_Hz = g_CCU6Frequency_Hz / (float32)finalFreqCntVal;

    g_measuredPwmDutyCycle = 100 * (finalDutyCycCntVal / (float32)finalFreqCntVal);

    g_previousCntVal = risingEdgeCntVal;    /* Update g_previousCntVal */
    g_cntOverflowPeriod = 0;                /* Reset overflow counter of the period measurement */
    g_cntOverflowHighLevel = 0;             /* Reset overflow counter of the signal high level time measurement */
}

/* CCU6 period match  Interrupt Service Routine */
IFX_INTERRUPT(CCU6_period_match_isr, 0, ISR_PRIORITY_CCU6_PERIOD_MATCH);

/* Interrupt Service Routine of the CCU6 */
void CCU6_period_match_isr(void)
{
    g_cntOverflowPeriod++;

    /* Increments high level time measurement overflows number if the signal is still high*/
    if(IfxCcu6_getInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_cc60FallingEdge) == FALSE)
    {
        g_cntOverflowHighLevel++;
    }
}

/* Configure the CCU6 for capturing the rising edge of a PWM signal */
void init_CCU6_ICU(void)
{
    IfxCcu6_Icu ccu6Handle;                                             /* CCU6 handle */
    IfxCcu6_Icu_Config ccu6Config;                                      /* Configuration structure */

    IfxCcu6_Icu_initModuleConfig(&ccu6Config, &MODULE_CCU60);
    IfxCcu6_Icu_initModule(&ccu6Handle, &ccu6Config);                   /* Initialize the module */

    /* Store the configured captured frequency */
    g_CCU6Frequency_Hz = ccu6Handle.captureFrequency;

    IfxCcu6_Icu_ChannelConfig ccu6ChannelConfig;                        /* Create a channel configuration structure */
    IfxCcu6_Icu_initChannelConfig(&ccu6ChannelConfig, &MODULE_CCU60);
    ccu6ChannelConfig.channelId = IfxCcu6_T12Channel_0;                 /* Timer T12 channel 0 */

    ccu6ChannelConfig.channelMode = IfxCcu6_T12ChannelMode_doubleRegisterCaptureRisingAndFalling;

    /* Service Request 0 for interrupt at any rising or falling edge */
    ccu6ChannelConfig.interrupt1.source = IfxCcu6_InterruptSource_cc60RisingEdge;   /* Select the interrupt source    */
    ccu6ChannelConfig.interrupt1.serviceRequest = IfxCcu6_ServiceRequest_0;         /* Select the service request output    */
    ccu6ChannelConfig.interrupt1.priority = ISR_PRIORITY_CCU6_RISING_EDGE;           /* Interrupt priority  */
    ccu6ChannelConfig.interrupt1.typeOfService = ISR_PROVIDER_CCU6_RISING_EDGE;       /* Interrupt service */

    /* Service Request 1 for interrupt at T12 period match */
    ccu6ChannelConfig.interrupt2.source = IfxCcu6_InterruptSource_t12PeriodMatch;           /* Select the interrupt source    */
    ccu6ChannelConfig.interrupt2.serviceRequest = IfxCcu6_ServiceRequest_1;                 /* Select the service request output    */
    ccu6ChannelConfig.interrupt2.priority = ISR_PRIORITY_CCU6_PERIOD_MATCH;                 /* Interrupt priority     */
    ccu6ChannelConfig.interrupt2.typeOfService = ISR_PROVIDER_CCU6_PERIOD_MATCH;           /* Interrupt service */

    /* Configure the start of Timer T12 */
    ccu6ChannelConfig.trigger.extInputTrigger = NULL_PTR;

    /* CCU6 ICU pin configuration */
    IfxCcu6_Icu_Pins pins =
    {
    	&CCU6_INPUT_PIN,	        /* CC60In pin */
		NULL_PTR, 	                /* CC61In pin not used */
        NULL_PTR,                   /* CC62In pin not used */
        NULL_PTR,                   /* CCPOS0In pin not used */
        NULL_PTR,                   /* CCPOS1In pin not used */
        NULL_PTR,                   /* CCPOS2In pin not used */
        IfxPort_InputMode_pullUp,   /* CC6x pin input mode */
        IfxPort_InputMode_pullUp,   /* CCPOSx pin input mode */
        NULL_PTR,                   /* T12HR input signal not used */
        NULL_PTR,                   /* T13HR input signal not used */
        IfxPort_InputMode_pullUp    /* Pad driver mode */
    };
    ccu6ChannelConfig.pins = &pins;                                     /* Set the configuration */

    ccu6ChannelConfig.multiInputCaptureEnabled = FALSE;                 /* Disable multiple input capture */

    IfxCcu6_Icu_initChannel(&g_icuChannel, &ccu6ChannelConfig);         /* Initialize the channel */

    /* Start the timer T12 */
    IfxCcu6_Icu_startCapture(&g_icuChannel);
}

void CCU6_PWM_result()
{
    /* Write the measured frequency and duty cycle */
	printf("Freq: %f, Duty: %f\n",g_measuredPwmFreq_Hz, g_measuredPwmDutyCycle);
}
