/*
 * Globals.cpp
 *
 * Created: 19.03.2017 09:24:37
 *  Author: Christof
 */
#define EXTERNALS_H_

#include "VentilSteuerung.h"

//using namespace OneWire;
//using namespace RomCommands;

#define V1 "V1"


using namespace OneWire;
using namespace RomCommands;


const char *Node = NODE_STRING;
char IDNumber[12] EEMEM = "1784324-01";
char SerialNumber[12] EEMEM = "1958632254";
char IndexNumber[2] EEMEM = "A";
//TempSensor *activeSensor=NULL;

const char *heaterStatusStrings[3]={"aus","ein","Auto"};

uint16_t actReportBetweenBlocks  = REPORT_BETWEEN_BLOCKS;
uint16_t actReportBetweenSensors = REPORT_BETWEEN_SENSORS;
uint16_t actWaitAfterLastSensor  = WAIT_AFTER_LAST_SENSOR;

// Klimasensoren
volatile double fTemperatur=-999,fHumidity=-999,fDewPoint=-999,fAbsHumitdity=-999;



volatile uint8_t u8HeatSwell[NUMBER_OF_VENTS],u8HeatHysterese[NUMBER_OF_VENTS];
volatile uint8_t u8HeatSetStatus[NUMBER_OF_VENTS],u8HeatActualStatus[NUMBER_OF_VENTS],u8HeatActualStatusOld[NUMBER_OF_VENTS];

volatile uint8_t u8oldHeatSwell[NUMBER_OF_VENTS],u8oldHeatHysterese[NUMBER_OF_VENTS],u8oldHeatSetStatus[NUMBER_OF_VENTS];

uint8_t EEMEM ee_u8HeatSwell[NUMBER_OF_VENTS],ee_u8HeatHysterese[NUMBER_OF_VENTS];
uint8_t EEMEM ee_u8FanSetStatus[NUMBER_OF_VENTS];


volatile uint8_t statusSensoren = KLIMASENSOR;
volatile uint8_t statusReport = TEMPREPORT;
volatile bool    sendStatusReport = false;
volatile uint8_t statusKlima = NOTHING_CLIMA_TODO;
volatile uint8_t statusLastSensor = NOTHING_LAST_TODO;
volatile uint8_t statusTemperature=NOTHING_TODO;

int errno;      // Globale Fehlerbehandlung

char SecurityLevel = 0;

uint16_t measureRate_100ms=10;

uint8_t actNumberSensors = 0;
/* Global variables for TWI */

volatile bool nextSendReady=false;

Communication cnet(0,Node,5,true);
ComReceiver cnetRec(&cnet,Node,cnetCommands,NUM_COMMANDS,NULL,0,NULL,NULL);

/* Global variables for TWI */
TWI_MasterDriver_t twiC_Master;    /*!< TWI master module. */
TWI_MasterDriver_t twiE_Master;    /*!< TWI master module. */

shtc3 humiSensor;

DS2484 owm;

SearchState searchState;
MultidropRomIterator selector(owm);

TempSensor *tempSensors[NUMBER_OF_TEMPSENSORS];
SENSINFOS storedSensors[NUMBER_STORED_SENSORS] EEMEM;
