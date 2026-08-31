/*
  serial.c - Low level functions for sending and recieving bytes via the serial port
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

#include <grbl.h>

volatile uint8_t serial_rx_buffer[RX_BUFFER_SIZE];
volatile char serial_tx_buffer[TX_BUFFER_SIZE + 1];

volatile uint16_t serial_rx_buffer_head = 0;
volatile uint16_t serial_rx_buffer_tail = 0;

volatile uint16_t serial_tx_idx;
volatile boolean serial_tx_busy = false;

#ifdef ENABLE_XONXOFF
  volatile uint8_t flow_ctrl = XON_SENT; // Flow control state variable
#endif



//---------------------------------------------------------------------------------------------------------------
//UART initialisations

void serial_init()
{
  //UART pins
  PFS_WRITE_ENABLE;

  // Define Bit as UART
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_TXI_CPU_BIT].PmnPFS = 0UL;          // Init all bits to 0, standard setting
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_TXI_CPU_BIT].PmnPFS_b.PODR = 1;     // Output high
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_TXI_CPU_BIT].PmnPFS_b.PDR = 1;      // Output
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_TXI_CPU_BIT].PmnPFS_b.PMR = 1;      // Peripheral
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_TXI_CPU_BIT].PmnPFS_b.PSEL = UART_CPU_PMNFS_PSEL;  // TXD

  R_PFS->PORT[UART_CPU_PORT].PIN[UART_RXI_CPU_BIT].PmnPFS = 0UL;          // Init all bits to 0, standard setting
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_RXI_CPU_BIT].PmnPFS_b.PDR = 0;      // Input
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_RXI_CPU_BIT].PmnPFS_b.PMR = 1;      // Peripheral
  R_PFS->PORT[UART_CPU_PORT].PIN[UART_RXI_CPU_BIT].PmnPFS_b.PSEL = UART_CPU_PMNFS_PSEL;  // RXD

  PFS_WRITE_DISABLE;

  // SCI Registers

  UART_PERIPHERAL_ENABLE;

  R_SCI->SCR= 0;         // Clear Register
  R_SCI->FCR_b.FM  = 0;  // No FIFO
  R_SCI->SCR_b.CKE = 0;  // On chip clock

  R_SCI->SIMR1_b.IICM = 0; // No I2C
  R_SCI->SCMR_b.SMIF = 0 ; // No Smart card
  R_SCI->SPMR = 0;         // No SS/CTS pins, Master select, Clock not inverted/delayed

  R_SCI->SCMR_b.SINV = 0;    // TDR/RDR streams not inverted
  R_SCI->SCMR_b.SDIR = 0;    // Standard stream, LSB first
  R_SCI->SCMR_b.CHR1 = 1;    // Length 8 bit (see SMR.CHR)

  R_SCI->SEMR = 0;           // Noise cancellation disabled / RXD low level -> start bit
                              // Clock related settings defined below

  R_SCI->SMR = 0;            // Length 8 bit (see SCMR.CH1) / No parity / asynchronous

  R_SCI->SPTR_b.SPB2DT = 1;  // TXD output is high, when UART is disabled
  R_SCI->SPTR_b.SPB2IO = 1;  // Enable previous setting

  //UART clock for 115200 baud rate
  // PCLKA: 48MHz
  // see {Table XY pinout UNO R4.ods} and page 726 of Renesas RA4M1 Group User’s Manual: Hardware, for formula

  R_SCI->SCMR_b.BCP2 = 1;    // Clock settings
  R_SCI->SMR_b.CKS = 0;      // PCLKA n = 0
  R_SCI->SEMR_b.BRME = 1;    // Bit Rate modulation
  R_SCI->SEMR_b.ABCSE = 0;   // Clock cycle defined by BGDM and ABCS
  R_SCI->SEMR_b.ABCS = 0;    // 16 base clock cycles for 1 bit period
  R_SCI->SEMR_b.BGDM = 0;    // Clock single frequency

  R_SCI->BRR = 11 ;
  R_SCI->MDDR = 236;

  //Set Interruption
  configInterrupt(ISR_UART_Reception, UART_INTERRUPT_EVENT_RXI_IELSRindex, UART_INTERRUPT_EVENT_PRIORITY_RXI, UART_INTERRUPT_EVENT_RXI_CODE, false);
  configInterrupt(ISR_UART_Transmission, UART_INTERRUPT_EVENT_TXI_IELSRindex, UART_INTERRUPT_EVENT_PRIORITY_TXI, UART_INTERRUPT_EVENT_TXI_CODE, false);

  R_SCI->SCR_b.RIE = 1;  // RXI interruption Enabled
  UART_INTERRUPT_TXI_DISABLE;  // TXI interruption disabled

  R_SCI->SCR_b.RE = 1;   // Reception enabled
  // UART_TX_ENABLE;    // Transmition enabled

  serial_tx_busy = false;

  // enable interrupt on complete reception of a byte
  // defaults to 8-bit, no parity, 1 stop bit
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// Write Functions

//--------------------------------------------------------------------------------------------
// Writes the FULL string
// If another print is required before the precedent has not been sent, it will wait till done
// Only ABORT is checked

void serial_write(char *string_data) {

  // Wait until previous has been transferred
  while (serial_tx_busy) {
    // TODO: Restructure st_prep_buffer() calls to be executed here during a long print.
    if (sys.rt_exec_state & EXEC_RESET) { return; } // Only check for abort to avoid an endless loop.
  }

  // Copy to Transfer memory
  strncpy( (char *) serial_tx_buffer, (const char *) string_data, TX_BUFFER_SIZE );
  serial_tx_buffer[TX_BUFFER_SIZE] = 00;    // To be sure if the lenght is >= than TX_BUFFER_SIZE

  serial_tx_busy = true;
  serial_tx_idx = 0;

  // Enable Data Register Empty Interrupt to make sure tx-streaming is running
    EnableTXIinterruption();
}

//---------------------------------------------------------------------------------------------------
// Data Register Empty Interrupt handler
void ISR_UART_Transmission(void)
{
  #ifdef ENABLE_XONXOFF
    if (flow_ctrl == SEND_XOFF) {
      UART_TX_register = XOFF_CHAR;
      flow_ctrl = XOFF_SENT;
    } else if (flow_ctrl == SEND_XON) {
      UART_TX_register = XON_CHAR;
      flow_ctrl = XON_SENT;
    } else
  #endif
  {
    if (serial_tx_buffer[serial_tx_idx] != 0){
      // Send a byte from the buffer
      UART_TX_register = serial_tx_buffer[serial_tx_idx++];

    } else{
      // Wait till last byte has been transferred
      while ( R_SCI->SSR_b.TEND == 0){__NOP();};

      UART_INTERRUPT_TXI_DISABLE;  // TXI interruption Disabled
      serial_tx_busy = false;
    }
  }

  R_ICU->IELSR_b[UART_INTERRUPT_EVENT_TXI_IELSRindex].IR = 0;   // Clear Interruption Flag

}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// Read functions

// Returns the number of bytes used in the RX serial buffer.
uint16_t serial_get_rx_buffer_count()
{
  uint16_t rtail = serial_rx_buffer_tail; // Copy to limit multiple calls to volatile
  if (serial_rx_buffer_head >= rtail) { return(serial_rx_buffer_head-rtail); }
  return (RX_BUFFER_SIZE - (rtail-serial_rx_buffer_head));
}

//----------------------------------------------------------------------------
// Fetches the first byte in the serial read buffer. Called by main program.
uint8_t serial_read(){
  uint16_t tail = serial_rx_buffer_tail; // Temporary serial_rx_buffer_tail (to optimize for volatile)
  if (serial_rx_buffer_head == tail) {
    return SERIAL_NO_DATA;
  } else {
    uint8_t data = serial_rx_buffer[tail];

    tail++;
    if (tail == RX_BUFFER_SIZE) { tail = 0; }
    serial_rx_buffer_tail = tail;

    #ifdef ENABLE_XONXOFF
      if ((serial_get_rx_buffer_count() < RX_BUFFER_LOW) && flow_ctrl == XOFF_SENT) {
        flow_ctrl = SEND_XON;
        // UCSR0B |=  (1 << UDRIE0); // Force TX
      }
    #endif

    return data;
  }
}


//----------------------------------------------------------------------------
void serial_reset_read_buffer()
{
  serial_rx_buffer_tail = serial_rx_buffer_head;

  #ifdef ENABLE_XONXOFF
    flow_ctrl = XON_SENT;
  #endif
}

//----------------------------------------------------------------------------
// Reception of Serial data

void ISR_UART_Reception(void)
{
  uint8_t data = UART_RX_register;
  uint16_t next_head;

  // Pick off realtime command characters directly from the serial stream. These characters are
  // not passed into the buffer, but these set system state flag bits for realtime execution.
  switch (data) {
    case CMD_STATUS_REPORT: bit_true_atomic(sys.rt_exec_state, EXEC_STATUS_REPORT); break; // Set as true
    case CMD_CYCLE_START:   bit_true_atomic(sys.rt_exec_state, EXEC_CYCLE_START); break; // Set as true
    case CMD_FEED_HOLD:     bit_true_atomic(sys.rt_exec_state, EXEC_FEED_HOLD); break; // Set as true
    case CMD_SAFETY_DOOR:   bit_true_atomic(sys.rt_exec_state, EXEC_SAFETY_DOOR); break; // Set as true
    case CMD_RESET:         mc_reset(); break; // Call motion control reset routine.

    default: // Write character to buffer

      next_head = serial_rx_buffer_head + 1;
      if (next_head == RX_BUFFER_SIZE) { next_head = 0; }

      // Write data to buffer unless it is full.
      if (next_head != serial_rx_buffer_tail) {
        serial_rx_buffer[serial_rx_buffer_head] = data;
        serial_rx_buffer_head = next_head;

        #ifdef ENABLE_XONXOFF
          if ((serial_get_rx_buffer_count() >= RX_BUFFER_FULL) && flow_ctrl == XON_SENT) {
            flow_ctrl = SEND_XOFF;
            // UCSR0B |=  (1 << UDRIE0); // Force TX
          }
        #endif

        break;
      }
      //TODO: else alarm on overflow?
  };

  R_ICU->IELSR_b[UART_INTERRUPT_EVENT_RXI_IELSRindex].IR = 0;   // Clear Interruption Flag

}
