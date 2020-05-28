
/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "CCU6_ADC.h"
#include "Ifx_Types.h"
#include "stdio.h"
#include "Ccu6/Timer/IfxCcu6_Timer.h"
#include "Vadc/Adc/IfxVadc_Adc.h"


/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

#define ADC_CHANNEL                     0                               /* ADC channel */


#define CCU6_T12_TIMER_FREQ             97656                          /* Timer T12 module frequency in Hz */
#define CCU6_T13_TIMER_FREQ             97656                           /* Timer T13 module frequency in Hz */
#define CCU6_T12_TIMER_MATCH_FREQ       1                               /* Timer T12 Period Match frequency in Hz */
#define CCU6_T13_TIMER_MATCH_PERIOD     0.5f                            /* Timer T13 Period Match time (secs) */

/* Timers period (tick) */
#define CCU6_T12_TIMER_PERIOD           (CCU6_T12_TIMER_FREQ / CCU6_T12_TIMER_MATCH_FREQ) - 1
#define CCU6_T13_TIMER_PERIOD           (CCU6_T13_TIMER_FREQ * CCU6_T13_TIMER_MATCH_PERIOD) - 1

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
/* VADC handle */
IfxVadc_Adc g_vadc;
IfxVadc_Adc_Group g_adcGroup;
IfxVadc_Adc_Channel g_adcChannel;

/* Timer handle */
IfxCcu6_Timer g_timer_adc;


/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* CCU6 initialization function */
void init_ccu6(void)
{
    IfxCcu6_Timer_Config timerConfig;
    IfxCcu6_Timer_initModuleConfig(&timerConfig, &MODULE_CCU60);

    /* Configure Timer T12 and Timer T13 */

    timerConfig.base.t12Frequency = CCU6_T12_TIMER_FREQ;                /* Timer T12 frequency */
    timerConfig.base.waitingTime = CCU6_T12_TIMER_PERIOD;               /* Timer T12 period */
    timerConfig.base.t13Frequency = CCU6_T13_TIMER_FREQ;                /* Timer T13 frequency */
    timerConfig.base.t13Period = CCU6_T13_TIMER_PERIOD;                 /* Timer T13 period */
    timerConfig.timer = IfxCcu6_TimerId_t13;                            /* Timer T13 as master */
    timerConfig.synchronousOperation = FALSE;                           /* Disable synchronous start  */
    timerConfig.trigger.t13InSyncWithT12 = TRUE;                        /* Timer T12 starts Timer T13 */

    /* Configure the T13 timer start */
    timerConfig.timer13.t12SyncEvent = IfxCcu6_T13TriggerEvent_onT12Period;
    timerConfig.timer13.t12SyncDirection = IfxCcu6_T13TriggerDirection_onT12CountingUp;

    /* Apply config to  CCU6 */
    IfxCcu6_Timer_initModule(&g_timer_adc, &timerConfig);

    /* Activate the line ServiceRequest_3 to route the T13 period match event trigger to the VADC module */
    IfxCcu6_enableInterrupt(&MODULE_CCU60, IfxCcu6_InterruptSource_t13PeriodMatch);
    IfxCcu6_routeInterruptNode(&MODULE_CCU60, IfxCcu6_InterruptSource_t13PeriodMatch, IfxCcu6_ServiceRequest_3);

    /* Configure the T13 timer */
    IfxCcu6_enableSingleShotMode(&MODULE_CCU60, IfxCcu6_TimerId_t13);
}

/* Function to start the CCU6 timer */
void start_ccu6_timer(void)
{
    IfxCcu6_Timer_start(&g_timer_adc);
}

/* Configuration and initialization of the VADC module */
void init_vadc(void)
{
    /* Create config */
    IfxVadc_Adc_Config adcConfig;
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);

    /* Initialize module */
    IfxVadc_Adc_initModule(&g_vadc, &adcConfig);

    /* Create and initialize group configuration */
    IfxVadc_Adc_GroupConfig adcGroupConfig;
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_vadc);

    /* Setting user configuration using group 0 */
    adcGroupConfig.groupId = IfxVadc_GroupId_0;                                         /* Select the group */
    adcGroupConfig.master = adcGroupConfig.groupId;                                     /* Select the master group  */
    adcGroupConfig.arbiter.requestSlotScanEnabled = TRUE;                               /* Enable scan source */
    adcGroupConfig.scanRequest.autoscanEnabled = FALSE;                                 /* Disable auto scan */

    adcGroupConfig.scanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;

    /* Configure the VADC module to trigger */
    adcGroupConfig.scanRequest.triggerConfig.triggerSource = IfxVadc_TriggerSource_0;
    adcGroupConfig.scanRequest.triggerConfig.triggerMode = IfxVadc_TriggerMode_uponRisingEdge;

    adcGroupConfig.scanRequest.requestSlotStartMode = IfxVadc_RequestSlotStartMode_cancelInjectRepeat;

    /* Initialize the group */
    IfxVadc_Adc_initGroup(&g_adcGroup, &adcGroupConfig);

    /* Channel configuration */
    IfxVadc_Adc_ChannelConfig adcChannelConfig;
    IfxVadc_Adc_initChannelConfig(&adcChannelConfig, &g_adcGroup);

    adcChannelConfig.channelId = (IfxVadc_ChannelId) (ADC_CHANNEL);             /* Select the channel */
    adcChannelConfig.resultRegister = (IfxVadc_ChannelResult) (ADC_CHANNEL);    /* Result register */

    /* Initialize the channel */
    IfxVadc_Adc_initChannel(&g_adcChannel, &adcChannelConfig);

    /* Add the channel to the scan seq */
    uint32 channels = (1 << adcChannelConfig.channelId);
    uint32 mask = channels;
    IfxVadc_Adc_setScan(&g_adcGroup, channels, mask);
}

/* ADC Interrupt Service Routine */
IFX_INTERRUPT(ISR_ADC_result, 0, ISR_PRIORITY_ADC);

void ISR_ADC_result(void)
{
    Ifx_VADC_RES conversionResult = IfxVadc_Adc_getResult(&g_adcChannel);
    /* Write the result */
    printf("ADC: %d\n", conversionResult.B.RESULT);
}



