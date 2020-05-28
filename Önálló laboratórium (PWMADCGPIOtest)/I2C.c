
/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Ifx_Types.h"
#include "I2c/I2c/IfxI2c_I2c.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

#define MCP_SCL_PIN             IfxI2c0_SCL_P02_5_INOUT          /* SCL PIN */
#define MCP_SDA_PIN             IfxI2c0_SDA_P02_4_INOUT         /* SDA PIN */
#define I2C_BAUDRATE            100000                          /* Baud rate (Hz) */

#define SLAVE_ADDRESS 0x57                            /* Slave device address */
#define MESSAGE  0xF2                            /* MAC address */
#define LENGTH_OF_TX_BUFFER       1                               /* Length of address of the register */
#define LENGTH_OF_RX_BUFFER   6                               /* Length of the MAC address */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxI2c_I2c          g_i2cHandle;                                /* I2C handle */
IfxI2c_I2c_Device   g_i2cslave;                             /* I2C Slave device handle*/

uint8 i2cRxBuffer[LENGTH_OF_RX_BUFFER] = {0, 0, 0, 0, 0, 0};    /* Global parameter for MAC address   */

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Initialize the master and the slave module*/
void init_I2C_module(void)
{
    /* Initialize module */
    IfxI2c_I2c_Config i2cConfig;                                                /*Configuration structure*/
    IfxI2c_I2c_initConfig(&i2cConfig, &MODULE_I2C0);

    /* I2C pin configuration */
    const IfxI2c_Pins MCP_PINS =
    {
            &MCP_SCL_PIN,                                                       /* SCL port pin */
            &MCP_SDA_PIN,                                                       /* SDA port pin */
            IfxPort_PadDriver_ttlSpeed1                                         /* Pad driver mode */
    };
    i2cConfig.pins = &MCP_PINS;                                                 /* Configure port pins*/
    i2cConfig.baudrate = I2C_BAUDRATE;                                          /* Configure baud rate */

    IfxI2c_I2c_initModule(&g_i2cHandle, &i2cConfig);                            /* Initialize module */

    /* Initialize device */
   IfxI2c_I2c_deviceConfig i2cDeviceConfig;                                    /* Device configuration */
   IfxI2c_I2c_initDeviceConfig(&i2cDeviceConfig, &g_i2cHandle);

    /* Because it is 7 bit long the device address has to be shifted by 1 */
   i2cDeviceConfig.deviceAddress = SLAVE_ADDRESS << 1;
   IfxI2c_I2c_initDevice(&g_i2cslave, &i2cDeviceConfig);                   /* Initialize the I2C device handle */
}

/* This function executes the I2C data transfer */
void read_write(void)
{
    /* Address of 6-byte MAC address location */
    uint8 i2cTxBuffer[LENGTH_OF_TX_BUFFER ] = {MESSAGE};

    /* Write data to device */
    while(IfxI2c_I2c_write(&g_i2cslave, &i2cTxBuffer[0], LENGTH_OF_TX_BUFFER ) == IfxI2c_I2c_Status_nak);
    //IfxI2c_I2c_write(&g_i2cslave, &i2cTxBuffer[0], LENGTH_OF_ADDRESS);
    /* Read the MAC address */
    while(IfxI2c_I2c_read(&g_i2cslave, &i2cRxBuffer[0], LENGTH_OF_RX_BUFFER) == IfxI2c_I2c_Status_nak);
}




