/*
  system.c - Handles system level commands and real-time processes
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

void system_init()
{
  // Defines Input Control pins

  PFS_WRITE_ENABLE;

  // Define Bit input

  // Feed_Hold is shared with Safety door. Enabled by config define.
  R_PFS->PORT[CONTROL_ABORT_RESET_CPU_PORT].PIN[CONTROL_ABORT_RESET_CPU_BIT].PmnPFS = 0UL; // Init all bits to 0, standard setting
  R_PFS->PORT[CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_PORT].PIN[CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_BIT].PmnPFS = 0UL;
  R_PFS->PORT[CONTROL_RESUME_CYCLE_START_CPU_PORT].PIN[CONTROL_RESUME_CYCLE_START_CPU_BIT].PmnPFS = 0UL;

  #ifdef DISABLE_CONTROL_PIN_PULL_UP
      R_PFS->PORT[CONTROL_ABORT_RESET_CPU_PORT].PIN[CONTROL_ABORT_RESET_CPU_BIT].PmnPFS_b.PCR = 0;  //Pull up none Normal low operation. Requires external pull-down.
      R_PFS->PORT[CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_PORT].PIN[CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_BIT].PmnPFS_b.PCR = 0;
      R_PFS->PORT[CONTROL_RESUME_CYCLE_START_CPU_PORT].PIN[CONTROL_RESUME_CYCLE_START_CPU_BIT].PmnPFS_b.PCR = 0;

  #else
      R_PFS->PORT[CONTROL_ABORT_RESET_CPU_PORT].PIN[CONTROL_ABORT_RESET_CPU_BIT].PmnPFS_b.PCR = 1;  //Pull up Enable internal pull-up resistors. Normal high operation.
      R_PFS->PORT[CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_PORT].PIN[CONTROL_FEED_HOLD_SAFETY_DOOR_CPU_BIT].PmnPFS_b.PCR = 1;
      R_PFS->PORT[CONTROL_RESUME_CYCLE_START_CPU_PORT].PIN[CONTROL_RESUME_CYCLE_START_CPU_BIT].PmnPFS_b.PCR = 1;

  #endif

  PFS_WRITE_DISABLE;

ControlAbortReset_ctr = 0;
ControlResumeCycleStart_ctr = 0;
ControlFeedHoldSafetyDoor_ctr = 0;

}


// Called by AGT1 interrupt
// Pin change Control commands, i.e. cycle start, feed hold, and reset. Sets
// only the realtime command execute variable to have the main program execute these when
// its ready. This works exactly like the character-based realtime commands when picked off
// directly from the incoming serial data stream.

void Function_ISRinterrupt_ControlInput(void)
{
  boolean active_low = true;

  #ifdef INVERT_CONTROL_PIN
    active_low = false;
  #endif

  if (checkControlLimitStatus (CONTROL_ABORT_RESET_GET, &ControlAbortReset_ctr, active_low)){
    // Bit ABORT / RESET is detected
    mc_reset();
    return;
  }

  if (checkControlLimitStatus (CONTROL_RESUME_CYCLE_START_GET, &ControlResumeCycleStart_ctr, active_low)){
    // Bit RESUME / CYCLE START is detected
    bit_true(sys.rt_exec_state, EXEC_CYCLE_START);
    return;
  }

if (checkControlLimitStatus ((u_int8_t) CONTROL_FEED_HOLD_SAFETY_DOOR_GET, &ControlFeedHoldSafetyDoor_ctr, active_low)){
  #ifndef ENABLE_SAFETY_DOOR_INPUT_PIN
    // Bit FEED HOLD is detected
    bit_true(sys.rt_exec_state, EXEC_FEED_HOLD);
  #else
    // Bit SAFETY DOOR is detected
    bit_true(sys.rt_exec_state, EXEC_SAFETY_DOOR);
  #endif
  }

return;
}


// Returns if safety door is ajar(T) or closed(F), based on pin state.
uint8_t system_check_safety_door_ajar()
{
  #ifdef ENABLE_SAFETY_DOOR_INPUT_PIN
    #ifdef INVERT_CONTROL_PIN
      if(CONTROL_FEED_HOLD_SAFETY_DOOR_GET)      return 0;  // line not YET detected as active
      return 1;

    #else
      if(CONTROL_FEED_HOLD_SAFETY_DOOR_GET)      return 1;  // line DETECTED as active
      return 0;

    #endif
  #else
    return(0); // Input pin not enabled, so just return that it's closed.
  #endif
}


// Executes user startup script, if stored.
void system_execute_startup(char *line)
{
  uint8_t n;
  for (n=0; n < N_STARTUP_LINE; n++) {
    if (!(settings_read_startup_line(n, line))) {
      report_status_message(STATUS_SETTING_READ_FAIL);
    } else {
      if (line[0] != 0) {
        printString(line); // Echo startup line to indicate execution.
        report_status_message(gc_execute_line(line));
      }
    }
  }
}


// Directs and executes one line of formatted input from protocol_process. While mostly
// incoming streaming g-code blocks, this also executes Grbl internal commands, such as
// settings, initiating the homing cycle, and toggling switch states. This differs from
// the realtime command module by being susceptible to when Grbl is ready to execute the
// next line during a cycle, so for switches like block delete, the switch only effects
// the lines that are processed afterward, not necessarily real-time during a cycle,
// since there are motions already stored in the buffer. However, this 'lag' should not
// be an issue, since these commands are not typically used during a cycle.
uint8_t system_execute_line(char *line)
{
  uint8_t char_counter = 1;
  uint8_t helper_var = 0; // Helper variable
  uint8_t idx;

  float parameter, value;
  unsigned char first_char;

  switch( line[char_counter] ) {
    case 0 : report_grbl_help(); break;
    case '$': case 'G': case 'C': case 'X':
      if ( line[(char_counter+1)] != 0 ) { return(STATUS_INVALID_STATEMENT); }
      switch( line[char_counter] ) {
        case '$' : // Prints Grbl settings
          if ( sys.state & (STATE_CYCLE | STATE_HOLD) ) { return(STATUS_IDLE_ERROR); } // Block during cycle. Takes too long to print.
          else { report_grbl_settings(); }
          break;
        case 'G' : // Prints gcode parser state
          // TODO: Move this to realtime commands for GUIs to request this data during suspend-state.
          report_gcode_modes();
          break;
        case 'C' : // Set check g-code mode [IDLE/CHECK]
          // Perform reset when toggling off. Check g-code mode should only work if Grbl
          // is idle and ready, regardless of alarm locks. This is mainly to keep things
          // simple and consistent.
          if ( sys.state == STATE_CHECK_MODE ) {
            mc_reset();
            report_feedback_message(MESSAGE_DISABLED);
          } else {
            if (sys.state) { return(STATUS_IDLE_ERROR); } // Requires no alarm mode.
            sys.state = STATE_CHECK_MODE;
            report_feedback_message(MESSAGE_ENABLED);
          }
          break;
        case 'X' : // Disable alarm lock [ALARM]
          if (sys.state == STATE_ALARM) {
            report_feedback_message(MESSAGE_ALARM_UNLOCK);
            sys.state = STATE_IDLE;
            // Don't run startup script. Prevents stored moves in startup from causing accidents.
            if (system_check_safety_door_ajar()) { // Check safety door switch before returning.
              bit_true(sys.rt_exec_state, EXEC_SAFETY_DOOR);
              protocol_execute_realtime(); // Enter safety door mode.
            }
          } // Otherwise, no effect.
          break;
    //  case 'J' : break;  // Jogging methods
          // TODO: Here jogging can be placed for execution as a seperate subprogram. It does not need to be
          // susceptible to other realtime commands except for e-stop. The jogging function is intended to
          // be a basic toggle on/off with controlled acceleration and deceleration to prevent skipped
          // steps. The user would supply the desired feedrate, axis to move, and direction. Toggle on would
          // start motion and toggle off would initiate a deceleration to stop. One could 'feather' the
          // motion by repeatedly toggling to slow the motion to the desired location. Location data would
          // need to be updated real-time and supplied to the user through status queries.
          //   More controlled exact motions can be taken care of by inputting G0 or G1 commands, which are
          // handled by the planner. It would be possible for the jog subprogram to insert blocks into the
          // block buffer without having the planner plan them. It would need to manage de/ac-celerations
          // on its own carefully. This approach could be effective and possibly size/memory efficient.
//       }
//       break;
      }
      break;
    default :
      // Block any system command that requires the state as IDLE/ALARM. (i.e. EEPROM, homing)
      if ( !(sys.state == STATE_IDLE || sys.state == STATE_ALARM) ) { return(STATUS_IDLE_ERROR); }
      switch(line[char_counter]) {

        case 'M' : case 'm' :// Display this menu
          report_grbl_help();
          break;


        case '#' : // Print Grbl NGC parameters
          if ( line[++char_counter] != 0 ) { return(STATUS_INVALID_STATEMENT); }
          else { report_ngc_parameters(); }
          break;


        case 'H' : case 'h' :// Perform homing cycle [IDLE/ALARM]
          if (bit_istrue(settings.flags,BITFLAG_HOMING_ENABLE)) {
            sys.state = STATE_HOMING; // Set system state variable
            // Only perform homing if Grbl is idle or lost.

            // TODO: Likely not required.
            if (system_check_safety_door_ajar()) { // Check safety door switch before homing.
              bit_true(sys.rt_exec_state, EXEC_SAFETY_DOOR);
              protocol_execute_realtime(); // Enter safety door mode.
            }

            mc_homing_cycle();
            if (!sys.abort) {  // Execute startup scripts after successful homing.
              sys.state = STATE_IDLE; // Set to IDLE when complete.
              st_go_idle(); // Set steppers to the settings idle state before returning.
              system_execute_startup(line);
            }
          } else { return(STATUS_SETTING_DISABLED); }
          break;


        case 'I' : case 'i' :
          // Print build info. [IDLE/ALARM]
          // It is a fixed define, not required to store in EEPROM
          report_build_info();
          break;


        case 'P' : case 'p' :
          // Print Pencil build info. [IDLE/ALARM]
          // It is a fixed define, not required to store in EEPROM
          report_build_pencil_info();
          break;


        case 'A' : case 'a' :
          // Print debug_data array
          if(line[++char_counter] != 0) { return(STATUS_INVALID_STATEMENT); }

          for(idx=0; idx < DEBUG_DATA_ARRAY_LENGTH; idx++){
            printString("debug_data[");
            print_uint8_base10(idx);
            printString("] : ");

            print_uint32_base10(debug_data[idx]);
            print_uint32_base16(debug_data[idx], "  0x%08X", 8);

            printString("\r\n");
          }
          printString("\r\n");
          break;


        case 'R' : // Restore defaults [IDLE/ALARM]
          if (line[++char_counter] != 'S') { return(STATUS_INVALID_STATEMENT); }
          if (line[++char_counter] != 'T') { return(STATUS_INVALID_STATEMENT); }
          if (line[++char_counter] != '=') { return(STATUS_INVALID_STATEMENT); }
          if (line[char_counter+2] != 0) { return(STATUS_INVALID_STATEMENT); }
          switch (line[++char_counter]) {
            case '$': settings_restore(SETTINGS_RESTORE_DEFAULTS); break;
            case '#': settings_restore(SETTINGS_RESTORE_PARAMETERS); break;
            case '*': settings_restore(SETTINGS_RESTORE_ALL); break;
            default: return(STATUS_INVALID_STATEMENT);
          }
          report_feedback_message(MESSAGE_RESTORE_DEFAULTS);
          mc_reset(); // Force reset to ensure settings are initialized correctly.
          break;


        case 'N' : case 'n' :// Startup lines. [IDLE/ALARM]
          if ( line[++char_counter] == 0 ) { // Print startup lines
            for (helper_var=0; helper_var < N_STARTUP_LINE; helper_var++) {
              if (!(settings_read_startup_line(helper_var, line))) {
                report_status_message(STATUS_SETTING_READ_FAIL);
              } else {
                report_startup_line(helper_var,line);
              }
            }
            break;
          } else { // Store startup line [IDLE Only] Prevents motion during ALARM.
            if (sys.state != STATE_IDLE) { return(STATUS_IDLE_ERROR); } // Store only when idle.
            helper_var = true;  // Set helper_var to flag storing method.
            // No break. Continues into default: to read remaining command characters.
          }


        default :  // Storing setting methods [IDLE/ALARM]
          if(!read_float(line, &char_counter, &parameter)) { return(STATUS_BAD_NUMBER_FORMAT); }
          if(line[char_counter++] != '=') { return(STATUS_INVALID_STATEMENT); }

          if (helper_var) { // Store startup line
            // Prepare sending gcode block to gcode parser by shifting all characters
            helper_var = char_counter; // Set helper variable as counter to start of gcode block
            do {
              line[char_counter-helper_var] = line[char_counter];
            } while (line[char_counter++] != 0);
            // Execute gcode block to ensure block is valid.
            helper_var = gc_execute_line(line); // Set helper_var to returned status code.
            if (helper_var) { return(helper_var); }
            else {
              helper_var = trunc(parameter); // Set helper_var to int value of parameter
              settings_store_startup_line(helper_var,line);
            }
          } else { // Store global setting.
            if(!read_float(line, &char_counter, &value)) { return(STATUS_BAD_NUMBER_FORMAT); }
            if((line[char_counter] != 0) || (parameter > 255)) { return(STATUS_INVALID_STATEMENT); }
            return(settings_store_global_setting((uint8_t)parameter, value));
          }
          break;
      }
  }
  return(STATUS_OK); // If '$' command makes it to here, then everything's ok.
}


// Returns machine position of axis 'idx'. Must be sent a 'step' array.
// NOTE: If motor steps and machine position are not in the same coordinate frame, this function
//   serves as a central place to compute the transformation.
float system_convert_axis_steps_to_mpos(int32_t *steps, uint8_t idx)
{
  float pos;
  #ifdef COREXY
    if (idx==A_MOTOR) {
      pos = 0.5*((steps[A_MOTOR] + steps[B_MOTOR])/settings.steps_per_mm[idx]);
    } else if (idx==B_MOTOR) {
      pos = 0.5*((steps[A_MOTOR] - steps[B_MOTOR])/settings.steps_per_mm[idx]);
    } else {
      pos = steps[idx]/settings.steps_per_mm[idx];
    }
  #else
    pos = steps[idx]/settings.steps_per_mm[idx];
  #endif
  return(pos);
}


void system_convert_array_steps_to_mpos(float *position, int32_t *steps)
{
  uint8_t idx;
  for (idx=0; idx<N_AXIS; idx++) {
    position[idx] = system_convert_axis_steps_to_mpos(steps, idx);
  }
  return;
}


//------------------------------------------------------------
// Detects if a Control or Input line is Active or Not Active
//  Active or not Active is defined by a parameter
// The detection is made on the AGT1 interrupt rutine
// If the line is detected Active for at least LINE_DETECTION_QTY.
//
// Parameters
//  u_int8_t value:
//    actual value: 0 or 1
//  u_int8_t counter:
//    number of times the line has been on this state
//  boolean active_low
//    true: value to be detected as active is LOW
//
// Return
//  Line status:
//    true: line is active

boolean checkControlLimitStatus (u_int16_t value, u_int8_t *counter, boolean active_low){

u_int16_t value_active = 0;

if(!active_low) value_active = 1;   //Active value is High

if(value == value_active){
  if (*counter  == SOFTWARE_DEBOUNCE){ return true;};  // line has been detected as active
  (*counter)++;
  return false;  // line not YET detected as active
}

// Value is not active
if (*counter  == 0){ return false;};  // line has been detected as NOT active
(*counter)--;

return true;  // line STILL detected as active

}