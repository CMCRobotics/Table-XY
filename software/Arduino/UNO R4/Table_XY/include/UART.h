/*
  Copyright (c) 2026 Gomez Costa J.L.

  This is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  It is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  If not, see <http://www.gnu.org/licenses/>.
*/

// Define standard libraries used by UART.
#include <Arduino.h>


// Define General libraries
#include <utilities.h>
#include <utilities_cpu.h>
#include "version.h"

// Definitions of Files used for UART access

void ISR_UART_Transmission(void);
void ISR_UART_Reception(void);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#ifdef MAIN_FILE

// Memories only defined on Main
    #define MAIN_FILE_UART

#else

    #define MAIN_FILE_UART extern

#endif
