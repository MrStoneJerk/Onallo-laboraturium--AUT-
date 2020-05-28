

#ifndef GTMTOMPWMHL_H
#define GTMTOMPWMHL_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <Ifx_Types.h>
#include "Configuration.h"
#include "SysSe/Bsp/Bsp.h"
#include "Gtm/Tom/PwmHl/IfxGtm_Tom_PwmHl.h"


/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

typedef enum
{
    GtmTomPwmHl_State_init = 0,
    GtmTomPwmHl_State_off,
    GtmTomPwmHl_State_duty0,
    GtmTomPwmHl_State_duty50,
    GtmTomPwmHl_State_duty100,
    GtmTomPwmHl_State_count
}GtmTomPwmHl_State;

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** \brief Application information */
typedef struct
{
    struct
    {
        float32           gtmFreq;
        float32           gtmGclkFreq;
        float32           gtmCmuClk0Freq; /**< \brief CMU CLK 0 frequency*/
        GtmTomPwmHl_State state;
        Ifx_TickTime      stateDeadline;
    }info;
    struct
    {
        IfxGtm_Tom_Timer timer;        /**< \brief Timer driver */
        IfxGtm_Tom_PwmHl pwm;
    }drivers;
    struct
    {
        uint32 slotTimer;
    }isrCounter;
}App_GtmTomPwmHl;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

IFX_EXTERN App_GtmTomPwmHl g_GtmTomPwmHl;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

IFX_EXTERN void initGtmTomPwmHl(void);
IFX_EXTERN void setDutyCycle(int);

#endif
