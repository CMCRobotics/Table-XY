/*
  debug.h - Definition of variables for enabling Debug via SCI9 USB connector

  Copyright (c) 2026 Gomez Costa J.L.

  This is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  If not, see <http://www.gnu.org/licenses/>.
*/

// Definitions for UART
// Mode:
//  If Normal mode is used:
//    SCI 9: Program communication
//    SCI 2: Not used
//
//  If Debug mode is used:
//    SCI 9: Debug exchange with IDE
//    SCI 2: Program communication
//

// #define SCI_MODE_DEBUG YES		// comment out for standard operation
