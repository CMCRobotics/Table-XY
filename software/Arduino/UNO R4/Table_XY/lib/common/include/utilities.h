/*
  General Utilities

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


// IMPORTANT: Any changes here requires a full re-compiling of the source code to propagate them.

#include <Arduino.h>

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

#ifndef utilities_h
#define utilities_h

// Prints menu / Get selected menu
struct MenuValues {
	char menu_text[80];
	void (*ptr_function)(void);
};
extern MenuValues MenuList[];

#define MENULIST_QTY (sizeof(MenuList) / sizeof(MenuList[0]))

void functionMenu(u_int8_t menu_qty);

// Display in Matrix LED a string
void displayMatrixLED(char *display_string, u_int8_t number_times);
#define MatrixLEDScrollSpeed  (40)    // The lower the faster

// Converts a string from the Serial Console to a defined int/float
int16_t SerialInput_conversion(u_int32_t *value);
int16_t SerialInput_conversion(int16_t *value);
int16_t SerialInput_conversion(float *value);
int16_t SerialInput_conversion(char *value);

// Prints in hexadecimal format
void print_uint32_base16(uint32_t number_int, char *format_string,  uint8_t number_of_hexadigits);
void print_uint8_base16(uint8_t number_int, char *format_string,  uint8_t number_of_hexadigits);

#endif
