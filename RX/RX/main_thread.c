#include <stdlib.h>
#include "RFQueue.h"
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>
#include <ti_radio_config.h>
#include <ti/drivers/rf/RF.h>
#include "ti_drivers_config.h"
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

/* Packet RX Configuration */
#define DATA_ENTRY_HEADER_SIZE 8
#define MAX_LENGTH             4
#define NUM_DATA_ENTRIES       2
#define NUM_APPENDED_BYTES     2

static void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

static RF_Object rfObject;
static RF_Handle rfHandle;

static UART_Handle uart;
static UART_Params uartParams;

/* Buffer which contains all Data Entries for receiving data.
 * Pragmas are needed to make sure this buffer is 4 byte aligned (requirement from the RF Core) */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN (rxDataEntryBuffer, 4);
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_LENGTH, NUM_APPENDED_BYTES)];
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 4
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_LENGTH, NUM_APPENDED_BYTES)];
#elif defined(__GNUC__)
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_LENGTH, NUM_APPENDED_BYTES)] __attribute__((aligned(4)));
#else
#error This compiler is not supported.
#endif

/* Receive dataQueue for RF Core to fill in data */
static dataQueue_t dataQueue;
static rfc_dataEntryGeneral_t* currentDataEntry;
static uint8_t packetLength;
static uint8_t* packetDataPointer;
static uint8_t packet[MAX_LENGTH + NUM_APPENDED_BYTES - 1];
char msg[7];

void *mainThread(void *arg0) {
    RF_Params rfParams;
    RF_Params_init(&rfParams);

    GPIO_init();
    GPIO_write(RED_LED, CONFIG_GPIO_LED_OFF);

    UART_init();
    UART_Params_init(&uartParams);
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL) {
        while (1);
    }

    if( RFQueue_defineQueue(&dataQueue, rxDataEntryBuffer, sizeof(rxDataEntryBuffer), NUM_DATA_ENTRIES, MAX_LENGTH + NUM_APPENDED_BYTES)) {
        /* Failed to allocate space for all data entries */
        while(1);
    }
    /* Our pointer to the queue for entry data */
    RF_cmdPropRx.pQueue = &dataQueue;
    /* Clean queue from failed packets */
    RF_cmdPropRx.rxConf.bAutoFlushIgnored = 1;
    RF_cmdPropRx.rxConf.bAutoFlushCrcErr = 1;
    /* Set max length */
    RF_cmdPropRx.maxPktLen = MAX_LENGTH;
    /* Turn on infinity work of our rx */
    RF_cmdPropRx.pktConf.bRepeatOk = 1;
    RF_cmdPropRx.pktConf.bRepeatNok = 1;

    /* Request access to the radio */
#if defined(DeviceFamily_CC26X0R2)
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioSetup, &rfParams);
#else
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
#endif

    /* Set the frequency */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

    /* Enter RX mode and stay forever in RX */
    RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropRx, RF_PriorityNormal, &callback, RF_EventRxEntryDone);
    while(1);
}

void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e) {
    if (e & RF_EventRxEntryDone) {
        /* Copy all data that we received */
        currentDataEntry  = RFQueue_getDataEntry();
        packetLength      = *(uint8_t*)(&currentDataEntry->data);
        packetDataPointer = (uint8_t*)(&currentDataEntry->data + 1);
        memcpy(packet, packetDataPointer, (packetLength + 1));

        sprintf(&msg, "%d %d\r\n", packet[2], packet[3]);
        UART_write(uart, msg, sizeof(msg));

        GPIO_toggle(RED_LED);
        RFQueue_nextEntry();
    }
}
