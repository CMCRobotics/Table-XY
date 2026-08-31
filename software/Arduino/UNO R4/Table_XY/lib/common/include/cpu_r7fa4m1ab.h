/*
  cpu_r7fa4m1ab.h - CPU configuration file for RENESAS RA4M1 Group

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

// Useful utils:
// AppData\Local\Arduino15\packages\arduino\hardware\renesas_uno\1.5.3\cores\arduino\api
//  common.h

#include "debug_cpu_r7fa4m1ab.h"

#ifndef cpu_r7fa4m1ab.h
#define cpu_r7fa4m1ab.h



//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Enable access to PFS registers

/*-----------------------------------------------------------
Bit         Symbol    Bit name                        Description
b5 to b0 —  Reserved
b6          PFSWE     PmnPFS Register Write Enable    0: Writing to the PmnPFS register disabled
                                                      1: Writing to the PmnPFS register enabled.
b7          B0WI      PFSWE Bit Write Disable         0: Writing to the PFSWE bit enabled
                                                      1: Writing to the PFSWE bit disabled.
-----------------------------------------------------------*/
#define PMISC_B0WI(value)   (R_PMISC->PWPR_b.B0WI = value)
#define PMISC_PFSWE(value)  (R_PMISC->PWPR_b.PFSWE = value )

#define PFS_WRITE_ENABLE    (PMISC_B0WI(0));(PMISC_PFSWE(1))
#define PFS_WRITE_DISABLE   (PMISC_B0WI(0));(PMISC_PFSWE(0));(PMISC_B0WI(1))

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Enable access to GPT registers

/*-----------------------------------------------------------
MSTPCRD: General PWM Timer Write-Protection Register

b0-b5 ....
b5    MSTPD5    General PWM Timer 32 0/1 Module Stop. 0: Cancel the module-stop state
b7-b31 ....
-----------------------------------------------------------*/
#define GPT32_PERIPHERAL_ENABLE (R_MSTP->MSTPCRD_b.MSTPD5 = 0)
#define GPT32_PERIPHERAL_DISABLE (R_MSTP->MSTPCRD_b.MSTPD5 = 1)

/*-----------------------------------------------------------
MSTPCRD: General PWM Timer Write-Protection Register

b0-b5 ....
b6    MSTPD6    General PWM Timer 167 to 162 Module Stop. 0: Cancel the module-stop state
b7-b31 ....
-----------------------------------------------------------*/
#define GPT16_PERIPHERAL_ENABLE (R_MSTP->MSTPCRD_b.MSTPD6 = 0)
#define GPT16_PERIPHERAL_DISABLE (R_MSTP->MSTPCRD_b.MSTPD6 = 1)

/*-----------------------------------------------------------
GTWP: General PWM Timer Write-Protection Register

b0       WP Register Write Disable  0: Write to the register enabled
b7-b1 —  Reserved
b15-b8   PRKEY[7:0] GTWP Key Code:  When A5h is written, writing to the WP bits is permitted
b31-b16 —  Reserved
-----------------------------------------------------------*/

#define GPT_REG_WRITE_ENABLE(ptr)  (ptr->GTWP_b.PRKEY=0xA5);(ptr->GTWP_b.WP=0)
#define GPT_REG_WRITE_DISABLE(ptr) (ptr->GTWP_b.PRKEY=0xA5);(ptr->GTWP_b.WP=1);(ptr->GTWP_b.PRKEY=0)


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Clock definitions for Peripherals
// As from Power-up:
//  PCLKA: %1
//  PCLKB: %2
//  PCLKC: %1
//  PCLKD: %1
//  ICK: %1
//  FCK: %2

#define CPU_CLOCK_PCLKA (F_CPU/1.0)
#define CPU_CLOCK_PCLKB (F_CPU/2.0)
#define CPU_CLOCK_PCLKD (((float)F_CPU)/1.0)

/*-----------------------------------------------------------
PRCR: Register Write protection

b0    PRC0 Protect Bit 0    Enables or disables writing to the registers related to the clock generation
                            0: Write disabled
                            1: Write enabled.
b1-b7 ....

b15 to b8 PRKEY[7:0] PRC    Key Code Control write access to the PRCR register. To modify the PRCR register,
                            write A5h to the upper 8 bits and the target value to the lower 8 bits as a 16-bit unit.
-----------------------------------------------------------*/
#define CPU_CLOCKDIV_ACCESS_ENABLE (R_SYSTEM->PRCR =0xA501)
#define CPU_CLOCKDIV_ACCESS_DISABLE (R_SYSTEM->PRCR =0xA500)

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Definitions for AGT1
#define AGT_INTERRUPT_EVENT_UNDERFLOW_CODE  (0x21)

#define AGT1_PERIPHERAL_ENABLE (R_MSTP->MSTPCRD_b.MSTPD2 = 0)
#define AGT1_PERIPHERAL_DISABLE (R_MSTP->MSTPCRD_b.MSTPD2 = 1)

#define AGT1_WAIT_WHILE_COUNTING while(R_AGT1->AGTCR_b.TCSTF == 1){__NOP();} // Wait while the counter is running
#define AGT1_STOP_COUNTER (R_AGT1->AGTCR_b.TSTOP = 1);AGT1_WAIT_WHILE_COUNTING

