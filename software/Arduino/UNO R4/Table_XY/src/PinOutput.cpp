/*
  Access to Output Pins
  
  Copyright (c) 2026 	  Gomez Costa J.L. 
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
***/

#include "GRBLFiles/grbl.h"


//===============================================================================
//  Initialization
//===============================================================================
void PinOutput_init(void) {

  // Output pin
  PFS_WRITE_ENABLE;

  R_PMISC->PWPR_b.B0WI = 1;
  R_PMISC->PWPR_b.PFSWE = 1;

  // Init all bits to 0, standard setting

  // Motor X:
  R_PFS->PORT[X_DIRECTION_CPU_PORT].PIN[X_DIRECTION_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[X_DIRECTION_CPU_PORT].PIN[X_DIRECTION_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

  R_PFS->PORT[X_STEP_CPU_PORT].PIN[X_STEP_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[X_STEP_CPU_PORT].PIN[X_STEP_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

   // Motor Y:
  R_PFS->PORT[Y_DIRECTION_CPU_PORT].PIN[Y_DIRECTION_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[Y_DIRECTION_CPU_PORT].PIN[Y_DIRECTION_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

  R_PFS->PORT[Y_STEP_CPU_PORT].PIN[Y_STEP_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[Y_STEP_CPU_PORT].PIN[Y_STEP_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

    // Motor Z:
  R_PFS->PORT[Z_DIRECTION_CPU_PORT].PIN[Z_DIRECTION_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[Z_DIRECTION_CPU_PORT].PIN[Z_DIRECTION_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

  R_PFS->PORT[Z_STEP_CPU_PORT].PIN[Z_STEP_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[Z_STEP_CPU_PORT].PIN[Z_STEP_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

  // Stepper Disable
  R_PFS->PORT[STEPPERS_DISABLE_CPU_PORT].PIN[STEPPERS_DISABLE_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[STEPPERS_DISABLE_CPU_PORT].PIN[STEPPERS_DISABLE_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

  PFS_WRITE_DISABLE;
}
