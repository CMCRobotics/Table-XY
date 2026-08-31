/*
  limits.h - code pertaining to limit-switches and performing the homing cycle
  Part of Grbl

  Copyright (c) 2026 Gomez Costa J.L.
  Copyright (c) 2012-2015 Sungeun K. Jeon
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

#ifndef limits_h
#define limits_h


// Initialize the limits module
void limits_init();
void Function_ISRinterrupt_LimitInput(void);

// Returns limit state as a bit-wise uint8 variable.
uint8_t limits_get_state();

// Perform one portion of the homing cycle based on the input settings.
void limits_go_home(uint8_t cycle_mask);

// Check for soft limit violations
void limits_soft_check(float *target);

#endif

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#ifdef MAIN_FILE

// Memories only defined on Main
    #define MAIN_FILE_LIMITS

#else

    #define MAIN_FILE_LIMITS extern

#endif

MAIN_FILE_LIMITS u_int8_t LimitX_ctr;     // Number of times line has been detected active/not active
MAIN_FILE_LIMITS u_int8_t LimitY_ctr;
MAIN_FILE_LIMITS u_int8_t LimitZ_ctr;
MAIN_FILE_LIMITS boolean disable_limits;
