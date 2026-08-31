/*
  probe.c - code pertaining to probing methods
  Part of Grbl

  Copyright (c) 2026 	  Gomez Costa J.L.
  Copyright (c) 2014-2015 Sungeun K. Jeon

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


// Inverts the probe pin state depending on user settings and probing cycle mode.
uint8_t probe_invert_mask;


#ifdef DEBUG_TEST_1_TEST_2

// Coolant and Probe are NOT USED
void probe_init() {__NOP();}
void probe_configure_invert_mask() {__NOP();}
uint8_t probe_get_state() {return 0;}
void probe_state_monitor() {__NOP();}
void probe_configure_invert_mask(uint8_t is_probe_away) {__NOP();}

#else

// Probe pin initialization routine.
void probe_init()
{
    // Input pin
  PFS_WRITE_ENABLE;

  // Define Bit input
  R_PFS->PORT[PROBE_CPU_PORT].PIN[PROBE_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting


  #ifdef DISABLE_PROBE_PIN_PULL_UP
      R_PFS->PORT[PROBE_CPU_PORT].PIN[PROBE_CPU_BIT].PmnPFS_b.PCR = 0;  //Pull up none Normal low operation. Requires external pull-down.
  #else
      R_PFS->PORT[PROBE_CPU_PORT].PIN[PROBE_CPU_BIT].PmnPFS_b.PCR = 1;  //Pull up Enable internal pull-up resistors. Normal high operation.
  #endif
  // probe_configure_invert_mask(false); // Initialize invert mask. Not required. Updated when in-use.

    PFS_WRITE_DISABLE;

}


// Called by probe_init() and the mc_probe() routines. Sets up the probe pin invert mask to
// appropriately set the pin logic according to setting for normal-high/normal-low operation
// and the probing cycle modes for toward-workpiece/away-from-workpiece.
void probe_configure_invert_mask(uint8_t is_probe_away)
{
  probe_invert_mask = 0; // Initialize as zero.
  if (bit_isfalse(settings.flags,BITFLAG_INVERT_PROBE_PIN)) { probe_invert_mask ^= PROBE_MASK; }
  if (is_probe_away) { probe_invert_mask ^= PROBE_MASK; }
}


// Returns the probe pin state. Triggered = true. Called by gcode parser and probe state monitor.
uint8_t probe_get_state() { return((PROBE_CPU_BIT & PROBE_MASK) ^ probe_invert_mask); }


// Monitors probe pin state and records the system position when detected. Called by the
// stepper ISR per ISR tick.
// NOTE: This function must be extremely efficient as to not bog down the stepper ISR.
void probe_state_monitor()
{
  if (sys.probe_state == PROBE_ACTIVE) {
    if (probe_get_state()) {
      sys.probe_state = PROBE_OFF;
      memcpy(sys.probe_position, sys.position, sizeof(float)*N_AXIS);
      bit_true(sys.rt_exec_state, EXEC_MOTION_CANCEL);
    }
  }
}

#endif