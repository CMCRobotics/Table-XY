/*
  cpu_map_r7fa4m1ab.h - CPU and pin mapping configuration file
  Part of Grbl

  Copyright (c) 2026 Gomez Costa J.L.s

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

/* Porting of GRBL to Arduino UNO R4  */

#define GRBL_PLATFORM "R7FA4M1AB"

// Useful utils:
// AppData\Local\Arduino15\packages\arduino\hardware\renesas_uno\1.5.3\cores\arduino\api
//  common.h

#include <cpu_r7fa4m1ab.h>


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Define Interruption indexes/Priorities

// IELSR index fixed

#define AGT1_INTERRUPT_EVENT_UDF_IELSRindex         (20UL)   // IELSR index AGT1

#define GPT320_INTERRUPT_EVENT_OVF_IELSRindex       (21UL)   // IELSR index GPT30 Overflow
#define GPT320_INTERRUPT_EVENT_CMPA_IELSRindex      (22UL)   // IELSR index GPT30 CMPA interrupt
#define GPT320_INTERRUPT_EVENT_CMPB_IELSRindex      (23UL)   // IELSR index GPT30 CMPB interrupt

#define UART_INTERRUPT_EVENT_TXI_IELSRindex    (24UL)   // IELSR index UART SCI TXI
#define UART_INTERRUPT_EVENT_RXI_IELSRindex    (25UL)   // IELSR index UART SCI RXI


// Interrupt Priority
#define AGT_INTERRUPT_EVENT_PRIORITY_UNDERFLOW  (15UL)

#define GPT320_INTERRUPT_EVENT_PRIORITY_OVF     (2UL)
#define GPT320_INTERRUPT_EVENT_PRIORITY_CMPA    (1UL)
#define GPT320_INTERRUPT_EVENT_PRIORITY_CMPB    (1UL)

#define UART_INTERRUPT_EVENT_PRIORITY_TXI  (8UL)
#define UART_INTERRUPT_EVENT_PRIORITY_RXI  (8UL)


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// AGT1 related interrupts

#define LIMIT_CONTROL_ISR_PERIOD_US (1000)          // Period of interruption for checking LIMIT and Controls
#define AGT1_LIMIT_CONTROL_COUNTER (LIMIT_CONTROL_ISR_PERIOD_US * AGT1_CLOCK_MHZ)

void AGT1_init(void);
void ISRinterrupt_AGT1(void);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#define STEP_PORT_WRITE  (R_PORT1->PODR)
#define DIRECTION_PORT_WRITE  (R_PORT1->PODR)

#define STEP_PORT_READ  (R_PORT1->PIDR)
#define DIRECTION_PORT_READ  (R_PORT1->PIDR)

// Define Motor X signals
#define X_DIRECTION_CPU_PORT  1
#define X_DIRECTION_CPU_BIT   7 // Arduino Uno Digital Pin D5

#define X_DIRECTION_SET  (R_PORT1->PODR_b.PODR7 = 1) // Direction
#define X_DIRECTION_CLR  (R_PORT1->PODR_b.PODR7 = 0)

#define X_STEP_CPU_PORT  1
#define X_STEP_CPU_BIT   4 // Arduino Uno Digital Pin D2

#define X_STEP_SET  (R_PORT1->PODR_b.PODR4 = 1) // Step pulse
#define X_STEP_CLR  (R_PORT1->PODR_b.PODR4 = 0)


// Define Motor Y signals
#define Y_DIRECTION_CPU_PORT  1
#define Y_DIRECTION_CPU_BIT   11 // Arduino Uno Digital Pin D6

#define Y_DIRECTION_SET  (R_PORT1->PODR_b.PODR11 = 1) // Direction
#define Y_DIRECTION_CLR  (R_PORT1->PODR_b.PODR11 = 0)

#define Y_STEP_CPU_PORT  1
#define Y_STEP_CPU_BIT   5 // Arduino Uno Digital Pin D3

#define Y_STEP_SET  (R_PORT1->PODR_b.PODR5 = 1) // Step pulse
#define Y_STEP_CLR  (R_PORT1->PODR_b.PODR5 = 0)

// Define Motor Z signals
#define Z_DIRECTION_CPU_PORT  1
#define Z_DIRECTION_CPU_BIT   12 // Arduino Uno Digital Pin D7

#define Z_DIRECTION_SET  (R_PORT1->PODR_b.PODR12 = 1) // Direction
#define Z_DIRECTION_CLR  (R_PORT1->PODR_b.PODR12 = 0)

#define Z_STEP_CPU_PORT  1
#define Z_STEP_CPU_BIT   6 // Arduino Uno Digital Pin D4

#define Z_STEP_SET  (R_PORT1->PODR_b.PODR6 = 1) // Step pulse
#define Z_STEP_CLR  (R_PORT1->PODR_b.PODR6 = 0)

