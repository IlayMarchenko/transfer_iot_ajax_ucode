#include <stdlib.h>
#include <unistd.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
#include <ti_radio_config.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/rf/RF.h>
#include "ti_drivers_config.h"
#include <ti/sail/bme280/bme280.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

#define PAYLOAD_LENGTH 4

static RF_Object rfObject;
static RF_Handle rfHandle;

static uint8_t packet[PAYLOAD_LENGTH];
static uint16_t seqNumber;

I2C_Handle      i2c;
I2C_Params      i2cParams;

s32 Temperature = 0;
u32 Pressure    = 0;
u32 Humidity    = 0;

char msg[7];

extern s32 bme280_data_readout_template(I2C_Handle i2cHndl);

void *mainThread(void *arg0) {

    I2C_init();
    GPIO_init();
    GPIO_write(GREEN_LED, CONFIG_GPIO_LED_OFF);

    /* Initialize I2C connection */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(CONFIG_I2C_BME, &i2cParams);
    if (i2c == NULL) {
        while(1);
    }

    /* Initialize the BME Sensor */
    if(BME280_INIT_VALUE != bme280_data_readout_template(i2c)) {
        while(1);
    }

    bme280_set_power_mode(BME280_NORMAL_MODE);

    RF_Params rfParams;
    RF_Params_init(&rfParams);

    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;

    /* Request access to the radio */
#if defined(DeviceFamily_CC26X0R2)
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioSetup, &rfParams);
#else
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
#endif// DeviceFamily_CC26X0R2

    /* Set the frequency */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

    while(1) {
        if (BME280_INIT_VALUE == bme280_read_pressure_temperature_humidity(&Pressure, &Temperature, &Humidity)) {
            Temperature /= 100;
            Humidity /= 1000;
            if (Temperature > 20 || Temperature < 15 || Humidity > 80 || Humidity < 60) {
                /* Create packet with incrementing sequence number and values of humidity and temperature */
                 packet[0] = (uint8_t)(seqNumber >> 8);
                 packet[1] = (uint8_t)(seqNumber++);
                 packet[2] = (uint8_t) Temperature;
                 packet[3] = (uint8_t) Humidity;

                 /* Send packet */
                 RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, NULL, 0);

                 GPIO_toggle(GREEN_LED);
                 RF_yield(rfHandle);
            }
            sleep(60);
        }
    }
}
