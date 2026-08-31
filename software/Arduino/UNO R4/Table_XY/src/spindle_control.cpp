/*
  spindle_control.c - spindle control methods
  Part of Grbl

  Copyright (c) 2026 	  Gomez Costa J.L.
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


/* RC-Servo PWM modification: switch between 0.6ms and 2.5ms pulse-width at 61Hz
   Prescaler 1024 = 15625Hz / 256Steps =  61Hz	64µs/step -> Values 15 / 32 for 1ms / 2ms
   Reload value = 0x07
   Replace this file in C:\Program Files (x86)\Arduino\libraries\GRBL
*/


#include "grbl.h"

#define RC_SERVO_INVERT     1     // Comment out for NOT Inverting servo direction


void spindle_init()
{
  // Configure variable spindle

  GPT16_PERIPHERAL_ENABLE;    // Enable GPT Peripheral before accessing

  //Servo pin
  PFS_WRITE_ENABLE;

  // Define Bit as GPT
  R_PFS->PORT[SERVO_CPU_PORT].PIN[SERVO_CPU_BIT].PmnPFS = 0UL;          // Init all bits to 0, standard setting
  R_PFS->PORT[SERVO_CPU_PORT].PIN[SERVO_CPU_BIT].PmnPFS_b.PDR = 1;      //Output
  R_PFS->PORT[SERVO_CPU_PORT].PIN[SERVO_CPU_BIT].PmnPFS_b.PMR = 1;      //Peripheral
  R_PFS->PORT[SERVO_CPU_PORT].PIN[SERVO_CPU_BIT].PmnPFS_b.PSEL = 0x03;  //GTIOC6A

  PFS_WRITE_DISABLE;

  //---------------------------------------------------------------------------------------------------------------
  //Servo PWM Timer
  GPT_REG_WRITE_ENABLE(R_GPT6);

  R_GPT6->GTCR = 0;             // Clear Register
  R_GPT6->GTCR_b.CST = 0;       // GPT Stop
  R_GPT6->GTCR_b.MD = 0;        // Saw-wave PWM mode (single buffer or double buffer possible)
  R_GPT6->GTCR_b.TPCS = SERVO_GPT6_TPCS;      // Clock %

  R_GPT6->GTUDDTYC = 0;             // Clear Register
  R_GPT6->GTUDDTYC_b.UD = 1;        // GTCNT count up
  R_GPT6->GTUDDTYC_b.OADTY = 0x00;  // GTIOCA pin duty depends on compare match

  R_GPT6->GTIOR = 0;                // Clear Register
  R_GPT6->GTIOR_b.GTIOA = 0x09;     // Initial output is low, High output at cycle end, Low output at GTCCRA/GTCCRB compare match
  R_GPT6->GTIOR_b.OADFLT = 0;       // GTIOCA Pin Output Value Setting at the Count Stop: The GTIOCA pin outputs low when counting stops
  R_GPT6->GTIOR_b.OAE = 1;          // GTIOCA Pin Output Enable
  R_GPT6->GTIOR_b.OADF = 0;         // GTIOCA Pin Disable value setting

  R_GPT6->GTBER_b.BD0 = 0;        // Timer buffer enable Register: Buffer is enabled GTCCR
  R_GPT6->GTBER_b.BD1 = 0;        // Buffer is enabled GTPR
  R_GPT6->GTBER_b.CCRA = 1;       // GTCCRA: single buffer
  R_GPT6->GTBER_b.PR = 1;         // GTPR: single buffer

  R_GPT6->GTCNT = 0;                // Ramp Counter
  R_GPT6->GTPR_b.GTPR = SERVO_MAX_COUNTER;  // Max counter for setting the Servo Period
  R_GPT6->GTPBR_b.GTPBR = SERVO_MAX_COUNTER;

  SERVO_REGISTER_VALUE = SERVO_COUNT_1MSEC;   // Compare Register for Servo

  R_GPT6->GTSSR_b.CSTRT = 1;    // Start source: Software start
  R_GPT6->GTSTR_b.CSTRT6 = 1;   // Start
  R_GPT6->GTCR_b.CST = 1;       // GPT Start

  GPT_REG_WRITE_DISABLE(R_GPT6);

  spindle_run(SPINDLE_DISABLE,0);  // Define initial position as Pencil Up
}

//-----------------------------------------------------------------------------
// Names are kept kept for compatibility with the main GRBL files, but it is not used
//
//  Parameters:
//    direction:
//      action to be taken
//        Pencil UP:    SPINDLE_DISABLE (M5)
//        Pencil Down:  SPINDLE_ENABLE_CW (M3) or SPINDLE_ENABLE_CCW (M4)
//
//    rpm: comes for gc_state.spindle_speed
//      Parameter S in M3/ M4 is not used when Pencil is installed
//

