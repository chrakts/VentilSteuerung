/*
 * CommandFunctions.h
 *
 * Created: 26.04.2017 14:55:18
 *  Author: a16007
 */


#ifndef COMMANDFUNCTIONS_H_
#define COMMANDFUNCTIONS_H_

#include "VentilSteuerung.h"
extern COMMAND cnetCommands[];

#define NUM_COMMANDS 24+CMULTI_STANDARD_NUM

void jobSetHeater1OnValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetHeater2OnValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetHeater1HystValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetHeater2HystValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetHeaterSetStatus(ComReceiver *comRec, char function,char address,char job, void * pMem);

void jobGetHeater1OnValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetHeater2OnValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetHeater1HystValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetHeater2HystValue(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetHeaterSetStatus(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetHeaterActualStatus(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetHeaterSetStatus(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetHeaterActualStatus(ComReceiver *comRec, char function,char address,char job, void * pMem);

void jobGetCTemperatureSensor(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetCHumiditySensor(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetCAbsHumiditySensor(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetCDewPointSensor(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetPressure(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetSealevel(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetSealevel(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetLight(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobTestTripleIntParameter(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobTestStringParameter(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobTestFloatParameter(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetTimeBetweenBlocks(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetTimeBetweenSensors(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobWaitAfterLastSensor(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetIDNumber(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetSerialNumber(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetIndex(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetIDNumber(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetSerialNumber(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobSetIndexNumber(ComReceiver *comRec, char function,char address,char job, void * pMem);

void reportHeatSetStatus(Communication *com, uint8_t address);
void reportHeatActualStatus(Communication *com, uint8_t address);
#endif /* COMMANDFUNCTIONS_H_ */
