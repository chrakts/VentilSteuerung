/*
 * CommandFunctions.cpp
 *
 * Created: 26.04.2017 14:54:45
 *  Author: a16007
 */

#include "CommandFunctions.h"
#include "External.h"
#include "../Secrets/secrets.h"

INFORMATION information[NUM_INFORMATION]=
{
  {"DT",'t','1','N',STRING,3,(void*)strStatusNachtabsenkung,gotStatusNachtabsenkung},
};

COMMAND cnetCommands[NUM_COMMANDS] =
	{
    cmultiStandardCommands,
		{'P','i',CUSTOMER,NOPARAMETER,0,jobGetIDNumber},
		{'P','s',CUSTOMER,NOPARAMETER,0,jobGetSerialNumber},
		{'P','x',CUSTOMER,NOPARAMETER,0,jobGetIndex},
		{'P','I',PRODUCTION,STRING,13,jobSetIDNumber},
		{'P','S',PRODUCTION,STRING,13,jobSetSerialNumber},
		{'P','X',PRODUCTION,STRING,3,jobSetIndexNumber},
		{'C','t',CUSTOMER,NOPARAMETER,0,jobGetCTemperatureSensor},
		{'C','h',CUSTOMER,NOPARAMETER,0,jobGetCHumiditySensor},
		{'C','d',CUSTOMER,NOPARAMETER,0,jobGetCDewPointSensor},
		{'C','a',CUSTOMER,NOPARAMETER,0,jobGetCAbsHumiditySensor},
		{'V','a',CUSTOMER,NOPARAMETER,0,jobGetHeaterActualStatus},
		{'V','s',CUSTOMER,NOPARAMETER,0,jobGetHeaterSetStatus},
		{'V','D',CUSTOMER,UINT_8,1,jobSetHeaterOnValue},
		{'V','N',CUSTOMER,UINT_8,1,jobSetHeaterOnNightValue},
		{'V','H',CUSTOMER,UINT_8,1,jobSetHeaterHystValue},
		{'V','S',CUSTOMER,STRING,8,jobSetHeaterSetStatus},
		{'V','l',CUSTOMER,NOPARAMETER,0,jobGetHeaterOnValue},
		{'V','h',CUSTOMER,NOPARAMETER,0,jobGetHeaterHystValue},
		{'T','B',CUSTOMER,UINT_16,1,jobSetTimeBetweenBlocks},
		{'T','S',CUSTOMER,UINT_16,1,jobSetTimeBetweenSensors},
		{'T','W',CUSTOMER,UINT_16,1,jobWaitAfterLastSensor}
	};

void gotStatusNachtabsenkung()
{
	if(strStatusNachtabsenkung[1]=='n')
	  statusNachtabsenkung = true;
  else
	  statusNachtabsenkung = false;
  if(statusNachtabsenkung==true)
    LEDROT_ON;
  else
    LEDROT_OFF;
}


void jobGetHeaterOnValue(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t adr = (uint8_t)address-48;
  if(adr<NUMBER_OF_VENTS)
    comRec->sendAnswerInt(function,address,job,u8HeatSwell[adr],true);
  else
    comRec->sendPureAnswer(function,address,job,false);
}

void jobGetHeaterHystValue(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t adr = (uint8_t)address-48;
  if(adr<NUMBER_OF_VENTS)
    comRec->sendAnswerInt(function,address,job,u8HeatHysterese[adr],true);
  else
    comRec->sendPureAnswer(function,address,job,false);
}

void jobGetHeaterSetStatus(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t adr = (uint8_t)address-48;
  if(adr<NUMBER_OF_VENTS)
    cnet.sendStandard(heaterStatusStrings[u8HeatSetStatus[adr]],BROADCAST,'V',address,'s',true);
  else
    comRec->sendPureAnswer(function,address,job,false);

}

void jobGetHeaterActualStatus(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t adr = (uint8_t)address-48;
  if(adr<NUMBER_OF_VENTS)
    comRec->sendAnswer(heaterStatusStrings[u8HeatActualStatus[adr]],'V',address,'a',true);
  else
    comRec->sendPureAnswer(function,address,job,false);
}

void jobSetHeaterOnValue(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t adr = (uint8_t)address-48;
  if(adr<NUMBER_OF_VENTS)
  {
    u8HeatSwell[adr] = ( (uint8_t*) pMem )[0];
    comRec->Getoutput()->broadcastUInt8(u8HeatSwell[adr],function,address,job);
  }
  else
    comRec->sendPureAnswer(function,address,job,false);
}

