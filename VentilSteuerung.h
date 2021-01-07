
#ifndef LUEFTERSTEUERUNG_H_
#define LUEFTERSTEUERUNG_H_

#include <avr/io.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "myconstants.h"

#include "Serial.h"
#include "External.h"
#include "timer.h"
#include "cmultiStandardCommands.h"
#include "ComReceiver.h"
#include "CommandFunctions.h"
#include "Communication.h"
#include "xmegaClocks.h"

#include "Masters/Masters.h"
#include "Slaves/Slaves.h"
#include "RomId/RomCommands.h"
#include "RomId/RomId.h"
#include "Masters/DS248x/DS2484/DS2484.h"
#include "twi_master_driver.h"
#include "max7328.h"
#include "LuefterHardware.h"
#include "shtc3.h"
#include "ledHardware.h"

using namespace OneWire;

uint8_t doLastSensor();
uint8_t doClima();
void setup_twi();
void readEEData();
void writeEEData();

#endif /* LUEFTERSTEUERUNG_H_ */
