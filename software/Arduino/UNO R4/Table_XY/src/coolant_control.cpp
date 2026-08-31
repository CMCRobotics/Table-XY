/*
  coolant_control.c - coolant control methods
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

#include "grbl.h"

#ifdef DEBUG_TEST_1_TEST_2

// Coolant and Probe are NOT USED
void coolant_init() {__NOP();};
void coolant_stop() {__NOP();};
void coolant_set_state(uint8_t mode) {__NOP();};
void coolant_run(uint8_t mode) {__NOP();};

#else

void coolant_init()
{
  // Output pin
  PFS_WRITE_ENABLE;

  R_PMISC->PWPR_b.B0WI = 1;
  R_PMISC->PWPR_b.PFSWE = 1;

  // Init all bits to 0, standard setting

  R_PFS->PORT[COOLANT_CPU_PORT].PIN[COOLANT_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[COOLANT_CPU_PORT].PIN[COOLANT_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

  #ifdef ENABLE_M7
    R_PFS->PORT[COOLANT_MIST_CPU_PORT].PIN[COOLANT_MIST_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
    R_PFS->PORT[COOLANT_MIST_CPU_PORT].PIN[COOLANT_MIST_CPU_BIT].PmnPFS_b.PDR = 1;  //Output
  #endif

  PFS_WRITE_DISABLE;

  coolant_stop();
}

//-------------------------------------------------------------------------
void coolant_stop()
{
  COOLANT_CLR;
  #ifdef ENABLE_M7
    COOLANT_MIST_CLR;
  #endif
}

//-------------------------------------------------------------------------
void coolant_set_state(uint8_t mode)
{
  if (mode == COOLANT_FLOOD_ENABLE) {
    COOLANT_SET;

  #ifdef ENABLE_M7
    } else if (mode == COOLANT_MIST_ENABLE) {
      COOLANT_MIST_SET;
  #endif

  } else {
    coolant_stop();
  }
}

//-------------------------------------------------------------------------
void coolant_run(uint8_t mode)
{
  if (sys.state == STATE_CHECK_MODE) { return; }
  protocol_buffer_synchronize(); // Ensure coolant turns on when specified in program.
  coolant_set_state(mode);
}

#endif