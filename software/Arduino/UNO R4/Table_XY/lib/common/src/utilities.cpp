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


/*
Version		Date			Author			Description
1.0			19/05/2026		Gomez Costa		Created
2.0			28/05/2026		Gomez Costa		Added getPrintMenu
3.0			29/05/2026		Gomez Costa		Added functionMenu, Deleted getPrintMenu, Added displayRevisionMatrixLED
4.0			03/06/2026		Gomez Costa		Added uint32_t configInterrupt(void (*pfunc)(),  u_int32_t irq_number, boolean print_true = false )
5.0			04/06/2026		Gomez Costa		Added int16_t SerialInput_conversion(char *value)
5.5			04/06/2026		Gomez Costa		SerialInput_conversion(xxx): BACKSPACE clears all input
6.0			04/06/2026		Gomez Costa		Transferred CPU related functions to utilities_cpu.*
6.5			10/06/2026		Gomez Costa		Corrected:
												- SerialInput_conversion(char *value): CR input is discarded
												- int16_t SerialInput_conversion(intXX): Zeros are not skipped at left
7.0			20/06/2026		Gomez Costa		Modified:
												- void displayRevisionMatrixLED: Added parameter number of times to show
7.2			20/06/2026		Gomez Costa		Modified:
												- void displayRevisionMatrixLED: check for number_times == 0
7.4			07/07/2026		Gomez Costa		Modified:
												- void displayRevisionMatrixLED: incremented display speed
8.0			08/07/2026		Gomez Costa		Modified:
												- function name/pourpose: displayRevisionMatrixLED(char *revision, u_int8_t number_times) to displayMatrixLED(char *display_string, u_int8_t number_times);

*/

#include "utilities.h"

// Define standard libraries used
#include <math.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//===============================================================
// Display in Matrix LED a text
//
// Parameters:
//	char *display_string
//		pointer to string
//	u_int8_t number_times
//		Number of times to execute: [0 .. n]
//

ArduinoLEDMatrix matrixLED;

void displayMatrixLED(char *display_string, u_int8_t number_times) {
	char banner[100];
	u_int8_t idx;

	if(number_times == 0) return;

	strcpy(banner, display_string);

	matrixLED.begin();
	matrixLED.beginDraw();
	matrixLED.stroke(0xFFFFFFFF);
	matrixLED.textScrollSpeed(MatrixLEDScrollSpeed);
	matrixLED.textFont(Font_5x7);

	for(idx=0; idx < number_times; idx++){
		// Matrix access
		matrixLED.beginText(0, 1, 0xFFFFFF);
		matrixLED.println(banner);
		matrixLED.endText(SCROLL_LEFT);
	}
	matrixLED.endDraw();
}

//===============================================================
// Execute a Menu of functions
//
// 	MenuValues MenuList[]: List of pairs: Menu text/Menu function
//	menu_qty: number of Menus

void functionMenu(u_int8_t menu_qty){

int16_t menu_val, result;
u_int8_t idx;


while (true) {
	Serial.println("\n\n\n\n");
	Serial.println("--------------------------------------------------------------");
	Serial.println("---- MENU ----------------------------------------------------");
	Serial.println("");

	for(idx = 0; idx < menu_qty; idx++){
		Serial.print(" - ");	Serial.print(idx);	Serial.print(" - ");
		Serial.println(MenuList[idx].menu_text);
	}

	Serial.print("\n  Selected index: ");

	result = SerialInput_conversion(&menu_val);

	if ((result == -1) || (menu_val < 0) || (menu_val > (menu_qty - 1)) ){
		// Error
		Serial.println("\n-----------------------------");
		Serial.println(  "Error in Menu index");

		continue;
		}
		else{
			// Correct input, execute
			// Serial.print(" - ");	Serial.println(menu_val);
			MenuList[menu_val].ptr_function();
		}

};

}

//===============================================================
// Prints an uint8 variable with base 16 and 2 digits.

void print_int8_base16_2(uint8_t number)
{
  unsigned char buf[2];
  int8_t i;

  buf[1] = (number / 16);
  buf[0] = number % 16 ;

  for (i = 1; i >= 0; i--){
    if(buf[i] > 9){
      // It is Letter A-F
      Serial.print('A' + buf[i] - 10);
    }else{
      // It is digit 0-9
      Serial.print('0' + buf[i]);
    }
  }
}

//-----------------------------------------------------------------------------
/*
  Input a string from Serial Console, convert to unsigned int
  range: unsigned int 32bits

  Accepted characters: [digits +]
  Other characters are discarged
  Space is skipped if before any digit is entered
  Input ends with NewLine
  ESC returns without error or value Change

  Parameters:
	value = Input value

	return =
		-2: ESC detected
			value = not modified
		-1: Error:
				any character not in the range [digits +]
			value = undefined
		 0: conversion is correct
		 	value = converted value

*/

