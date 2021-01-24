/*
* VentilSteuerung.cpp
*
* Created: 16.03.2017 13:03:01
* Author : a16007
*/

#include "VentilSteuerung.h"

void setup()
{
	//_delay_ms(10); // gibt dem Schaltregler Zeit

	// Achtung: Prozessor ist runtergetaktet auf knapp 30MHz
  init_clock(SYSCLK,PLL,true,CLOCK_CALIBRATION);
	PORTA_DIRSET = PIN2_bm | PIN3_bm | PIN4_bm;
	PORTA_OUTSET = 0xff;

	PORTB_DIRSET = 0xff;

	PORTC_DIRSET = PIN1_bm;

	PORTD_DIRSET = PIN0_bm | PIN4_bm | PIN5_bm | PIN7_bm;
	PORTD_DIRCLR = PIN6_bm;
	PORTD_OUTCLR = PIN4_bm | PIN5_bm;

	PORTE_DIRSET = 0xff;

	uint8_t i;

  TWI_MasterInit(&twiC_Master, &TWIC, TWI_MASTER_INTLVL_LO_gc, TWI_BAUDSETTING);
  TWI_MasterInit(&twiE_Master, &TWIE, TWI_MASTER_INTLVL_LO_gc, TWI_BAUDSETTING);

	for(i=0;i<20;i++)
	{
		LEDGRUEN_TOGGLE;
		_delay_ms(50);
	}
  LEDGRUEN_OFF;

  for(i=0;i<NUMBER_OF_VENTS;i++)
  {
    fHeatSwell[i]           = 35.1;
    fHeatNightSwell[i]      = 25.3;
    fHeatHysterese[i]       = 3.4;
    u8HeatSetStatus[i]       = HEAT_STATUS_AUTO;
    u8HeatActualStatus[i]    = HEAT_STATUS_ON;
    u8HeatActualStatusOld[i] = HEAT_STATUS_UNVALID;
    foldHeatSwell[i]        = -33.5;
    foldHeatHysterese[i]    = -5.4;
    u8oldHeatSetStatus[i]    = HEAT_STATUS_UNVALID;
  }



	PMIC_CTRL = PMIC_LOLVLEX_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm;
	sei();

	cnet.open(Serial::BAUD_57600,F_CPU);
}