#define AGT1_COMPARE_MATCH_DISABLED (R_AGT1->AGTCMSR = 0)

#define AGT1_TCK_CODE (00)   // PCLKB Division %1
#define AGT1_CLOCK_MHZ  (0.000001 * CPU_CLOCK_PCLKB/1.0)

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Definitions for GPT320 (32 bit Timer 0)
#define GPT_INTERRUPT_EVENT_OVF_CODE (0x5D)    // IELSR code for Overflow
#define GPT_INTERRUPT_EVENT_CMPA_CODE (0x57)   // IELSR code for CMPA interrupt
#define GPT_INTERRUPT_EVENT_CMPB_CODE (0x58)   // IELSR code for CMPB interrupt

#define GPT320_TPCS_CODE (00)   // PCLKD Division %1
#define GPT320_CLOCK_MHZ  (0.000001 * CPU_CLOCK_PCLKD/1.0)

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Definitions for UART
// Mode:
//  If Normal mode is used:
//    SCI 9: Program communication
//    SCI 2: Not used
//
//  If Debug mode is used:
//    SCI 9: Debug exchange with IDE
//    SCI 2: Program communication

#ifndef SCI_MODE_DEBUG
  // Communication of the program is via SCI 9
  // This is the standard otupt path

  #define R_SCI R_SCI9

  #define UART_INTERRUPT_EVENT_RXI_CODE (0xA8)    // IELSR code for RX received
  #define UART_INTERRUPT_EVENT_TXI_CODE (0xA9)    // IELSR code for TX free

  #define UART_PERIPHERAL_ENABLE (R_MSTP->MSTPCRB_b.MSTPB22 = 0)      // SCI 9 Enabled

  #define UART_INTERRUPT_TXI_ENABLE  (R_SCI9->SCR_b.TIE = 1)  // TXI interruption Enabled
  #define UART_INTERRUPT_TXI_DISABLE (R_SCI9->SCR_b.TIE = 0)  // TXI interruption Disabled

  #define UART_TX_ENABLE  (R_SCI9->SCR_b.TE = 1)   // Transmition enabled
  #define UART_TX_DISABLE (R_SCI9->SCR_b.TE = 0)   // Transmition Disabled

  #define UART_RX_register  (R_SCI9->RDR)
  #define UART_TX_register  (R_SCI9->TDR)

  #define UART_RX_FULL_flag        (R_SCI9->SSR_b.RDRF)

  #define UART_TX_EMPTY_flag       (R_SCI9->SSR_b.TDRE)
  #define UART_TX_END_flag         (R_SCI9->SSR_b.TEND)

  #define UART_RX_FRAME_ERROR_flag   (R_SCI9->SSR_b.FER)
  #define UART_RX_OVR_ERROR_flag     (R_SCI9->SSR_b.ORER)
  #define UART_RX_PARITY_ERROR_flag  (R_SCI9->SSR_b.PER)

  #define UART_CPU_PORT  1
  #define UART_RXI_CPU_BIT   10  // Arduino Uno ESP J6 pin #3 [3v3]
  #define UART_TXI_CPU_BIT   9   // Arduino Uno ESP J6 pin #5 [3v3]

  #define UART_CPU_PMNFS_PSEL (0x05);  // SCI9

#else
  // Communication of the program is via SCI 2

  #define R_SCI R_SCI2

  #define UART_INTERRUPT_EVENT_RXI_CODE (0xA3)    // IELSR code for RX received
  #define UART_INTERRUPT_EVENT_TXI_CODE (0xA4)    // IELSR code for TX free

  #define UART_PERIPHERAL_ENABLE (R_MSTP->MSTPCRB_b.MSTPB29 = 0)      // SCI 2 Enabled

  #define UART_INTERRUPT_TXI_ENABLE  (R_SCI2->SCR_b.TIE = 1)  // TXI interruption Enabled
  #define UART_INTERRUPT_TXI_DISABLE (R_SCI2->SCR_b.TIE = 0)  // TXI interruption Disabled

  #define UART_TX_ENABLE  (R_SCI2->SCR_b.TE = 1)   // Transmition enabled
  #define UART_TX_DISABLE (R_SCI2->SCR_b.TE = 0)   // Transmition Disabled

  #define UART_RX_register  (R_SCI2->RDR)
  #define UART_TX_register  (R_SCI2->TDR)

  #define UART_RX_FULL_flag        (R_SCI2->SSR_b.RDRF)

  #define UART_TX_EMPTY_flag       (R_SCI2->SSR_b.TDRE)
  #define UART_TX_END_flag         (R_SCI2->SSR_b.TEND)

  #define UART_RX_FRAME_ERROR_flag   (R_SCI2->SSR_b.FER)
  #define UART_RX_OVR_ERROR_flag     (R_SCI2->SSR_b.ORER)
  #define UART_RX_PARITY_ERROR_flag  (R_SCI2->SSR_b.PER)

  #define UART_CPU_PORT  3
  #define UART_RXI_CPU_BIT   1    // Arduino Uno Digital pin #D0 [3v3]
  #define UART_TXI_CPU_BIT   2    // Arduino Uno Digital pin #D1 [3v3]

  #define UART_CPU_PMNFS_PSEL (0x04);  // SCI2
#endif

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Definitions for Interruptions
#define IELSRn_ptr	(0x40006300UL)

#endif

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Prototypes
