


#ifndef CCU6ICUDEMO_H
#define CCU6ICUDEMO_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <Ifx_Types.h>
#include "Configuration.h"
#include "Ccu6/Icu/IfxCcu6_Icu.h"
#include "Ifx_Types.h"
#include "SysSe/Bsp/Bsp.h"

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

void init_CCU6_ICU(void);                       /* Function to configure the CCU6  */
void CCU6_PWM_result(void);						/* Function to print the measured freq and dutycycle */

#endif
