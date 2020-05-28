/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include <stdio.h>
#include "SysSe/Bsp/Bsp.h"
#include "IfxScuWdt.h"
#include "IfxCpu.h"
#include "CCu6_PWM_Capture.h"
#include "CCU6_ADC.h"
#include "CCU6_PWM_Generation.h"
#include "GtmTomPwmGenerationHl.h"
#include "I2C.h"
#include "SPI.h"
#include "Timer_interrupt.h"
#include "VadcBackgroundScan.h"

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

IfxCpu_syncEvent g_cpuSyncEvent = 0;

/******************************************************************************/
/*-------------------------   Main function   --------------------------------*/
/******************************************************************************/

int core0_main(void)
{
    printf( "Hello world\n" );
 //   gpio_init_pins();
     /*
         * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
         * Enable the watchdog in the demo if it is required and also service the watchdog periodically
         * */
        IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
        IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

        /* Enable the global interrupts of this CPU */
        IfxCpu_enableInterrupts();

        IfxCpu_emitEvent(&g_cpuSyncEvent);
        IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

        //initVadcBackgroundScanDemo();    //ADC backgroud scan
        //initGtmTomPwmHl();               //Timer PWM

        //init_CCU6_ICU();                  // PWM measurement

        initCCU6();                         //PWM generation with CCU6
        startPWMGeneration();

       // setDutyCycle(20);                 //Timer PWM  duty cycle

        //init_ccu6();                      // ADC with CCU6
        //init_vadc();
        //start_ccu6_timer();


        //init_I2C_module();                //I2C init
        //read_write();

        while (TRUE)
        {
            for (int i = 0; i <= 100 ; i += 20)
            {
                CCU6_Duty(i);
                //setDutyCycle(i);
                wait(100000000);        //Timwe pwm duty
                //CCU6_PWM_result();
            }

            //runVadcBackgroundScanDemo()

        }
}