// Define step pulse output pins. NOTE: All step bit pins must be on the same port.
#define X_STEP_BIT      X_STEP_CPU_BIT  // Uno Digital Pin 2
#define Y_STEP_BIT      Y_STEP_CPU_BIT  // Uno Digital Pin 3
#define Z_STEP_BIT      Z_STEP_CPU_BIT  // Uno Digital Pin 4
#define STEP_MASK       ((uint16_t)((1<<X_STEP_BIT)|(1<<Y_STEP_BIT)|(1<<Z_STEP_BIT))) // All step bits

// Define step direction output pins. NOTE: All direction pins must be on the same port.
#define X_DIRECTION_BIT   X_DIRECTION_CPU_BIT  // Uno Digital Pin 5
#define Y_DIRECTION_BIT   Y_DIRECTION_CPU_BIT  // Uno Digital Pin 6
#define Z_DIRECTION_BIT   Z_DIRECTION_CPU_BIT  // Uno Digital Pin 7
#define DIRECTION_MASK    ((uint16_t)((1<<X_DIRECTION_BIT)|(1<<Y_DIRECTION_BIT)|(1<<Z_DIRECTION_BIT))) // All direction bits

// Define stepper driver enable/disable output pin.
#define STEPPERS_DISABLE_CPU_PORT  3
#define STEPPERS_DISABLE_CPU_BIT   4 // Arduino Uno Digital Pin D8

#define STEPPERS_DISABLE_BIT    4  // Uno Digital Pin 8
#define STEPPERS_DISABLE_MASK   (1<<STEPPERS_DISABLE_BIT)

#define STEPPERS_DISABLE_SET  (R_PORT3->PODR_b.PODR4 = 1) // Disable pin Set
#define STEPPERS_DISABLE_CLR  (R_PORT3->PODR_b.PODR4 = 0) // Disable pin Clear

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Define LIMIT input pins

#define LIMIT_X_CPU_PORT  3
#define LIMIT_X_CPU_BIT   3  // Arduino Uno Pin D9

#define LIMIT_X_GET   (R_PORT3->PIDR_b.PIDR3)

#define LIMIT_Y_CPU_PORT  1
#define LIMIT_Y_CPU_BIT   3  // Arduino Uno Pin D10

#define LIMIT_Y_GET   (R_PORT1->PIDR_b.PIDR3)

#define LIMIT_Z_CPU_PORT  4
#define LIMIT_Z_CPU_BIT   10  // Arduino Uno Pin D12

#define LIMIT_Z_GET   (R_PORT4->PIDR_b.PIDR10)

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Define Servo for Pencil

/*--------------------------------------------------------------------------------------
Signal pin source select
  PFS.PDR 1: Output pin
  PFS.PMR 1: Peripheral
  PFS.PSEL[4:0] x03: GTIOC6A
--------------------------------------------------------------------------------------*/

#define SERVO_CPU_PORT  4
#define SERVO_CPU_BIT   11  // Arduino Uno Pin D11

/*--------------------------------------------------------------------------------------

---------------------------------------------------------------------
GPT PWM Timer 6

  Frequency:
    CPU clock: Internal 48MHz
    PCLKD frequency: CPU
    GPT6 clock Frequency: PCLDK/16
    GPT6 clock freq: 3 MHz

  GPT6 counter
    Max counter: 60000
    Servo Period: (Max counter) / 3MHz: 20msec
    Servo Frequency: 50Hz

    count 1msec: 3000

---------------------------------------------------------------------
Counter frequency setting

  SCKDIVCR: System clock Division control Register
    PCKD[2:0] Peripheral Modules clock D: 000b %1

  GTCR: General PWM Timer Control Register
    TPCS[2:0] Timer prescaler select: 001b: %4

---------------------------------------------------------------------
General behaviour setting

  GTCR: General PWM Timer Control Register
    CST: 1/0 Start/Stop operation
    MD[2:0]  Mode Select: 000b: Saw-wave PWM mode (single buffer or double buffer possible)

  GTUDDTYC:General PWM Timer Count Direction and Duty Setting Register
    UD Count Direction Setting 1: GTCNT counts up.
    OADTY[1:0] GTIOCA Output Duty Setting: 0xb: GTIOCA pin duty depends on compare match

  GTIOR:  General PWM Timer I/O Control Register
    GTIOA[4:0] GTIOCA Pin Function Select: See Table 22.5.
              01001b: Initial output is low, High output at cycle end, Low output at GTCCRA/GTCCRB compare match
    OADFLT GTIOCA Pin Output Value Setting at the Count Stop: 0: The GTIOCA pin outputs low when counting stops
    OAE GTIOCA Pin Output Enable: 1: Output is enabled.

--------------------------------------------------------------------------------------*/
#define SERVO_GPT6_TPCS       (4)       // This value gives %16 if Freq: 48MHz

