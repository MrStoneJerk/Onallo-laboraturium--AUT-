

#ifndef CONFIGURATIONISR_H
#define CONFIGURATIONISR_H
/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/** \brief Build the ISR configuration object
 * \param no interrupt priority
 * \param cpu assign CPU number
 */
#define ISR_ASSIGN(no, cpu)  ((no << 8) + cpu)

/** \brief extract the priority out of the ISR object */
#define ISR_PRIORITY(no_cpu) (no_cpu >> 8)

/** \brief extract the service provider  out of the ISR object */
#define ISR_PROVIDER(no_cpu) (no_cpu % 8)


/**
 * \name Interrupt priority configuration.
 * The interrupt priority range is [1,255]
 * \{
 */
#define ISR_PRIORITY_PRINTF_ASC0_TX         5               /* ASC0 transmit interrupt priority used by printf.c */
#define ISR_PRIORITY_PRINTF_ASC0_EX         6               /* ASC0 error interrupt priority used by printf.c */
#define ISR_PRIORITY_CCU6_RISING_EDGE       2				/* CCU6 rising edge interrupt priority */
#define ISR_PRIORITY_CCU6_PERIOD_MATCH      3               /* CCU6 period match interrupt priority */
#define ISR_PRIORITY_TIMER                  1               /* Timer interrupt priority */
#define ISR_PRIORITY_ADC                    4               /* ADC interrupt priority */
#define ISR_PRIORITY_GPT12_TIMER            7               /* Timer GPT12 interrupt priority */

/* Interrupt Service Routine priorities for Master SPI communication */
#define ISR_PRIORITY_MASTER_TX              50
#define ISR_PRIORITY_MASTER_RX              51
#define ISR_PRIORITY_MASTER_ER              52

/** \} */

/**
 * \name Interrupt service provider configuration.
 * \{ */
#define ISR_PROVIDER_PRINTF_ASC0_TX         IfxSrc_Tos_cpu0             /* ASC0 transmit interrupt provider used by printf.c   */
#define ISR_PROVIDER_PRINTF_ASC0_EX         IfxSrc_Tos_cpu0             /* ASC0 error interrupt provider used by printf.c */
#define ISR_PROVIDER_CCU6_RISING_EDGE       IfxSrc_Tos_cpu0             /* CCU6 rising edge interrupt provider */
#define ISR_PROVIDER_CCU6_PERIOD_MATCH      IfxSrc_Tos_cpu0             /* CCU6 period match interrupt provider */
#define ISR_PROVIDER_TIMER                  IfxSrc_Tos_cpu0             /* Timer interrupt provider */
#define ISR_PROVIDER_ADC                    IfxSrc_Tos_cpu0             /* ADC interrupt provider */
#define ISR_PROVIDER_GPT12_TIMER            IfxSrc_Tos_cpu0             /* Timer GPT12 interrupt provider */

/* Interrupt Service Routine priorities for Master SPI communication */
#define ISR_PROVIDER_MASTER_TX              IfxSrc_Tos_cpu0
#define ISR_PROVIDER_MASTER_RX              IfxSrc_Tos_cpu0
#define ISR_PROVIDER_MASTER_ER              IfxSrc_Tos_cpu0

/** \} */

/**
 * \name Interrupt configuration.
 * \{ */
#define INTERRUPT_PRINTF_ASC0_TX      ISR_ASSIGN(ISR_PRIORITY_PRINTF_ASC0_TX, ISR_PROVIDER_PRINTF_ASC0_TX)                  /**< \brief Define the ASC0 transmit interrupt priority used by printf.c */
#define INTERRUPT_PRINTF_ASC0_EX      ISR_ASSIGN(ISR_PRIORITY_PRINTF_ASC0_EX, ISR_PROVIDER_PRINTF_ASC0_EX)                  /**< \brief Define the ASC0 error interrupt priority used by printf.c */
#define INTERRUPT_CCU6_RISING_EDGE    ISR_ASSIGN(ISR_PRIORITY_CCU6_RISING_EDGE, ISR_PROVIDER_CCU6_RISING_EDGE)               /* Define the CCU6 rising edge interrupt priority           */
#define INTERRUPT_CCU6_PERIOD_MATCH   ISR_ASSIGN(ISR_PRIORITY_CCU6_PERIOD_MATCH, ISR_PROVIDER_CCU6_PERIOD_MATCH)
#define INTERRUPT_TIMER               ISR_ASSIGN(ISR_PRIORITY_TIMER, ISR_PROVIDER_TIMER)
#define INTERRUPT_ADC                 ISR_ASSIGN(ISR_PRIORITY_ADC, ISR_PROVIDER_ADC)
#define INTERRUPT_GPT12_TIMER         ISR_ASSIGN(ISR_PRIORITY_GPT12_TIMER, ISR_PROVIDER_GPT12_TIMER)

/* Interrupt Configuration for Master SPI communication */
#define INTERRUPT_MASTER_TX           ISR_ASSIGN(ISR_PRIORITY_MASTER_TX, ISR_PROVIDER_MASTER_TX)
#define INTERRUPT_MASTER_RX           ISR_ASSIGN(ISR_PRIORITY_MASTER_RX, ISR_PROVIDER_MASTER_RX)
#define INTERRUPT_MASTER_ER           ISR_ASSIGN(ISR_PRIORITY_MASTER_ER, ISR_PROVIDER_MASTER_ER)

/** \} */

/** \} */
//------------------------------------------------------------------------------

#endif
