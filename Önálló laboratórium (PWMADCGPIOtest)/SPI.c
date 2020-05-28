


/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "SPI.h"
#include "IfxPort.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* QSPI modules */
#define QSPI2_MASTER                &MODULE_QSPI2   /* SPI Master module */

#define MASTER_CHANNEL_BAUDRATE     400000         /* Master channel baud rate */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
qspiComm g_qspi;

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void initQSPIMaster(void);
void initQSPIMasterChannel(void);
void initQSPIMasterBuffers(void);
void initQSPI(void);

/*********************************************************************************************************************/
/*----------------------------------------------Function Implementations---------------------------------------------*/
/*********************************************************************************************************************/
IFX_INTERRUPT(masterTxISR, 0, ISR_PRIORITY_MASTER_TX);                  /* SPI Master ISR for transmit data */
IFX_INTERRUPT(masterRxISR, 0, ISR_PRIORITY_MASTER_RX);                  /* SPI Master ISR for receive data */
IFX_INTERRUPT(masterErISR, 0, ISR_PRIORITY_MASTER_ER);                  /* SPI Master ISR for error */


void masterTxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiMaster_isrTransmit(&g_qspi.spiMaster);
}

void masterRxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiMaster_isrReceive(&g_qspi.spiMaster);
}

void masterErISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiMaster_isrError(&g_qspi.spiMaster);
}


/* QSPI Master initialization*/

void initQSPIMaster(void)
{
    IfxQspi_SpiMaster_Config spiMasterConfig;                           /* Master configuration */

    IfxQspi_SpiMaster_initModuleConfig(&spiMasterConfig, QSPI2_MASTER);

    spiMasterConfig.base.mode = SpiIf_Mode_master;                      /* Configure the mode */

    /* Select the port pins for communication */
    const IfxQspi_SpiMaster_Pins qspi2MasterPins = {
        &IfxQspi2_SCLK_P15_3_OUT, IfxPort_OutputMode_pushPull,          /* SCLK Pin (CLK) */
        &IfxQspi2_MTSR_P15_6_OUT, IfxPort_OutputMode_pushPull,          /* MasterTransmitSlaveReceive pin (MOSI) */
        &IfxQspi2_MRSTA_P15_4_IN, IfxPort_InputMode_pullDown,           /* MasterReceiveSlaveTransmit pin (MISO) */
        IfxPort_PadDriver_cmosAutomotiveSpeed3                          /* Pad driver mode */
    };
    spiMasterConfig.pins = &qspi2MasterPins;                            /* Assign the Master's port pins */


    /* Initialize the QSPI Master module */
    IfxQspi_SpiMaster_initModule(&g_qspi.spiMaster, &spiMasterConfig);
}

/* QSPI Master channel initialization */
void initQSPIMasterChannel(void)
{
    IfxQspi_SpiMaster_ChannelConfig spiMasterChannelConfig;             /* Master Channel configuration */
    IfxQspi_SpiMaster_initChannelConfig(&spiMasterChannelConfig, &g_qspi.spiMaster);

    spiMasterChannelConfig.base.baudrate = MASTER_CHANNEL_BAUDRATE;     /* Set SCLK frequency */

    /* Select the port pin for the Chip Select signal */
    const IfxQspi_SpiMaster_Output qspi2SlaveSelect = {                 /* QSPI2 Master selects the Slave */
        &IfxQspi2_SLSO7_P20_10_OUT, IfxPort_OutputMode_pushPull,         /* Slave Select port pin (CS) */
        IfxPort_PadDriver_cmosAutomotiveSpeed1                          /* Pad driver mode */
    };
    spiMasterChannelConfig.sls.output = qspi2SlaveSelect;

    /* Initialize the QSPI Master channel */
    IfxQspi_SpiMaster_initChannel(&g_qspi.spiMasterChannel, &spiMasterChannelConfig);
}

/* QSPI Master SW buffer initialization */
void initQSPIMasterBuffers(void)
{
    for (uint8 i = 0; i < SPI_BUFFER_SIZE; i++)
    {
        g_qspi.spiBuffers.spiMasterTxBuffer[i] = (uint8)(i + 1);        /* Fill TX Master Buffer */
        g_qspi.spiBuffers.spiMasterRxBuffer[i] = 0;                     /* Clear RX Buffer  */
    }
}


/* This function initialize the QSPI modules */
void initQSPI(void)
{
    /* Initialize the Master */
    initQSPIMaster();
    initQSPIMasterChannel();
    initQSPIMasterBuffers();
}




/* Data transfer */
void transferData(void)
{
    /* Send a data stream */
    IfxQspi_SpiMaster_exchange(&g_qspi.spiMasterChannel, &g_qspi.spiBuffers.spiMasterTxBuffer[0], NULL_PTR, SPI_BUFFER_SIZE);

}


