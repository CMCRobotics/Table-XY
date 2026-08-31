/*
Main Interruption via GPT3203 bit Timer 0
Generates:
  - Direction and Pulse X/Y/Z
  - Step X/Y/Z: High at CMPA, low at CMPB

  Part of Grbl

  Copyright (c) 2026 Gomez Costa J.L.
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

#include "GRBLFiles/grbl.h"

volatile u_int32_t return_value_GPT320;
volatile u_int32_t dump1, dump2;

//===============================================================================
//  Initialization
//===============================================================================
void GPT320_init(void) {

//---------------------------------------------------------------------------------------------------------------
//Initialice GPT320:
//  Timer Mode
//  Clock PCLKD: as set by  GPT320_TCK_CODE
//  Interruption when:
//    OVF: Timer underflows
//    CMPA: Compare match
//    CMPB: Compare match

GPT32_PERIPHERAL_ENABLE;
__NOP();__NOP();__NOP();

GPT_REG_WRITE_ENABLE(R_GPT0);

R_GPT0->GTCR = 0;             // Clear Register
R_GPT0->GTCR_b.CST = 0;       // GPT Stop
R_GPT0->GTCR_b.MD = 0;        // Saw-wave PWM mode (single buffer or double buffer possible)
R_GPT0->GTCR_b.TPCS = MOTOR_STEP_TPCS;      // Clock %

R_GPT0->GTUDDTYC = 0;             // Clear Register
R_GPT0->GTUDDTYC_b.UD = 1;        // GTCNT count up

R_GPT0->GTIOR = 0;                // Clear Register
                                  // GTIOCA/B Pin Output Disabled

R_GPT0->GTBER_b.BD0 = 1;        // Timer buffer enable Register: Buffer is Disabled
R_GPT0->GTBER_b.BD1 = 0;        // Buffer is Enabled GTPR
R_GPT0->GTBER_b.CCRA = 0;       // GTCCRA: no buffer
R_GPT0->GTBER_b.CCRB = 0;       // GTCCRB: no buffer
R_GPT0->GTBER_b.PR = 1;         // GTPR: single buffer

R_GPT0->GTCNT = 0;                // Ramp Counter
R_GPT0->GTPR_b.GTPR = Test_MOTOR_PERIOD_ticks;  // Max counter for setting the Step period
R_GPT0->GTPBR_b.GTPBR = Test_MOTOR_PERIOD_ticks;

R_GPT0->GTSSR_b.CSTRT = 1;    // Start source: Software start
R_GPT0->GTSTR_b.CSTRT0 = 0;   // No Start

GPT_REG_WRITE_DISABLE(R_GPT0);

//Set Interruption
return_value_GPT320 = configInterrupt(ISRinterrupt_Motor_Main, GPT320_INTERRUPT_EVENT_OVF_IELSRindex, GPT320_INTERRUPT_EVENT_PRIORITY_OVF, GPT_INTERRUPT_EVENT_OVF_CODE, false);

#ifdef STEP_PULSE_DELAY
return_value_GPT320 = configInterrupt(ISRinterrupt_Motor_DelayEnd, GPT320_INTERRUPT_EVENT_CMPA_IELSRindex, GPT320_INTERRUPT_EVENT_PRIORITY_CMPA, GPT_INTERRUPT_EVENT_CMPA_CODE, false);

dump1 =(u_int32_t) (STEP_PULSE_DELAY * MOTOR_TICK_PER_USEC);
dump2 = (u_int32_t) ((STEP_PULSE_DELAY + STEP_PULSE_WIDTH_DEFAULT) * MOTOR_TICK_PER_USEC);

  MOTOR_DELAY_DIRECTION_2_STEP_REGISTER_VALUE = dump1;   // Set CMPA interrupt time
  MOTOR_STEP_PULSE_WIDTH_REGISTER_VALUE = dump2;   // CMPB interrupt time, step Pulse width default + Delay

#else // Normal operation
  // Set step pulse time

  // No delay -> set CMPA to maximum value
  // So this interruption will never be fired
  MOTOR_DELAY_DIRECTION_2_STEP_REGISTER_VALUE  = 0xFFFFFFFF;

  volatile u_int32_t test_dump = (u_int32_t) (STEP_PULSE_WIDTH_DEFAULT * MOTOR_TICK_PER_USEC);

  MOTOR_STEP_PULSE_WIDTH_REGISTER_VALUE = test_dump;

#endif

return_value_GPT320 = configInterrupt(ISRinterrupt_Motor_StepPulseEnd, GPT320_INTERRUPT_EVENT_CMPB_IELSRindex, GPT320_INTERRUPT_EVENT_PRIORITY_CMPB, GPT_INTERRUPT_EVENT_CMPB_CODE, false);

}

//===============================================================================
//  Main
//===============================================================================

//-----------------------------------------------------------------------------
//  Disable Stepper Main interruption
//  All Timer counters MUST have been set previously


void GPT320_enable(void) {

  GPT_REG_WRITE_ENABLE(R_GPT0);

  R_GPT0->GTCR_b.CST = 1;       // GPT Start

  GPT_REG_WRITE_DISABLE(R_GPT0);

  __enable_irq();
}

//-----------------------------------------------------------------------------
//  Disable Stepper Main interruption


void GPT320_disable(void) {

  GPT_REG_WRITE_ENABLE(R_GPT0);

  R_GPT0->GTCR_b.CST = 0;       // GPT Stop

  GPT_REG_WRITE_DISABLE(R_GPT0);

}