int main(void)
{
uint8_t reportStarted = false;
static uint8_t indexToReport = 0;

	setup();

	cnet.broadcastUInt8((uint8_t) RST.STATUS,'S','0','R');

  readEEData();

	init_mytimer();
 	setup_twi();


	uint8_t sensorReady=SENSOR_READY;
	MAX7328 maxTest(&twiE_Master,I2C_EXTENDER_ADDRESS);
  maxTest.newValue(0xff);
  while(!TWI_MasterReady(&twiE_Master))
    ;

  LEDGRUEN_ON;

#ifdef KLIMASENSOR
  humiSensor.begin(&twiE_Master);
  humiSensor.setMode(SHTC3_NORMAL_T_FIRST);
  _delay_ms(10);
  cnet.broadcastUInt16(humiSensor.getID(),'a','b','c');
#endif

	while (1)
	{
		cnetRec.comStateMachine();
		cnetRec.doJob();
		// Ermittlung des neuen Heizungsstatus
		for( uint8_t i=0;i<NUMBER_OF_VENTS;i++)
    {
      if(u8HeatSetStatus[i]!=HEAT_STATUS_AUTO)
        u8HeatActualStatus[i] = u8HeatSetStatus[i];
      else // Heat = Auto
      {
        double swell = fHeatSwell[i];
        if(statusNachtabsenkung)
          swell = fHeatNightSwell[i];
        if(u8HeatActualStatus[i]==HEAT_STATUS_ON)
          swell += fHeatHysterese[i];
        if(tempSensors[i]->getMeanTemperature()>swell)
          u8HeatActualStatus[i]=HEAT_STATUS_OFF;
        else
          u8HeatActualStatus[i]=HEAT_STATUS_ON;
      }

    }
    // Setzen der Relais und der LEDs entsprechend des Heizungsstatus
    uint8_t outputs = 0xff;
    if(u8HeatActualStatus[0]==HEAT_STATUS_ON)
      outputs ^= LED_RGB_BLUE | POWER_3;
    if(u8HeatActualStatus[1]==HEAT_STATUS_ON)
      outputs ^= LED_RGB_GREEN | POWER_2;
    maxTest.updateValue(outputs);

    // Falls sich der Heizungsstatus geändert, wird dieser gesendet
		for( uint8_t i=0;i<NUMBER_OF_VENTS;i++)
    {
      if( u8HeatActualStatusOld[i] != u8HeatActualStatus[i])
      {
        reportHeatActualStatus(&cnet,i);
        u8HeatActualStatusOld[i]=u8HeatActualStatus[i];
      }
    }
		switch(statusSensoren)
		{
			case KLIMASENSOR:
#ifdef KLIMASENSOR
				sensorReady = doClima();
#else
        sensorReady = SENSOR_READY;
#endif
			break;
			case TEMPSENSOREN:
        sensorReady = doTemperature();
      break;
			case LASTSENSOR:
				sensorReady = doLastSensor();
			break;
		}
		if (sensorReady==SENSOR_READY)
		{
			statusSensoren++;
			if (statusSensoren>LASTSENSOR)
			{
				statusSensoren = KLIMASENSOR;
				if(reportStarted==false)
        {
          MyTimers[TIMER_REPORT].state = TM_START;
          reportStarted = true;
        }
			}
		}
		if( sendStatusReport )
    {
        char buffer[16];
        sendStatusReport = false;
        MyTimers[TIMER_REPORT].value = actReportBetweenSensors;
        MyTimers[TIMER_REPORT].state = TM_START;
        switch(statusReport)
        {
          case FIRSTREPORT:
          break;

#ifdef KLIMASENSOR
            case TEMPREPORT:
                LEDGRUEN_ON;
                sprintf(buffer,"%.1f",(double)fTemperatur);
                cnet.sendStandard(buffer,BROADCAST,'C','1','t','F');
            break;
            case HUMIREPORT:
                sprintf(buffer,"%.1f",(double)fHumidity);
                cnet.sendStandard(buffer,BROADCAST,'C','1','h','F');
            break;
            case ABSHUMIREPORT:
                sprintf(buffer,"%.1f",(double)fAbsHumitdity);
                cnet.sendStandard(buffer,BROADCAST,'C','1','a','F');
            break;
            case DEWPOINTREPORT:
                sprintf(buffer,"%.1f",(double)fDewPoint);
                cnet.sendStandard(buffer,BROADCAST,'C','1','d','F');
            break;
#endif // KLIMASENSOR
            case DS18B20REPORT:
              if(actNumberSensors>0)
              {
                sprintf(buffer,"%f",(double)(tempSensors[indexToReport]->getMeanTemperature()));
                cnet.sendStandard(buffer,BROADCAST,'T',int('a')+indexToReport,'t','F');
                indexToReport++;
              }
              else
                indexToReport=255;
              if(indexToReport>=actNumberSensors)
                indexToReport = 0;
              else
                statusReport--; // damit wird erreicht, dass der gleiche Report mit neuer Sensornummer abläuft
            break;
            case HEAT_ACTUAL_STATUS_REPORT:
              reportHeatActualStatus(&cnet,indexToReport);
              indexToReport++;
              if(indexToReport>=NUMBER_OF_VENTS)
                indexToReport = 0;
              else
                statusReport--; // damit wird erreicht, dass der gleiche Report mit neuer Ventilnummer abläuft
            break;
            case HEAT_SET_STATUS_REPORT:
              reportHeatSetStatus(&cnet,indexToReport);
              indexToReport++;
              if(indexToReport>=NUMBER_OF_VENTS)
                indexToReport = 0;
              else
                statusReport--; // damit wird erreicht, dass der gleiche Report mit neuer Ventilnummer abläuft
            break;
            case HEAT_SWELL_REPORT:
              cnet.broadcastDouble(fHeatSwell[indexToReport],'V','0'+indexToReport,'D');
              indexToReport++;
              if(indexToReport>=NUMBER_OF_VENTS)
                indexToReport = 0;
              else
                statusReport--; // damit wird erreicht, dass der gleiche Report mit neuer Ventilnummer abläuft
            break;
            case HEAT_NIGHT_SWELL_REPORT:
              cnet.broadcastDouble(fHeatNightSwell[indexToReport],'V','0'+indexToReport,'N');
              indexToReport++;
              if(indexToReport>=NUMBER_OF_VENTS)
                indexToReport = 0;
              else
                statusReport--; // damit wird erreicht, dass der gleiche Report mit neuer Ventilnummer abläuft
            break;
            case HEAT_HYSTERESE_REPORT:
              cnet.broadcastDouble(fHeatHysterese[indexToReport],'V','0'+indexToReport,'H');
              indexToReport++;
              if(indexToReport>=NUMBER_OF_VENTS)
                indexToReport = 0;
              else
                statusReport--; // damit wird erreicht, dass der gleiche Report mit neuer Ventilnummer abläuft
            break;
            case LASTREPORT:
                LEDGRUEN_OFF;
                MyTimers[TIMER_REPORT].value = actReportBetweenBlocks;
                MyTimers[TIMER_REPORT].state = TM_START;
            break;
        }
    }
/*
    if( (u8oldF1Swell      != u8F1Swell     ) |
        (u8oldF1Hysterese  != u8F1Hysterese ) |
        (u8oldF2Swell      != u8F2Swell     ) |
        (u8oldF2Hysterese  != u8F2Hysterese ) |
        (u8oldFanSetStatus != u8FanSetStatus)
      )
    {
      writeEEData();
      u8oldF1Swell      = u8F1Swell     ;
      u8oldF1Hysterese  = u8F1Hysterese ;
      u8oldF2Swell      = u8F2Swell     ;
      u8oldF2Hysterese  = u8F2Hysterese ;
      u8oldFanSetStatus = u8FanSetStatus;
    }*/
	}
}

