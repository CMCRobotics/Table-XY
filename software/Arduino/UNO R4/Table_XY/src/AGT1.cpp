/*
  Interruption via AGT1 timer

  Part of Grbl

  Copyright (c) 2026 Gomez Costa J.L.

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

#include "GRBLFiles/grbl.h"

//===============================================================================
//  Initialization
//===============================================================================
//---------------------------------------------------------------------------------------------------------------
//Initialice AGT1:
//  This interruption allows checking the Limit and Control bits
//  Timer Mode
//  Clock PCLKB: as set by  AGT1_TCK_CODE
//  Interruption when:
//    Initial: Timer underflows

void AGT1_init(void) {

AGT1_PERIPHERAL_ENABLE;
__NOP();__NOP();__NOP();

AGT1_STOP_COUNTER;

R_AGT1->AGTMR1_b.TMOD = 0;  // Mode Timer
R_AGT1->AGTMR1_b.TCK = AGT1_TCK_CODE;   // PCLKB Division

R_AGT1->AGTMR2 = 0;   // Mode Normal
R_AGT1->AGTIOC = 0;   // Not used in Timer
AGT1_COMPARE_MATCH_DISABLED;

R_AGT1->AGTCMA = 0xFFFF;
R_AGT1->AGTCMB = 0xFFFF;

//Set Interruption

configInterrupt(ISRinterrupt_AGT1, AGT1_INTERRUPT_EVENT_UDF_IELSRindex, AGT_INTERRUPT_EVENT_PRIORITY_UNDERFLOW, AGT_INTERRUPT_EVENT_UNDERFLOW_CODE, false);

R_AGT1->AGT = (u_int16_t)AGT1_LIMIT_CONTROL_COUNTER;
R_AGT1->AGTCR_b.TSTART = 1;

__enable_irq();
}


//===============================================================================
//  AGT1 Interrupt service routine
//===============================================================================
//  This interruption allows checking the Limit and Control bits

void ISRinterrupt_AGT1(void) {

  //-------------------------------------------------------------------------
  // Also modify at the end of function, if it is Set/Clear
  // TEST_2_SET; // Debug: Used to time ISR
  //-------------------------------------------------------------------------

  //Clear Interrupt flag
  R_AGT1->AGTCR_b.TUNDF = 0;                // Clear Underflow Flag
  R_ICU->IELSR_b[AGT1_INTERRUPT_EVENT_UDF_IELSRindex].IR = 0;   // Clear Interruption Flag

  __enable_irq();

  Function_ISRinterrupt_LimitInput();

  Function_ISRinterrupt_ControlInput();

  //-------------------------------------------------------------------------
  // Also modify at the end of function, if it is Set/Clear
  // TEST_2_CLR; // Debug: Used to time ISR
  //-------------------------------------------------------------------------

}