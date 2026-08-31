/*
Utilities focused on Renesas RA4M1

  Copyright (c) 2026 Gomez Costa J.L.

  This is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "cpu_r7fa4m1ab.h"
#include "flash.h"

#include <string.h>

#ifndef utilities_cpu_h
#define utilities_cpu_h

// Stores the Interrupt ISR address/Priority in the Interrupt Vector Table, enables Interruption
uint32_t configInterrupt(void (*pfunc)(),  u_int32_t irq_number, uint32_t irq_priority, uint32_t irq_event_code,boolean print_true );

// Get the first free place of IELSRn
u_int8_t getIdxIELSRn(boolean print_true);

// Prints desired CPU Registers
void printCPURegisterValues(u_int32_t *reg_ptr, int number, char *text);
void printCPURegisterValues(u_int16_t *reg_ptr, int number, char *text);
void printCPURegisterValues(u_int8_t *reg_ptr, int number, char *text);

// Test pins
void debugPinOutputTest_1_init(void);
void debugPinOutputTest_2_init(void);
void debugPinOutputTest_3_init(void);

// Inline function for Enabling TXI interruption
void EnableTXIinterruption (void);


// Delay functions
static void _delay_us_cpu(uint32_t us) PLACE_IN_RAM_SECTION __attribute__((noinline));
void _delay_ms(unsigned int);
void _delay_us(unsigned int);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Define Probe test signals

#define TEST_1_CPU_PORT   1
#define TEST_1_CPU_BIT    1 // Arduino Uno R4 Analog Pin A4

#define TEST_1_SET  (R_PORT1->PODR_b.PODR1 = 1) // Debug: Used to time ISR
#define TEST_1_CLR  (R_PORT1->PODR_b.PODR1 = 0)
#define TEST_1_TOGGLE   ((R_PORT1->PIDR_b.PIDR1)? TEST_1_CLR: TEST_1_SET)


#define TEST_2_CPU_PORT   1
#define TEST_2_CPU_BIT    0 // Arduino Uno R4 Analog Pin A5

#define TEST_2_SET  (R_PORT1->PODR_b.PODR0 = 1) // Debug
#define TEST_2_CLR  (R_PORT1->PODR_b.PODR0 = 0)
#define TEST_2_TOGGLE   ((R_PORT1->PIDR_b.PIDR0)? TEST_2_CLR: TEST_2_SET)


#define TEST_3_CPU_PORT   1
#define TEST_3_CPU_BIT    2 // Arduino Uno R4 Pin D13

#define TEST_3_SET  (R_PORT1->PODR_b.PODR2 = 1) // Debug
#define TEST_3_CLR  (R_PORT1->PODR_b.PODR2 = 0)
#define TEST_3_TOGGLE   ((R_PORT1->PIDR_b.PIDR2)? TEST_3_CLR: TEST_3_SET)

#endif