uint8_t doLastSensor()
{
	switch( statusLastSensor )
	{
		case NOTHING_LAST_TODO:
			MyTimers[TIMER_SENSOREN].value = actWaitAfterLastSensor;
			MyTimers[TIMER_SENSOREN].state = TM_START;
			statusLastSensor = WAIT_LAST;
		break;
		case READY_LAST:
			statusLastSensor = NOTHING_LAST_TODO;
		break;
	}
	return statusLastSensor;
}

uint8_t doTemperature()
{
static uint8_t sensorToRead=0;
    switch(statusTemperature)
    {
        case NOTHING_TODO: // ca. 4.75ms
            tempSensors[0]->startConversion(true);
            MyTimers[TIMER_SENSOREN].value = 75;
            MyTimers[TIMER_SENSOREN].state = TM_START;
            statusTemperature = WAIT_TCONVERSION;
        break;
        case GET_TEMPERATURE:
            if(actNumberSensors>0)
            {
                int16_t temp;
                tempSensors[sensorToRead]->readTemperature(temp);
    //			ftemp = tempSensors[sensorToRead]->caliTemperature(tempSensors[sensorToRead]->getMeanTemperature() )*1000.0;
                sensorToRead++;
            }
            else
            {
                sensorToRead = 255;
            }
            if (sensorToRead>=actNumberSensors)
            {
                MyTimers[TIMER_SENSOREN].value = 25+measureRate_100ms-10;
                MyTimers[TIMER_SENSOREN].state = TM_START;
                statusTemperature = NOTHING_TODO;
                sensorToRead = 0;
            }
        break;
    }
    return(statusTemperature);
}


