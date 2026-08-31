/*
  Utilities functions related to the CPU Data Flash


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
1.0			27/06/2026		Gomez Costa		Created
1.2			27/06/2026		Gomez Costa		Eliminated default value in function declaration
2.0			27/07/2026		Gomez Costa		TEST_3 Collides with Main ISR for R4 GRBL

*/

#include "utilities_cpu.h"


//-----------------------------------------------------------------------------
// Initialises Data Flash functions
//
//
//	Return:
//		fsp_err_t Error Code
//			detail in fsp_common_api.h
//

fsp_err_t Flash_init(void) {

return R_FLASH_LP_Open(&DataFlash_access_ctrl, &DataFlash_access_cfg);

}

//-----------------------------------------------------------------------------
// Writes to Data Flash
//  Whole Block is erased then written
//  Data is 8 bits only
//  Based on FSP Blocking access
//
//	Debug signals:	if enabled, Set while
//		TEST_2: Erase, Write
//		TEST_3:	Blanck check, Status check
//
//	Parameters:
//		uint8_t *data_src
//			pointer to data to be written
//		boolean debug_timming
//			Set pin flags for checking the Time spent
//
//	Return:
//		fsp_err_t Error Code
//			detail in fsp_common_api.h
//

fsp_err_t writeDataFlash(uint8_t *data_src, boolean debug_timming){

	fsp_err_t	action_error;

	// Collides with Probe if(debug_timming)	TEST_2_SET;

	action_error = R_FLASH_LP_Erase (&DataFlash_access_ctrl,FLASH_DF_BLOCK_0, 1);

	if (FSP_SUCCESS != action_error){
		/* This will clear error flags and exit the P/E mode*/
		R_FLASH_LP_Reset(&DataFlash_access_ctrl);

		if(debug_timming){
			// Collides with Main ISR;
			TEST_3_CLR;
		}
		return action_error;
	}

	if(debug_timming){
		// Collides with Probe TEST_2_CLR;
		TEST_3_SET;
	}

	action_error =  R_FLASH_LP_BlankCheck (&DataFlash_access_ctrl, FLASH_DF_BLOCK_0, FLASH_DF_TRANSFER_LENGTH, &flash_result);
	if (FSP_SUCCESS != action_error){
		/* This will clear error flags and exit the P/E mode*/
		R_FLASH_LP_Reset(&DataFlash_access_ctrl);

		if(debug_timming){
			// Collides with Main ISR;
			TEST_3_CLR;
		}
		return action_error;
	}

	if(debug_timming){
		// Collides with Main ISR;
		TEST_3_CLR;
	}

	action_error =  R_FLASH_LP_Write (&DataFlash_access_ctrl, (uint32_t const) data_src, FLASH_DF_BLOCK_0, FLASH_DF_TRANSFER_LENGTH);
	if (FSP_SUCCESS != action_error){
		/* This will clear error flags and exit the P/E mode*/
		R_FLASH_LP_Reset(&DataFlash_access_ctrl);

		if(debug_timming){
			// Collides with Main ISR;
			TEST_3_CLR;
		}
		return action_error;
	}

	if(debug_timming){
		// Collides with Main ISR;
		TEST_3_SET;
	}

	action_error =  R_FLASH_LP_StatusGet (&DataFlash_access_ctrl, &flash_status);

	if (FSP_SUCCESS != action_error){
		/* This will clear error flags and exit the P/E mode*/
		R_FLASH_LP_Reset(&DataFlash_access_ctrl);

		if(debug_timming){
			// Collides with Main ISR;
			TEST_3_CLR;
		}
		return action_error;
	}

	if(debug_timming)	TEST_3_CLR; // Collides with Main ISR;

	return action_error;
}

//-----------------------------------------------------------------------------
// Writes a byte to Data Flash
//  Whole Block is erased then written
//  Data is 8 bits only
//  Based on FSP Blocking access
//
//	Debug signals:	if enabled, Set while
//		TEST_2: Erase, Write
//		TEST_3:	Blanck check, Status check
//
//	Parameters:
//		unsigned int addr
//			pointer to data position in Data Flash
//		uint8_t data_src
//			data to be written
//		boolean debug_timming
//			Set pin flags for checking the Time spent
//			default: false
//
//	Return:
//		fsp_err_t Error Code
//			detail in fsp_common_api.h
//

void eeprom_put_char(unsigned int addr, unsigned char new_value, boolean debug_timming){

  fsp_err_t return_error;

  // Read Full Block of Data Flash
  readDataFlash(flash_buffer_memory, false);

  // Update desired value
  flash_buffer_memory[addr] = new_value;

  // Write Full Block of Data Flash
  return_error = writeDataFlash(flash_buffer_memory, false);

  if (return_error != FSP_SUCCESS){
    displayFlashError("Write", return_error);
    //Program will stay here
  };
}

//-----------------------------------------------------------------------------
// Reads from Data Flash
//  Whole Block is read
//  Data is 8 bits only
//  Based on FSP Blocking access
//
//	Debug signals:	if enabled, Set while
//		TEST_2: Read
//
//	Parameters:
//		uint8_t *data_src
//			pointer to storage of data
//		boolean debug_timming
//			Set pin flags for checking the Time spent
//
//	Return:
//		none
//

void readDataFlash(uint8_t *data_src, boolean debug_timming){

	u_int32_t idx;
	u_int8_t * pointer;

	// Collides with Probe if(debug_timming)	TEST_2_SET;

	for (idx = 0; idx < FLASH_DF_TRANSFER_LENGTH ; idx ++){
		// Read data
		pointer = (u_int8_t * )((u_int32_t *)FLASH_DF_BLOCK_0) + (idx);
		*(data_src + idx) = *(u_int8_t *)(pointer);
	  }

	// Collides with Probe if(debug_timming)	TEST_2_CLR;
}


//-----------------------------------------------------------------------------
// Reads one byte from Data Flash
//  Data is 8 bits only
//  Based on FSP Blocking access
//
//	Debug signals:	if enabled, Set while
//		TEST_2: Read
//
//	Parameters:
//		unsigned int addr
//			pointer to storage of data
//		boolean debug_timming
//			Set pin flags for checking the Time spent
//
//	Return:
//		uint8_t read byte
//

uint8_t eeprom_get_char(unsigned int addr, boolean debug_timming){

	u_int8_t regvalue;
	u_int8_t * pointer;

	// Collides with Probe if(debug_timming)	TEST_2_SET;

	// Read data
	pointer = (u_int8_t * )((u_int32_t *)FLASH_DF_BLOCK_0) + (addr);
	regvalue = *(u_int8_t *)(pointer);

	// Collides with Probe if(debug_timming)	TEST_2_CLR;

	return regvalue;
}
