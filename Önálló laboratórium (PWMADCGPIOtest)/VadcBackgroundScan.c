
/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "VadcBackgroundScan.h"
#include <stdio.h>
#include <Cpu/Std/IfxCpu.h>

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

App_VadcBackgroundScan g_VadcBackgroundScan;

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

/* Function to initailize the module */
void initVadcBackgroundScan(void)
{
    /* VADC Configuration */
    IfxVadc_Adc_Config adcConfig;
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);

    /* initialize module */
    IfxVadc_Adc_initModule(&g_VadcBackgroundScan.vadc, &adcConfig);

    /* create group config */
    IfxVadc_Adc_GroupConfig adcGroupConfig;
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_VadcBackgroundScan.vadc);

    /* with group 0 */
    adcGroupConfig.groupId = IfxVadc_GroupId_0;
    adcGroupConfig.master  = adcGroupConfig.groupId;

    /* enable background scan source */
    adcGroupConfig.arbiter.requestSlotBackgroundScanEnabled = TRUE;

    /* enable background auto scan */
    adcGroupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;

    /* enable all gates in "always" mode (no edge detection) */
    adcGroupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;

    /* initialize the group */
    IfxVadc_Adc_initGroup(&g_VadcBackgroundScan.adcGroup, &adcGroupConfig);

    /* initialize the group */
    IfxVadc_Adc_initGroup(&g_VadcBackgroundScan.adcGroup, &adcGroupConfig);
}


/* Function to run the scan */
void runVadcBackgroundScan(void)
{
	IfxCpu_disableInterrupts();
    //printf("VadcBackgroundScanDemo_run() called\n");


    //printf("Add Background Scan channels\n");

    {
        uint32                    chnIx;

        /* create channel config */
        IfxVadc_Adc_ChannelConfig adcChannelConfig[2];
        IfxVadc_Adc_Channel       adcChannel[2];

        for (chnIx = 0; chnIx < 2; ++chnIx)
        {
            IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &g_VadcBackgroundScan.adcGroup);

            adcChannelConfig[chnIx].channelId         = (IfxVadc_ChannelId)(chnIx + 5);
            adcChannelConfig[chnIx].resultRegister    = (IfxVadc_ChannelResult)(5 + chnIx); // use register #5 and 6 for results
            adcChannelConfig[chnIx].backgroundChannel = TRUE;

            /* initialize the channel */
            IfxVadc_Adc_initChannel(&adcChannel[chnIx], &adcChannelConfig[chnIx]);

            /* add to background scan */
            unsigned channels = (1 << adcChannelConfig[chnIx].channelId);
            unsigned mask     = channels;
            IfxVadc_Adc_setBackgroundScan(&g_VadcBackgroundScan.vadc, &g_VadcBackgroundScan.adcGroup, channels, mask);
        }

        /* start scan */
        IfxVadc_Adc_startBackgroundScan(&g_VadcBackgroundScan.vadc);

        /* check results */
        for (chnIx = 0; chnIx < 1; ++chnIx) // <2
        {
            unsigned     group   = adcChannel[chnIx].group->groupId;
            unsigned     channel = adcChannel[chnIx].channel;

            /* wait for valid result */
            Ifx_VADC_RES conversionResult;

            do
            {
                conversionResult = IfxVadc_Adc_getResult(&adcChannel[chnIx]);
            } while (!conversionResult.B.VF);

            /* print result, check with expected value */
            {
                uint32 actual = conversionResult.B.RESULT;
                printf("Group %d Channel %d: , result : %d\n", group, channel, actual);
            }
        }
    }
    IfxCpu_enableInterrupts();
}
