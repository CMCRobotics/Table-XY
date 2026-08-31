/*
  grbl.h - main Grbl include file
  Part of Grbl
    GRBL for Arduino UNO R4

  Based on:
    GRBL 0.9i
      GRBL_VERSION "0.9i"
      GRBL_VERSION_BUILD "20150620"

  Copyright (c) 2026 Gomez Costa J.L.
  Copyright (c) 2015 Sungeun K. Jeon
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef grbl_h
#define grbl_h

// Serial Port test Messages

// Comment out for disabling Debug messages
#define DebugEnabled Yes

/**********************
Example
      #ifdef DebugEnabled
        printString("Original: ");
        printString(line);
        printString("\r\nModified: ");
        printString(gline_pencil);
        printString("\r\n");
      #endif
****************************/


// Grbl versioning system
#define GRBL_VERSION "0.9i"
#define GRBL_VERSION_BUILD "20150620"


// Define standard libraries used by Grbl.
#include <Arduino.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// #include <util/delay.h>


// Define General libraries
#include <utilities.h>
#include <utilities_cpu.h>
#include "version.h"
#include "utils.h"

#include "UART.h"
#include "debug_cpu_r7fa4m1ab.h"

// Define the Grbl system include files. NOTE: Do not alter organization.
#include "cpu_map/cpu_map_r7fa4m1ab.h"
#include "TestPins_1_2_debug.h"

#include "config.h"
#include "pencil.h"
#include "nuts_bolts.h"
#include "settings.h"
#include "system.h"
#include "defaults.h"
#include "cpu_map.h"
#include "coolant_control.h"
#include "eeprom.h"
#include "gcode.h"
#include "limits.h"
#include "motion_control.h"
#include "planner.h"
#include "print.h"
#include "probe.h"
#include "protocol.h"
#include "report.h"
#include "serial.h"
#include "spindle_control.h"
#include "stepper.h"

#endif


#ifdef ENABLE_M7 // Mist coolant disabled by default. See config.h to enable/disable.

// This bit collieds with TEST-1
// Define Coolant MIST Output bit

#define COOLANT_MIST_CPU_PORT  1
#define COOLANT_MIST_CPU_BIT   1 // Arduino Uno Analog pin A4

#define COOLANT_MIST_SET  (R_PORT1->PODR_b.PODR1 = 1)
#define COOLANT_MIST_CLR  (R_PORT1->PODR_b.PODR1 = 0)

#endif