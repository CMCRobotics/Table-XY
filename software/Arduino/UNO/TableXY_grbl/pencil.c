/*
  gcode.c - rs274/ngc parser.
  Part of Grbl

  Copyright (c) 2011-2015 Sungeun K. Jeon
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

#include "grbl.h"

/************************
Parses the Gcode line.

  When Z command is detected:
    - Mxx command is executed
    - then the original command line is executed unmodified.

  Process: 
    1.0 Keep OriginalLine unmodified
    2.0 Command Line is parsed:	
      2.1 if no Z command is found, jump to 3.0
      2.2 if Z command is found: Generate a Dummy CommandLine with the M03 or M05
      2.3 Execute Dummy CommandLine
      2.3.1 If error detected, jump in 4.0
    3.0 Execute OriginalLine
    4.0 Return with:
      detected error
      or OK if none

Parses one line of 0-terminated G-Code. The line is assumed to contain only uppercase
characters and signed floating point values (no whitespace). Comments and block delete
characters have been removed. 

******************************/
char gline_pencil_up[] = ACTION_PENCIL_UP;
char gline_pencil_down[] = ACTION_PENCIL_DOWN;

uint8_t gc_execute_line(char *line) 
{

  uint8_t char_counter = 0;  
  char letter;
  float value;
  char *gline_pencil;

  uint8_t execution_status;


  while (line[char_counter] != 0) { // Loop until no more g-code words in line.
    
    // Import the next g-code word, expecting a letter followed by a value. Otherwise, error out.
    letter = line[char_counter];
    
    if(letter == 'Z') { 
      //It is Zaxis movement
      char_counter++;
      
      // Get the Floating point value, or error if not present
      if (!read_float(line, &char_counter, &value)) { return(STATUS_BAD_NUMBER_FORMAT); } // [Expected word value]

      // Assumes it is Pencil Down
      gline_pencil = &gline_pencil_down;

      if(value >= Z_AXIS_LIMIT){
        // It is Pencil Up
        gline_pencil = &gline_pencil_up;
      }
      
      #ifdef DebugEnabled
        printString("Original: ");
        printString(line);
        printString("\r\nModified: ");        
        printString(gline_pencil);
        printString("\r\n");           
      #endif


      // Move Pencil
      execution_status = gc_execute_line_actual(gline_pencil);
      if(execution_status != STATUS_OK) { return execution_status;}     
    }

    char_counter++;
  
  }

  // execute initial command line
  execution_status = gc_execute_line_actual(line);
  if(execution_status != STATUS_OK) { return execution_status;} 

  // TODO: % to denote start of program.
  return(STATUS_OK);
}
        