uint8_t doClima()
{
bool noError;

	switch(statusKlima)
	{
		case NOTHING_CLIMA_TODO:
			statusKlima = WAKEUP;
		break;
		case WAKEUP:
		  humiSensor.wakeup();
		  statusKlima = WAIT_WAKEUP;
      MyTimers[TIMER_SENSOREN].value = 2; //22
      MyTimers[TIMER_SENSOREN].state = TM_START;
    break;

		case START_CONVERSION: //
			noError=humiSensor.startMeasure();
			if (noError==true)
			{
				statusKlima = WAIT_CONVERSION;
				MyTimers[TIMER_SENSOREN].value = 2; //22
				MyTimers[TIMER_SENSOREN].state = TM_START;
			}
			else
				statusKlima = NOTHING_CLIMA_TODO;
		break;

		case READ_CONVERSION:  // Durchlaufzeit ca.
			noError = humiSensor.readResults();
			if (noError==true)
			{
				statusKlima = WAIT_READ;
				MyTimers[TIMER_SENSOREN].value = 2; //22
				MyTimers[TIMER_SENSOREN].state = TM_START;
			}
			else
				statusKlima = NOTHING_CLIMA_TODO;
		break;
		case CALC_CONVERSION0:  // Durchlaufzeit ca.
		  humiSensor.getResults(fTemperatur,fHumidity);
		  humiSensor.sleep();
			statusKlima = CALC_CONVERSION1;
		break;
		case CALC_CONVERSION1:  // Durchlaufzeit ca.
		  fAbsHumitdity =  humiSensor.calcAbsHumi(fTemperatur,fHumidity);
			statusKlima = CALC_CONVERSION2;
		break;
		case CALC_CONVERSION2:  // Durchlaufzeit ca.
		  fDewPoint =  humiSensor.calcDewPoint(fTemperatur,fHumidity);
			statusKlima = NOTHING_CLIMA_TODO;
		break;
	}
	return(statusKlima);
}

void setup_twi()
{
  char romBuf[40];
  bool last_dev = false;

	OneWireMaster::CmdResult result = owm.begin(&twiC_Master,0x18);
	cnet.sendInfo("Master Ready",BROADCAST);
	if(result != OneWireMaster::Success)
	{
        cnet.sendAlarm("Failed 1W Master",BROADCAST);
		while(1);
	}
	result = owm.OWReset();
	if(result == OneWireMaster::Success)
	{
		result = OWFirst(owm, searchState);
		if(result == OneWireMaster::Success)
		{
			uint8_t temp_index = 0;
			do
			{
				cnet.sendInfo("Search sensor: ",BROADCAST);
				last_dev = searchState.last_device_flag;
				if( (searchState.romId.familyCode() == 0x28) | (searchState.romId.familyCode() == 0x10))
				{
					if (actNumberSensors<NUMBER_OF_TEMPSENSORS)
					{
						tempSensors[actNumberSensors] = new TempSensor(selector,true,temp_index);
						temp_index++;
						tempSensors[actNumberSensors]->setRomID(searchState.romId);
						buffer_rom_id(romBuf,searchState.romId);
						cnet.sendInfo(romBuf,BROADCAST);
						actNumberSensors++;
					}
					else
					{
						cnet.sendWarning("Too much sensors",BROADCAST);
					}
				}
				result = OWNext(owm, searchState);
			}
			while( (result == OneWireMaster::Success) && (last_dev==false) );
		}
		else
		{
			//cnet.print("OWFirst failed with error code: ");
			//cnet.println(result, Serial::DEC);
		}
	}
	else
	{
		cnet.println("No 1-wire devices");
	}
	sprintf(romBuf,"No. Sensoren:%d", actNumberSensors);
	cnet.sendInfo(romBuf,BROADCAST);
}

//*********************************************************************
void buffer_rom_id(char *buffer,OneWire::RomId & romId)
{
	char temp[3];
	strcpy(buffer,"0x");
	for(uint8_t idx = 0; idx < RomId::byteLen; idx++)
	{
		sprintf(temp,"%x",romId[idx]);
		strcat(buffer,temp);
	}
}

void readEEData()
{/*
  for(uint8_t i=0;i<NUMBER_OF_VENTS;i++)
  {
    u8HeatSwell[i]      = eeprom_read_byte(&(ee_u8HeatSwell[i]));
    u8HeatHysterese[i]  = eeprom_read_byte(&(ee_u8HeatHysterese[i]));
    u8HeatSetStatus[i]  = eeprom_read_byte(&(ee_u8HeatSetStatus[i]));
    u8oldHeatSwell[i]      = u8HeatSwell[i]    ;
    u8oldHeatHysterese[i]  = u8HeatHysterese[i];
    u8oldHeatSetStatus[i]  = u8HeatSetStatus[i];
  }*/
}

void writeEEData()
{
  LEDGRUEN_ON;
	MyTimers[TIMER_SAVE_DELAY].state = TM_START; // Speicherverzögerung läuft los
}


