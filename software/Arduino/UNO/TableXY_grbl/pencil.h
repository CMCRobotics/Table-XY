/*
  gcode.h - rs274/ngc parser.
  Part of Grbl

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

#ifndef pencil_h
#define pencil_h

#define GRBL_BUILD_PENCIL GRBL_BUILD_PENCIL_YEAR GRBL_BUILD_PENCIL_MONTH GRBL_BUILD_PENCIL_DAY    // It is YearMonthDay

//--------------------------------------------------------------------------------------------------------
// Conversion of Z plane movement to M03 / M05 Pencil Up or Down

#define Z_AXIS_LIMIT  1.00                // Limit of Zaxis movement: higher it is Pencil up, lower it is Pencil down

#define PENCIL_UP     SPINDLE_DISABLE     // Action for setting pencil UP
#define PENCIL_DOWN   SPINDLE_ENABLE_CW   // Action for setting pencil UP

#define ACTION_PENCIL_UP "M03"
#define ACTION_PENCIL_DOWN "M05"

#endif