#define SERVO_COUNT_1MSEC     ((F_CPU/16.0) * 0.001)      // value = (F_CPU/16) * time
#define SERVO_COUNTER_CLOCK   (F_CPU/(16.0))              // value = (F_CPU/16)
#define SERVO_COUNTER_CLOCK_MZ (SERVO_COUNTER_CLOCK / 1000000)

#define SERVO_REGISTER_VALUE  (R_GPT6->GTCCR_b[2].GTCCR)


/*--------------------------------------------------------------------------------------
Main ISR Interruption for Motor control
---------------------------------------------------------------------
GPT PWM 32 bit Timer 0

  Frequency:
    CPU clock: Internal 48MHz
    PCLKD frequency: CPU
    GPT0 clock Frequency: PCLDK/1
    GPT0 clock freq: 48MHz MHz

  GPT0 counter
    Max counter: Step speed
    CMP-A: Delay DIRECTION bit - STEP pulse (if Enabled)
    CMP-B: STEP pulse Thigh width


---------------------------------------------------------------------
Counter frequency setting

  SCKDIVCR: System clock Division control Register
    PCKD[2:0] Peripheral Modules clock D: 000b %1

  GTCR: General PWM Timer Control Register
    TPCS[3:0] Timer prescaler select: 000b: %1

---------------------------------------------------------------------
General behaviour setting

  GTCR: General PWM Timer Control Register
    CST: 1/0 Start/Stop operation
    MD[2:0]  Mode Select: 000b: Saw-wave PWM mode (single buffer or double buffer possible)

  GTUDDTYC:General PWM Timer Count Direction and Duty Setting Register
    UD Count Direction Setting 1: GTCNT counts up.

  GTIOR:  General PWM Timer I/O Control Register
    Output PWM pins are disabled

--------------------------------------------------------------------------------------*/
// F_CPU: 48MHz
//  TPCS    Meas div    Meas Freq [MHz]   Manual Div
//    4       16          3                 256
//    3        4          12                64
//    2        4?         12?               64?
//    1        1          48                4?
//    0        1          48                4?
// Inconsistent:
//  Include file: TPCS  4 bits [26..23]
//  Manual doc:   TPCS  3 bits [26..24]

#define MOTOR_STEP_TPCS           (0)
#define MOTOR_STEP_TPCS_PRESCALER_DIVISION  (1.0)

#define Test_MOTOR_PERIOD_ticks     (90000UL)
#define Test_MOTOR_DELAY_DIRECTION_2_STEP_ticks   (18000UL)
#define Test_MOTOR_STEP_PULSE_WIDTH_ticks         (30000UL)

#define MOTOR_DELAY_DIRECTION_2_STEP_REGISTER_VALUE  (R_GPT0->GTCCR[0])
// #define MOTOR_DELAY_DIRECTION_2_STEP_REGISTER_VALUE  (R_GPT0->GTCCR_b[0].GTCCR)
#define MOTOR_STEP_PULSE_WIDTH_REGISTER_VALUE  (R_GPT0->GTCCR_b[1].GTCCR)


#define MOTOR_TICK_PER_USEC (CPU_CLOCK_PCLKD / (1000000.0 * MOTOR_STEP_TPCS_PRESCALER_DIVISION ))
#define MOTOR_TICK_PER_SEC (1000000.0 * 60.0 * MOTOR_TICK_PER_USEC)     // Cycles of Timer per sec

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Define probe switch input pin.
// This setting collides with TEST-2

#define PROBE_CPU_PORT  1
#define PROBE_CPU_BIT   0  // Uno Analog Pin 5

#define PROBE_PORT  (R_PORT1->PODR)

#define PROBE_MASK  (1<<PROBE_CPU_BIT)

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Define Control input pins

#define CONTROL_ABORT_RESET_CPU_PORT  0
#define CONTROL_ABORT_RESET_CPU_BIT   14  // Arduino Uno Pin A0

#define CONTROL_ABORT_RESET_GET   (R_PORT0->PIDR_b.PIDR14)

// NOTE: Safety door is shared with feed hold. Enabled by config define.
#define CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_PORT  0
#define CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_BIT   0  // Arduino Uno Pin A1

#define CONTROL_FEED_HOLD_SAFETY_DOOR_GET   (R_PORT0->PIDR_b.PIDR0)


#define CONTROL_RESUME_CYCLE_START_CPU_PORT  0
#define CONTROL_RESUME_CYCLE_START_CPU_BIT   1  // Arduino Uno Pin A2

#define CONTROL_RESUME_CYCLE_START_GET   (R_PORT0->PIDR_b.PIDR1)

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Define Coolant Output bit

#define COOLANT_CPU_PORT  0
#define COOLANT_CPU_BIT   2 // Arduino Uno Analog pin A3

#define COOLANT_SET  (R_PORT0->PODR_b.PODR2 = 1)
#define COOLANT_CLR  (R_PORT0->PODR_b.PODR2 = 0)