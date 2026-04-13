#!/usr/bin/env python
"""\
Transfers a GCode control strings to a GRBL based CNC controller.
    - Gcode comes from a file or Keyboard
    - A logging file can be set
    - Communication is through a USB serial link
        Settings:
            - baud rate = 115200
            - data bits: 8
            - stop bits: 1            
            - parity: None
            - flow control: None

usage: grbl_serial.py [-h] [-f FILE] [-l LOG] port

    Sends GCode to a GRBL based controller through a serial (USB) link. Input from Gcode file or keyboard. Exchange can be
    logged

    positional arguments:
    port                  USB port name

    options:
    -h, --help            show this help message and exit
    -f FILE, --file FILE  Gcode file [directory/../name.extension], if None: input is from Keyboard
    -l LOG, --log LOG     Logging of exchanges, to file log.log, stored in [directory] or in pwd (if no Gcode)


Based on: 
    Simple g-code streaming script for grbl

    Provided as an illustration of the basic communication interface
    for grbl. When grbl has finished parsing the g-code block, it will
    return an 'ok' or 'error' response. When the planner buffer is full,
    grbl will not send a response until the planner buffer clears space.

    G02/03 arcs are special exceptions, where they inject short line 
    segments directly into the planner. So there may not be a response 
    from grbl for the duration of the arc.

---------------------
The MIT License (MIT)

Copyright (c) 2012 Sungeun K. Jeon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
---------------------
"""

import serial
import time

import os
import argparse

import grbl_serial_version
import utils

from pathlib import Path

# -------------------------------------------------------------------
# Transfers a Gcode text to the CNC controller, waits to receive the answer
#  Returns:
#   data_sent : data sent to CNC Controller
#   data_received : CNC controller answer

def transfer_gcode(serial_port, line: str):
    line = line.strip() + '\n' # Strip all EOL characters from line for consistency
    data_sent = '- Sent: ' + line
    print (data_sent, end='')

    serial_port.write(str.encode(line)) # Send g-code block to grbl    


    data_received = serial_port.readline() # Wait for grbl response with carriage return

    data_received = '  Answer: ' + str(data_received.strip(),'utf-8')  + '\n'
    print (data_received)
    
    return(data_sent, data_received)

# -------------------------------------------------------------------
#Clear the Terminal
utils.clear_terminal()

# -------------------------------------------------------------------
# Parse command line args
parse_command_line_args = argparse.ArgumentParser(
                    description='Sends GCode to a GRBL based controller through a serial (USB) link. Input from Gcode file or keyboard. Exchange can be logged'
                    )
parse_command_line_args.add_argument("port", help="USB port name")
parse_command_line_args.add_argument('-f', '--file', help='Gcode file [directory/../name.extension], if None: input is from Keyboard')
parse_command_line_args.add_argument('-l', '--log', help='Logging of exchanges, to file log.log, stored in [directory] or in pwd (if no Gcode)')
command_line_args = parse_command_line_args.parse_args()

port = (command_line_args.port)
file = (command_line_args.file)
log = (command_line_args.log)

# -------------------------------------------------------------------
# Start parsing
print("\nTransfers GCode file to a GRBL based controler connected through a serial (USB) port")

print(f"\nScript Version: {grbl_serial_version.script_version}")
print(f"Script Date:    {grbl_serial_version.script_date}")
print(f"Script Author:  {grbl_serial_version.script_author}")

print(f"\n  USB port name: {port}")

if(file != None):
    directory, file_name = os.path.split(os.path.abspath(file))
    print(f"  GCode file:    {file}")
    print(f"      Directory:    {directory}")
    print(f"      File name:    {file_name}")
else:
    directory=r"."                      # Store in present pwd    
    print(f"  Input from Keyboard")

if(log != None):
    logfile = directory + r"/" + 'log.log'    
    print(f"  Logging enabled, stored in {logfile}")

input("\n\n  Press <Enter> to start.") 
utils.clear_terminal()

# -------------------------------------------------------------------
# Open grbl serial port
baudrate  = 115200

try:
    serial_port = serial.Serial(
        port = '/dev/' + port, 
        baudrate = baudrate,
        parity = serial.PARITY_NONE,
        stopbits = serial.STOPBITS_ONE,
        bytesize = serial.EIGHTBITS
    )

except Exception as error_io:
    error_message = (f"Port '/dev/{port}' open error:\n {error_io}")
    utils.error_function(error_text=error_message)


# -------------------------------------------------------------------
# Open g-code file, if added
try:
    if(file != None):
        grbl_file = open(file,'r')

except Exception as error_io:
    error_message = (f"File {file} open error:\n {error_io}")
    utils.error_function(error_text=error_message)

# -------------------------------------------------------------------
# Open logging file, if requested

try:
    if(log != None):        
        logging = open(logfile, 'w+')

except Exception as error_io:
    error_message = (f"Error creating 'log.log' file:\n {error_io}")
    utils.error_function(error_text=error_message)


# -------------------------------------------------------------------
# Wake up GRBL controller
print ('\n-------------------------------------------------')
print (  '  wait for CNC GRBL to initialize .....')

serial_port.write(str.encode('\r\n\r\n'))
time.sleep(2)   # Wait for grbl to initialize
serial_port.flushInput()  # Flush startup text in serial input

serial_port.write(str.encode('$I\n'))         # Request GRBL version
data_init_received = serial_port.readline()
data_init_received = '\n- GRBL version: ' + str(data_init_received.strip(),'utf-8')
print (data_init_received)
if(log != None):
    logging.write(data_init_received)
    logging.write('\n')

time.sleep(1)
serial_port.flushInput()  # Flush the ok from GRBL Controller

serial_port.write(str.encode('$P\n'))         # Request Pencil version
data_init_received = serial_port.readline()
data_init_received = '- PENCIL version: ' + str(data_init_received,'utf-8')
print (data_init_received)

if(log != None):
    logging.write(data_init_received)
    logging.write('\n-------------------------------------------------\n\n')

time.sleep(1)
serial_port.flushInput()  # Flush the ok from GRBL Controller

print ('  CNC GRBL initialization done .....')
print ('-------------------------------------------------\n\n')


# -------------------------------------------------------------------
# Stream g-code to CNC controller
if(file != None):
    # It is from File
    for line in grbl_file:
        data_tx, data_rx = transfer_gcode(serial_port = serial_port, line = line)

        # -------------------------------------------------------------------
        # Add to logging
        if(log != None):
            logging.write(data_tx)
            logging.write(data_rx + '\n')
else:
    # It is from Keyboard
    while True:
        print (      '--------------------------------- ')
        line = input("- Input Gcode, Q to exit: ") 

        if line[0].upper() == 'Q':
            # -------------------------------------------------------------------
            # end of input
            break

        # -------------------------------------------------------------------
        # Transfer
        print (' ')
        data_tx, data_rx = transfer_gcode(serial_port = serial_port, line = line)

        # -------------------------------------------------------------------
        # Add to logging
        if(log != None):
            logging.write(data_tx)
            logging.write(data_rx + '\n')


# -------------------------------------------------------------------
# GRBL is finished
if(log != None):
    logging.write('\n-------------------------------------------------\n  End of transfer ...')
    logging.write('\n-------------------------------------------------\n\n')

print ('\n-----------------------------------------')
input(   " Press <Enter> to exit and disable grbl.") 
print (  '-----------------------------------------\n')
# -------------------------------------------------------------------
# Close file and serial port
serial_port.close()

if(file != None):
    grbl_file.close()

if(log != None):
    logging.flush()
    logging.close()

