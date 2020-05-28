

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "Ifx_Cfg.h"
#include "ConfigurationIsr.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Demo_VadcBackgroundScanDemo_SrcDoc_Config
 * \{ */
/*______________________________________________________________________________
** Help Macros
**____________________________________________________________________________*/
/**
 * \name Macros for Regression Runs
 * \{
 */
#ifndef REGRESSION_RUN_STOP_PASS
#define REGRESSION_RUN_STOP_PASS
#endif

#ifndef REGRESSION_RUN_STOP_FAIL
#define REGRESSION_RUN_STOP_FAIL
#endif

/** \} */
#define ADC_STARTUP_CALIBRATION 1  /**< \brief Enable Calibration for TC27xB,TC26x and TC29x Derivatives */

/** \} */
#endif
