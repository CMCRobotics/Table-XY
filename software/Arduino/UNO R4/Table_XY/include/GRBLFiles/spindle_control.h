/*
  spindle_control.h - spindle control methods
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

// Servo Hardware:

// Servo Period
#define SERVO_PULSE_FREQ_HZ 50
#define SERVO_MAX_COUNTER     (SERVO_COUNTER_CLOCK / SERVO_PULSE_FREQ_HZ)   // value = (F_CPU/16) * desired_period

// Servo Pulse value High
#define SERVO_PULSE_HIGH_US_MIN (700)
#define SERVO_PULSE_HIGH_US_MAX (1250)
// #define SERVO_PULSE_HIGH_US_MAX (2500)

// PWM counter
#define PWM_COUNTER_MIN (SERVO_COUNTER_CLOCK_MZ * SERVO_PULSE_HIGH_US_MIN)
#define PWM_COUNTER_MAX (SERVO_COUNTER_CLOCK_MZ * SERVO_PULSE_HIGH_US_MAX)


#ifndef spindle_control_h
#define spindle_control_h


// Initializes spindle pins and hardware PWM, if enabled.
void spindle_init();

// Sets spindle direction and spindle rpm via PWM, if enabled.
void spindle_run(uint8_t direction, float rpm);

void spindle_set_state(uint8_t state, float rpm);

// Kills spindle.
void spindle_stop();

#endif
