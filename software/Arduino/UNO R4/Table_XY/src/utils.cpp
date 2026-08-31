/*
  // sUtilities

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

// Define standard libraries used

#include <math.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

//===============================================================
// Prints an uint8 variable with base 16 and 2 hexadecimal digits.

void print_int8_base16(uint8_t n)
{
  unsigned char buf[2];
  unsigned char buf_string[2 + 1];

  int8_t i;

  buf[1] = (n / 16);
  buf[0] = n % 16 ;

  for (i = 1; i >= 0; i--){
    if(buf[i] > 9){
      // It is Letter A-F
      buf_string[1-i] = 'A' + buf[i] - 10;
    }else{
      // It is digit 0-9
      buf_string[1-i] = '0' + buf[i];
    }
  }

  buf_string[2] = 0;   // Make a string
  printString( (char *) buf_string);
}

//===============================================================
// Prints an integer variable with base 16 and selected hexadecimal digits.
//
// Parameters:
//  int number_int
//    number to be converted
//
//  uint8_t number_of_hexa
//    Number of hexadecimal characters
//
//  char *format_string
//    format for the string
//    example: "%08X"

void print_uint32_base16(uint32_t number_int, char *format_string,  uint8_t number_of_hexadigits)
{
  char hexadecimal[number_of_hexadigits + 1];

  sprintf(hexadecimal,format_string, number_int);

  printString( (char *) hexadecimal);
}


void print_uint8_base16(uint8_t number_int, char *format_string,  uint8_t number_of_hexadigits)
{
  char hexadecimal[number_of_hexadigits + 1];

  sprintf(hexadecimal,format_string, number_int);

  printString( (char *) hexadecimal);
}