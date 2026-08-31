/*
  flash.h - Definition of variables for Flash Access
 
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
#include "hal_data.h"
#include "r_flash_lp_cfg.h"

#ifndef flash_h
#define flash_h

// Flash related actions
fsp_err_t Flash_init(void);
fsp_err_t writeDataFlash(uint8_t *data_src, boolean debug_timming);
void readDataFlash(uint8_t *data_src, boolean debug_timming);

uint8_t eeprom_get_char(unsigned int addr, boolean debug_timming);
void eeprom_put_char(unsigned int addr, unsigned char new_value, boolean debug_timming);

void displayFlashError(char *function, fsp_err_t return_error);

#define FLASH_DF_BLOCK_0            (0x40100000UL) /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_DF_TRANSFER_LENGTH    (1024)
#define FLASH_DATA_BLOCK_SIZE       (FLASH_DATA_BLOCK_SIZE)
#define FLASH_WRITE_SIZE            FLASH_DATA_BLOCK_SIZE



#ifdef MAIN_FILE

// Memories only defined on Main
    #define FLASH_MAIN_FILE_DEF

#else

    #define FLASH_MAIN_FILE_DEF extern

#endif

FLASH_MAIN_FILE_DEF uint8_t        flash_buffer_memory[FLASH_DF_TRANSFER_LENGTH];
FLASH_MAIN_FILE_DEF flash_result_t flash_result;
FLASH_MAIN_FILE_DEF flash_status_t flash_status;

#endif