int16_t SerialInput_conversion(u_int32_t *value)
{
bool digit = false;
bool error = false;
int inChar;

String inString = "";  	// string to hold input

while (true) {

	while (Serial.available() > 0) {
		inChar = Serial.read();

		switch((char)inChar){
			case '0':
				inString += (char)inChar;
				Serial.print((char)inChar);
				digit = true;
				break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// It is a Digit 1 to 9
				// convert the incoming byte to a char and add it to the string:
				inString += (char)inChar;
				Serial.print((char)inChar);
				digit = true;
				break;

			case 27:
				// ESC
				return -2;

				break;

			case 8:
				// Backspace

				if(!inString.isEmpty()){
					// Not at start
					inString.remove(inString.length()-1, 1);		// Eliminate last character
					Serial.print("\b  \b\b");
					if(inString.isEmpty())	digit = false;			// No more digits agreed
				}

				break;

			case '\n':
				// newline, end input
				if (error || !digit){
					// Any error detected in the characters, or no number entered
					return -1;
				}

				// All is OK
				*value = u_int32_t (inString.toDouble());
				return 0;
				break;

			default:
				// It is any other character do nothing
				break;
		}
	}
}

}


//-----------------------------------------------------------------------------
/*
Input a string from Serial Console, convert to signed int
range: signed int 32bits

Accepted characters: [digits + -]
Other characters are discarged
Space is skipped if before any digit is entered
Input ends with NewLine
ESC returns without error or value Change

Parameters:
  value = Input value

  return =
	  -2: ESC detected
		  value = not modified
	  -1: Error:
		  any character not in the range [digits + -]
		  value = undefined
	   0: conversion is correct
		   value = converted value

*/

int16_t SerialInput_conversion(int32_t *value)
{
bool digit = false;
bool error = false;
bool sign = false;
int inChar;

String inString = "";  	// string to hold input

while (true) {

	while (Serial.available() > 0) {
		inChar = Serial.read();

		switch((char)inChar){
			case '-':
			case '+':
				if (digit || sign){
					// Any digit already entered -> Error, Sign not allowed between digits
					error = true;
				}else{
					sign = true;
					inString += (char)inChar;
					Serial.print((char)inChar);
				}
				break;

			case '0':
				inString += (char)inChar;
				Serial.print((char)inChar);
				digit = true;
				break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// It is a Digit 1 to 9
				// convert the incoming byte to a char and add it to the string:
				inString += (char)inChar;
				Serial.print((char)inChar);
				digit = true;
				break;

			case 27:
				// ESC
				return -2;

				break;

			case 8:
				// Backspace

				if(!inString.isEmpty()){
					// Not at start
					inString.remove(inString.length()-1, 1);		// Eliminate last character
					Serial.print("\b  \b\b");
					if(inString.isEmpty())	digit = false;			// No more digits agreed
				}

				break;

			case '\n':
				// newline, end input
				if (error || !digit){
					// Any error detected in the characters, or no number entered
					return -1;
				}

				// All is OK
				*value = inString.toInt();
				return 0;
				break;

			default:
				// It is any other character do nothing
				break;
		}
	}
}

}


//-----------------------------------------------------------------------------
/*
Input a string from Serial Console, convert to signed int
range: signed int 16bits

Accepted characters: [digits + -]
Other characters are discarged
Space is skipped if before any digit is entered
Input ends with NewLine
ESC returns without error or value Change

Parameters:
  value = Input value

  return =
	  -2: ESC detected
		  value = not modified
	  -1: Error:
		  any character not in the range [digits + -]
		  value = undefined
	   0: conversion is correct
		   value = converted value

*/

int16_t SerialInput_conversion( int16_t *value)
{
bool digit = false;
bool error = false;
bool sign = false;
int inChar;

int32_t testval;

String inString = "";  	// string to hold input

while (true) {

	while (Serial.available() > 0) {
		inChar = Serial.read();

		switch((char)inChar){
			case '-':
			case '+':
				if (digit || sign){
					// Any digit already entered -> Error, Sign not allowed between digits
					error = true;
				}else{
					sign = true;
					inString += (char)inChar;
					Serial.print((char)inChar);
				}
				break;

			case '0':
				// Clip zeros at left
				inString += (char)inChar;
				Serial.print((char)inChar);
				digit = true;
				break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// It is a Digit 1 to 9
				// convert the incoming byte to a char and add it to the string:
				inString += (char)inChar;
				Serial.print((char)inChar);
				digit = true;
				break;

			case 27:
				// ESC
				return -2;

				break;

			case 8:
				// Backspace

				if(!inString.isEmpty()){
					// Not at start
					inString.remove(inString.length()-1, 1);		// Eliminate last character
					Serial.print("\b  \b\b");
					if(inString.isEmpty())	digit = false;			// No more digits agreed
				}

				break;

			case '\n':
				// newline, end input
				if (error || !digit){
					// Any error detected in the characters, or no number entered
					return -1;
				}

				// All is OK
				testval = inString.toInt();
				if ((testval > 32767) || (testval < -32768)){
					// Error
					return -1;
				}

				*value = (int16_t)(inString.toInt());
				return 0;
				break;

			default:
				// It is any other character do nothing
				break;

		}
	}
}

}

