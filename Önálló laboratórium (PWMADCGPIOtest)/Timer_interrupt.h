#ifndef TIMER_INTERRUPT_H_
#define TIMER_INTERRUPT_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Configuration.h"

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

void initGpt12Timer(void);                          /* Function to initialize the GPT12 Module and start the timer  */
void interruptGpt12(void);                          /* Interrupt Service Routine of the GPT12                       */


#endif /* TIMER_INTERRUPT_H_ */
