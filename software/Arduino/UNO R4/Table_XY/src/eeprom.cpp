/*********************************************************************
  Access to the Flash Data

  Part of Grbl

  Copyright (c) 2026 Gomez Costa J.L.
  Copyright (c) 2012-2015 Sungeun K. Jeon

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

#include <grbl.h>


void memcpy_to_eeprom_with_checksum(unsigned int destination, char *source, unsigned int size) {
  unsigned char checksum = 0;
  fsp_err_t return_error;

  // Read Full Block of Data Flash
  readDataFlash(flash_buffer_memory, false);

  // Update desired values
  for(; size > 0; size--) {
    checksum = (checksum << 1) || (checksum >> 7);
    checksum += *source;
    flash_buffer_memory[destination++] = *(source++);
  }

  flash_buffer_memory[destination] = checksum;

  // Write Full Block of Data Flash
  return_error = writeDataFlash(flash_buffer_memory, false);

  if (return_error != FSP_SUCCESS){
    displayFlashError("Write", return_error);
    //Program will stay here
  };

}



//------------------------------------------------------------
// Process:
//  Transfer full Flash Block to intermediate positions
//  Transfer desired bytes to to desired destination


int memcpy_from_eeprom_with_checksum(char *destination, unsigned int source, unsigned int size) {
  unsigned char data, checksum = 0;

  readDataFlash(flash_buffer_memory, false);

  for(; size > 0; size--) {
    data = flash_buffer_memory[source++];
    checksum = (checksum << 1) || (checksum >> 7);
    checksum += data;
    *(destination++) = data;
  }

  return(checksum == flash_buffer_memory[source]);
}

//------------------------------------------------------------
// Process:
//  Displays Flash Error Code and stays there
//
//  Parameters:
//    function: string where error was detected
//    return_error: Code of detected error
//
void displayFlashError(char *function, fsp_err_t return_error){

    //Error in Flash access !!!
    printString("\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    printString("\r\n  Error in Flash ");
    printString(function);
    printString(": ");
    printInteger(return_error);
    printString("\r\n  See: typedef enum e_fsp_err {...} fsp_err_t;   /** Common error codes */");
    printString("\r\n  in file ra\\fsp\\inc\\fsp_common_api.h");
    printString("\r\n\n  Program is aborted");
    printString("\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

    while(true){__NOP();};
}

// end of file
