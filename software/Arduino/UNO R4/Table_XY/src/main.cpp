/*
  main.c - An embedded CNC Controller with rs274/ngc (g-code) support
  Part of Grbl

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
*/

#define MAIN_FILE

#include <grbl.h>

#include <Arduino.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Declare system global variable structure
system_t sys;


//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
void setup(void){

  char revision[90];
  fsp_err_t return_value;

  strcpy(revision, VERSION_NUMBER); strcat(revision, " ");
  strcat(revision, VERSION_BUILD_DAY); strcat(revision, "/");
  strcat(revision, VERSION_BUILD_MONTH); strcat(revision, "/");
  strcat(revision, VERSION_BUILD_YEAR); strcat(revision, " ");

  displayMatrixLED(revision, 2);

  // Initialize system upon power-up.

  #ifdef DEBUG_TEST_1_TEST_2
  // TEST 1 pin collides with COOLANT MIST pin
  // If used, COOLANT nust be disabled
  debugPinOutputTest_1_init();

  // TEST 2 pin collides with PROBE pin
  // If used, PROBE nust be disabled
  debugPinOutputTest_2_init();
  #endif


  debugPinOutputTest_3_init();

  serial_init();    // Setup serial baud rate and interrupts

  // Initialisation of Flash Data
  return_value = Flash_init();

  if (return_value != FSP_SUCCESS){
    __enable_irq(); // Enable interrupts for accessing Serial data
    displayFlashError("Initialisation", return_value);
    //Program will stay here
  };

  __enable_irq(); // Enable interrupts

  settings_init(); // Load Grbl settings from EEPROM
  stepper_init();  // Configure stepper pins and interrupt timers
  system_init();   // Configure pinout pins

  memset(&sys, 0, sizeof(sys));  // Clear all system variables
  sys.abort = true;   // Set abort to complete initialization

    // Check for power-up and set system alarm if homing is enabled to force homing cycle
  // by setting Grbl's alarm state. Alarm locks out all g-code commands, including the
  // startup scripts, but allows access to settings and internal commands. Only a homing
  // cycle '$H' or kill alarm locks '$X' will disable the alarm.
  // NOTE: The startup script will run after successful completion of the homing cycle, but
  // not after disabling the alarm locks. Prevents motion startup blocks from crashing into
  // things uncontrollably. Very bad.
  #ifdef HOMING_INIT_LOCK
    if (bit_istrue(settings.flags,BITFLAG_HOMING_ENABLE)) { sys.state = STATE_ALARM; }
  #endif

  // Force Grbl into an ALARM state upon a power-cycle or hard reset.
  #ifdef FORCE_INITIALIZATION_ALARM
    sys.state = STATE_ALARM;
  #endif

}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
void loop() {

  // Grbl initialization loop upon power-up or a system abort. For the latter, all processes
  // will return to this loop to be cleanly re-initialized.
  for(;;) {

    // TODO: Separate configure task that require interrupts to be disabled, especially upon
    // a system abort and ensuring any active interrupts are cleanly reset.

    // Reset Grbl primary systems.
    serial_reset_read_buffer(); // Clear serial read buffer
    gc_init(); // Set g-code parser to default state
    spindle_init();
    coolant_init();
    probe_init();
    limits_init();
    plan_reset(); // Clear block buffer and planner variables
    st_reset();   // Clear stepper subsystem variables.

    AGT1_init();    // This must be executed after Initialising Limit and Control bits

    // Sync cleared gcode and planner positions to current system position.
    plan_sync_position();
    gc_sync_position();

    // Reset system variables.
    sys.abort = false;
    sys.rt_exec_state = 0;
    sys.rt_exec_alarm = 0;
    sys.suspend = false;

    // Start Grbl main loop. Processes program inputs and executes them.
    protocol_main_loop();

  }
  // Never reached
}
