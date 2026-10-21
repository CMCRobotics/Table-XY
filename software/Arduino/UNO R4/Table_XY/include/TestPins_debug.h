/*
TestPins_debug.h - Definition of variables for enabling Debug using TEST_0/1/2 pins

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

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
/*
TEST 0: A3
TEST 1: A4
TEST 2: A5
TEST 3: D13

*/

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Definitions for using Test0[1/2] pins
//  If Not defined
//    NORMAL
//    	TEST0[1/2] pin is NOT used
//    	COOLANT ENABLE [COOLANT / PROBE] pin IS defined
//
//  If defined
//    TEST_xx
//    	TEST0[1/2] pin IS used
//    	COOLANT ENABLE [COOLANT / PROBE]pin IS NOT defined
//

#include <Arduino.h>

// #define DEBUG_TEST_0 YES		// comment out for standard operation
// #define DEBUG_TEST_1 YES		// comment out for standard operation
// #define DEBUG_TEST_2 YES		// comment out for standard operation

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#ifdef MAIN_FILE

// Memories only defined on Main
    #define MAIN_FILE_DEBUG volatile

#else

    #define MAIN_FILE_DEBUG extern volatile

#endif

MAIN_FILE_DEBUG uint8_t  debug_uint8_1;
MAIN_FILE_DEBUG uint16_t  debug_uint16_1;
MAIN_FILE_DEBUG uint32_t  debug_uint32_1;

//----------------------------------------
// Test & Debug Memories
MAIN_FILE_DEBUG uint8_t debug_mode;
MAIN_FILE_DEBUG uint8_t debug_idx;
#define DEBUG_VALUE  (5)