void jobSetHeaterOnNightValue(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t adr = (uint8_t)address-48;
  if(adr<NUMBER_OF_VENTS)
  {
    u8HeatNightSwell[adr] = ( (uint8_t*) pMem )[0];
    comRec->Getoutput()->broadcastUInt8(u8HeatNightSwell[adr],function,address,job);
  }
  else
    comRec->sendPureAnswer(function,address,job,false);
}

void jobSetHeaterHystValue(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t adr = (uint8_t)address-48;
  if(adr<NUMBER_OF_VENTS)
  {
    u8HeatHysterese[adr] = ( (uint8_t*) pMem )[0];
    comRec->Getoutput()->broadcastUInt8(u8HeatHysterese[adr],function,address,job);
  }
  else
    comRec->sendPureAnswer(function,address,job,false);
}

void jobSetHeaterSetStatus(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
char *temp;
  uint8_t adr = ((uint8_t)address)-48;
	temp = (char*) pMem;
	if(adr<NUMBER_OF_VENTS)
  {
    switch(temp[0])
    {
      case 'a':
        u8HeatSetStatus[adr] = HEAT_STATUS_OFF;
      break;
      case 'e':
        u8HeatSetStatus[adr] = HEAT_STATUS_ON;
      break;
      case 'A':
        u8HeatSetStatus[adr] = HEAT_STATUS_AUTO;
      break;
    }
    reportHeatSetStatus(comRec->Getoutput(),adr);
  }
  else
    comRec->sendPureAnswer(function,adr+48,job,false);
}

void jobSetIDNumber(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	if (strlen((char*)pMem)<=11)
	{
		eeprom_write_block((char*)pMem,IDNumber,strlen((char*)pMem)+1);
		comRec->sendPureAnswer(function,address,job,true);
	}
	else
		comRec->sendPureAnswer(function,address,job,false);
}

void jobSetSerialNumber(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	if (strlen((char*)pMem)<=11)
	{
		eeprom_write_block((char*)pMem,SerialNumber,strlen((char*)pMem)+1);
		comRec->sendPureAnswer(function,address,job,true);
	}
	else
	comRec->sendPureAnswer(function,address,job,false);
}

void jobSetIndexNumber(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	if (strlen((char*)pMem)<=2)
	{
		eeprom_write_block((char*)pMem,IndexNumber,strlen((char*)pMem)+1);
		comRec->sendPureAnswer(function,address,job,true);
	}
	else
		comRec->sendPureAnswer(function,address,job,false);
}

void jobGetIDNumber(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	char temp[12];
	eeprom_read_block(temp,IDNumber,12);
	comRec->sendAnswer(temp,function,address,job,true);
}

void jobGetSerialNumber(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	char temp[12];
	eeprom_read_block(temp,SerialNumber,12);
	comRec->sendAnswer(temp,function,address,job,true);
}

void jobGetIndex(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	char temp[2];
	eeprom_read_block(temp,IndexNumber,2);
	comRec->sendAnswer(temp,function,address,job,true);
}

void jobGetCTemperatureSensor(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
char answer[20]="";

	sprintf(answer,"%f",(double)fTemperatur);
	comRec->sendAnswer(answer,function,address,job,true);
}

void jobGetCHumiditySensor(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	char answer[20]="";

	sprintf(answer,"%f",(double)fHumidity);
	comRec->sendAnswer(answer,function,address,job,true);
}

void jobGetCAbsHumiditySensor(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	char answer[20]="";

	sprintf(answer,"%f",(double)fAbsHumitdity);
	comRec->sendAnswer(answer,function,address,job,true);
}

void jobGetCDewPointSensor(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	char answer[20]="";

	sprintf(answer,"%f",(double)fDewPoint);
	comRec->sendAnswer(answer,function,address,job,true);
}

void jobSetTimeBetweenBlocks(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	actReportBetweenBlocks = ( (uint16_t*) pMem )[0];
	comRec->sendAnswerInt(function,address,job,actReportBetweenBlocks,true);
}

void jobSetTimeBetweenSensors(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	actReportBetweenSensors = ( (uint16_t*) pMem )[0];
	comRec->sendAnswerInt(function,address,job,actReportBetweenSensors,true);
}

void jobWaitAfterLastSensor(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	actWaitAfterLastSensor = ( (uint16_t*) pMem )[0];
	comRec->sendAnswerInt(function,address,job,actWaitAfterLastSensor,true);
}

void reportHeatSetStatus(Communication *com, uint8_t adr)
{
  com->sendStandard(heaterStatusStrings[u8HeatSetStatus[adr]],BROADCAST,'V','0'+adr,'s','T');
}

void reportHeatActualStatus(Communication *com, uint8_t adr)
{
  com->sendStandard(heaterStatusStrings[u8HeatActualStatus[adr]],BROADCAST,'V','0'+adr,'a','T');
}
