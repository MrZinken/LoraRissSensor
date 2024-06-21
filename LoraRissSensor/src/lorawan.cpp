#include "main.h"

/*
 * set LoraWan_RGB to Active,the RGB active in loraWan
 * RGB red means sending;
 * RGB purple means joined done;
 * RGB blue means RxWindow1;
 * RGB yellow means RxWindow2;
 * RGB green means received done;
 */

/* OTAA para */
uint8_t devEui[] = {0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x};
uint8_t appEui[] = {0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x};
uint8_t appKey[] = {0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x};
/* ABP para*/
uint8_t nwkSKey[] = {0x, 0x, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85};
uint8_t appSKey[] = {0xd7, 0x, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67};
uint32_t devAddr = (uint32_t)0x007e6ae1;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

// LoraWan region, select in arduino IDE tools
LoRaMacRegion_t loraWanRegion = LORAMAC_REGION_EU868;

// LoraWan Class, Class A and Class C are supported
DeviceClass_t loraWanClass = CLASS_A;

// the application data transmission duty cycle.  value in [ms].
uint32_t appTxDutyCycle = 1000 * 60 * 60;

// OTAA or ABP
bool overTheAirActivation = true;

// ADR enable
bool loraWanAdr = true;

// set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again
bool keepNet = false;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = false;

bool lorawan_send = true;

// Application port
uint8_t appPort = 2;
/*!
 * Number of trials to transmit the frame, if the LoRaMAC layer did not
 * receive an acknowledgment. The MAC performs a datarate adaptation,
 * according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
 * to the following table:
 *
 * Transmission nb | Data Rate
 * ----------------|-----------
 * 1 (first)       | DR
 * 2               | DR
 * 3               | max(DR-1,0)
 * 4               | max(DR-1,0)
 * 5               | max(DR-2,0)
 * 6               | max(DR-2,0)
 * 7               | max(DR-3,0)
 * 8               | max(DR-3,0)
 *
 * Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
 * the datarate, in case the LoRaMAC layer did not receive an acknowledgment
 */
uint8_t confirmedNbTrials = 4;

bool initLorawanModule()
{
    deviceState = DEVICE_STATE_INIT;
    LoRaWAN.ifskipjoin();
    return true;
    debug_println("Lorawan setup done");
}

// Prepares the payload of the frame
void prepareTxFrame(uint8_t port)
{
    /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
     *appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
     *if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
     *if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
     *for example, if use REGION_CN470,
     *the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
     */
    digitalWrite(Vext, LOW); // Enable Vext
    delay(50);               // Wait for voltage to stabilize
    Wire.begin();
    Wire.setClock(400000); // Increase to fast I2C speed!

    if (initLinearSensor())
    {
        debug_println("Sensor setup done!");
    }
    else
    {
        debug_println("BME not initialized.");
    }
    uint8_t adc_distance = readLinearSensor_8();

    uint8_t battery = getBatteryLevel();

    digitalWrite(Vext, HIGH); // Disable Vext

    debug_print("ADC: ");
    debug_println(adc_distance);
    debug_print("Battery: ");
    debug_println(getBatteryLevel());

    appDataSize = 2; // Size of data being sent
    appData[0] = battery;
    appData[1] = adc_distance;        // Lower byte of distance
    

    debug_println(appData[0]);
}

void loraLoopHandler()
{
    switch (deviceState)
    {
    case DEVICE_STATE_INIT: // active after initialization
    {
        printDevParam();
        LoRaWAN.init(loraWanClass, loraWanRegion);
        deviceState = DEVICE_STATE_JOIN;
        break;
    }
    case DEVICE_STATE_JOIN: // joins with provided credentials
    {
        LoRaWAN.join();
        break;
    }
    case DEVICE_STATE_SEND: // preparesTXpayload and sends it
    {
        prepareTxFrame(appPort);
        if(lorawan_send){
        LoRaWAN.send();
        debug_println("Package send ");
        }
        
        deviceState = DEVICE_STATE_CYCLE;
        break;
    }
    case DEVICE_STATE_CYCLE:
    { // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr(0, APP_TX_DUTYCYCLE_RND);
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
    }
    case DEVICE_STATE_SLEEP: // sleeps for time given in appTxDutyCycle
    {
        LoRaWAN.sleep();
        break;
    }
    default:
    {
        deviceState = DEVICE_STATE_INIT;
        break;
    }
    }
}