//-----------------------------------------------------------------------------
/*
  Input an alpha char from Serial Console, Convert to upper
  range: [a-z]

  Accepted characters: [a-z]
  Space is skipped if before any digit is entered
  Input ends with NewLine
  ESC returns without error or Change

  Parameters:
	value = Input value in char format
	return =
		-2: ESC detected
		-1: Error:
			any character not in the range
			more that one character
			value = undefined
		 0: conversion is correct
		 	value = converted value

*/

int16_t SerialInput_conversion(char *value)
{
bool digit = false;
bool error = false;
char inChar;
String inString = "";  	// string to hold input

while (true) {

	while (Serial.available() > 0) {
		inChar = Serial.read();

		switch(inChar){
			case 27:
				// ESC
				return -2;

				break;

			case 8:
				// Backspace

				if(!inString.isEmpty()){
					// Not at start
					inString.remove(inString.length()-1, 1);		// Eliminate last character
					Serial.print("\b  \b\b");
					if(inString.isEmpty())	digit = false;			// No more digits agreed
				}

				break;

			case '\n':
				// newline, end input
				if (error || !digit){
					// Any error detected in the characters, or no number entered
					return -1;
				}

				// All is OK
				*value = inString[0];
				return 0;

				break;

			case 0x0D:
				// CR, do nothing
				break;

			default:
				if(isAlpha((int)inChar)){
					// It is alphanumeric
					inString += (char)inChar;
					Serial.print(inChar);
					digit = true;
					break;
				}else{
					error = true;
				}
				break;
		}
	}
}

}


//-----------------------------------------------------------------------------
/*
  Input a string from Serial Console, convert to float

  Arduino Float accepts only 6 characters in total

  Accepted characters: [all digits + - . e E]
  Other characters are discarged
  Space is skipped if before any digit is entered
  Zeros at left are skipped.
  Input ends with NewLine
  ESC returns without error or value Change

	Parameters:
	float_value = Input value in float

	return =
		-2: ESC detected
			value = not modified
		-1: Error:
			any character not in the range [all digits + - . e E]
			value = undefined
		0: conversion is correct
			value = converted value

*/
int16_t SerialInput_conversion(float *float_value)
{
bool exponent = false;
bool digit = false;
bool comma = false;
bool error = false;

int inChar;

String inString = "";  	// string to hold input


while (true) {

	while (Serial.available() > 0) {
		inChar = Serial.read();

		switch((char)inChar){
			case '-':
			case '+':
				// It is sign
				// Accepted at first position, or after E/e

				if (digit && !exponent){
					// Any digit already entered -> Error, Sign not allowed between digits
					error = true;
				}else{
					// First symbol or after Exponent
					inString += (char)inChar;
					Serial.print((char)inChar);
				}
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// It is a Digit 1 to 9
				// convert the incoming byte to a char and add it to the string
				inString += (char)inChar;
				Serial.print((char)inChar);
				digit = true;
				break;

			case '.':
				// It is a .
				// convert the incoming byte to a char and add it to the string
				if (comma){
					// A comma already entered -> Error
					error = true;
				}else{
					// First time the point
					inString += (char)inChar;
					Serial.print((char)inChar);
					comma = true;
				}
				break;

			case 'e':
			case 'E':
				// It is e or E
				// convert the incoming byte to a char and add it to the string
				if(!exponent){
					inString += (char)inChar;
					Serial.print((char)inChar);
					exponent = true;
				}
				break;

			case 27:
				// ESC
				return -2;

				break;

			case 8:
				// Backspace

				if(!inString.isEmpty()){
					// Not at start
					inString.remove(inString.length()-1, 1);		// Eliminate last character
					Serial.print("\b  \b\b");
					if(inString.isEmpty())	digit = false;			// No more digits agreed
				}

				break;

			case '\n':
				// newline, end input
				if (error || !digit){
					// Any error detected in the characters, or no number entered
					return -1;
				}

				// All is OK
				*float_value = inString.toFloat();
				return 0;
				break;

			default:
				// It is any other character do nothing
				break;
		}
	}
}

}