void spindle_run(uint8_t direction, float rpm)
{
  if (sys.state == STATE_CHECK_MODE) { return; }

  // Empty planner buffer to ensure spindle is set when programmed.
  protocol_auto_cycle_start();  //temp fix for M3 lockup
  protocol_buffer_synchronize();

  if (direction == SPINDLE_DISABLE) {

    spindle_stop();

  } else {

    GPT_REG_WRITE_ENABLE(R_GPT6);

    #ifdef RC_SERVO_INVERT
      SERVO_REGISTER_VALUE = PWM_COUNTER_MIN;
    #else
      SERVO_REGISTER_VALUE = PWM_COUNTER_MAX;
    #endif

    GPT_REG_WRITE_DISABLE(R_GPT6);

  }
}



void spindle_stop()
{

GPT_REG_WRITE_ENABLE(R_GPT6);

#ifdef RC_SERVO_INVERT
  SERVO_REGISTER_VALUE = PWM_COUNTER_MAX;
#else
  SERVO_REGISTER_VALUE = PWM_COUNTER_MIN;
#endif

GPT_REG_WRITE_DISABLE(R_GPT6);

}

// Not used as it is Pencil up/down
void spindle_set_state(uint8_t state, float rpm){

}

//-----------------------------------------------------------------------------------------------
// The following commented-out functions are the originals, with parts related to PWM features
// As it is NOT possible to test, I left them for a future addition
/*

#define RC_SERVO_SHORT     9      // Original: 15
#define RC_SERVO_LONG      39     // Original: 32

void spindle_stop()
{     // On the Uno, spindle enable and PWM are shared. Other CPUs have seperate enable pin.
       #ifdef RC_SERVO_INVERT
          OCR_REGISTER = RC_SERVO_LONG;

          GPT_REG_WRITE_ENABLE(R_GPT6);
          SERVO_REGISTER_VALUE = servo_count;
          GPT_REG_WRITE_DISABLE(R_GPT6);

      #else
          OCR_REGISTER = RC_SERVO_SHORT;

          GPT_REG_WRITE_ENABLE(R_GPT6);
          SERVO_REGISTER_VALUE = servo_count;
          GPT_REG_WRITE_DISABLE(R_GPT6);
      #endif
}


void spindle_run(uint8_t direction, float rpm)
{
  if (sys.state == STATE_CHECK_MODE) { return; }

  // Empty planner buffer to ensure spindle is set when programmed.
  protocol_auto_cycle_start();  //temp fix for M3 lockup
  protocol_buffer_synchronize();

  if (direction == SPINDLE_DISABLE) {

    spindle_stop();

  } else {

      // TODO: Install the optional capability for frequency-based output for servos.
      #define SPINDLE_RPM_RANGE (SPINDLE_MAX_RPM-SPINDLE_MIN_RPM)
      #define RC_SERVO_RANGE (RC_SERVO_LONG-RC_SERVO_SHORT)

      uint8_t current_pwm;

	   if ( rpm < SPINDLE_MIN_RPM ) { rpm = 0; }
      else {
        rpm -= SPINDLE_MIN_RPM;
        if ( rpm > SPINDLE_RPM_RANGE ) { rpm = SPINDLE_RPM_RANGE; } // Prevent integer overflow
      }

      #ifdef RC_SERVO_INVERT
          current_pwm = floor( RC_SERVO_LONG - rpm*(RC_SERVO_RANGE/SPINDLE_RPM_RANGE));

          GPT_REG_WRITE_ENABLE(R_GPT6);
          SERVO_REGISTER_VALUE = current_pwm;
          GPT_REG_WRITE_DISABLE(R_GPT6);

      #else
         current_pwm = floor( rpm*(RC_SERVO_RANGE/SPINDLE_RPM_RANGE) + RC_SERVO_SHORT);

         GPT_REG_WRITE_ENABLE(R_GPT6);
         SERVO_REGISTER_VALUE = current_pwm;
         GPT_REG_WRITE_DISABLE(R_GPT6);

      #endif

	  #ifdef MINIMUM_SPINDLE_PWM
        if (current_pwm < MINIMUM_SPINDLE_PWM) { current_pwm = MINIMUM_SPINDLE_PWM; }

        GPT_REG_WRITE_ENABLE(R_GPT6);
        SERVO_REGISTER_VALUE = current_pwm;
        GPT_REG_WRITE_DISABLE(R_GPT6);

      #endif

  }
}

*/
