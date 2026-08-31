/*
  Several Utilities functions related to the CPU

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



/*
Version		Date			Author			Description
1.0			04/06/2026		Gomez Costa		Created
2.0			11/06/2026		Gomez Costa		Added:
												- Flash related functions
3.0			18/06/2026		Gomez Costa		Eliminated not used functions (overloaded):
												- configInterrupt()
4.0			27/06/2026		Gomez Costa		Eliminated Data Flash related functions, transferred to flash_cpu.cpp
4.2			27/06/2026		Gomez Costa		Eliminated default value in function declaration
5.0			01/07/2026		Gomez Costa		Test point definition separated in 3 functions
6.0			03/07/2026		Gomez Costa		Added delay_us and delay_ms
7.0			14/07/2026		Gomez Costa		SCI related functions use defined SCI port

*/


#include "utilities_cpu.h"


//===============================================================
// Get the first free place of IELSRn
// Return the index in the list
// Parameter:
//	print: boolean
//		true: Print list and index


u_int8_t getIdxIELSRn(boolean print_true) {
	u_int8_t idx;
	uint32_t regvalue;

	for(idx = 0; idx < 31; idx++){
		regvalue = *(u_int32_t *)(IELSRn_ptr + (4*idx));
		if (regvalue == 0UL)	break;	// Found a non installed
	}

	if(print_true)	{
		printCPURegisterValues((u_int32_t *)IELSRn_ptr, 32, "IELSRn ");
		Serial.print("\n\n Index: ");
		Serial.println(idx);
	}

	return idx;

}

/* -------------------------------------------------------------------------- */
// Add the Interrupt funtion to the Interrupt Table
// Return the precedent Interrupt vector
//
// Parameter:
//	pfunc: Interrupt function
//	irq_number: number of the IELSR register
//			See RA4M1, 13 Interrupt Controller Unit, Table 13.3
//	irq_priority: Priority assigned
//	print_true: boolean
//		true: Print list and index


uint32_t configInterrupt(void (*pfunc)(),  u_int32_t irq_number, uint32_t irq_priority, uint32_t irq_event_code,boolean print_true ) {
		volatile uint32_t *irq_ptr = (volatile uint32_t *)SCB->VTOR;
		uint32_t return_value = 0;
		u_int32_t irq_ptr_offset;

		R_ICU->IELSR_b[irq_number].DTCE = 0;    // DTCE disabled
		R_ICU->IELSR_b[irq_number].IR = 0;      // Clear Interruption Flag
		R_ICU->IELSR_b[irq_number].IELS = irq_event_code;


		//	irq_ptr_offset: offset in the vector table (module 4).
		//			See RA4M1, 13 Interrupt Controller Unit, Table 13.3
		//			0x040: Offset for IELSR0
		irq_ptr_offset = ((0x040UL)/4) + irq_number;

		__disable_irq();
		return_value =  *(irq_ptr + irq_ptr_offset);
		*(irq_ptr + irq_ptr_offset) = (uint32_t)pfunc;
		__enable_irq();

		R_BSP_IrqCfgEnable ((IRQn_Type const) irq_number, irq_priority, NULL);

		// IRQn_Type const _irq = (IRQn_Type const) irq_number;
		// __COMPILER_BARRIER();
		// NVIC->ISER[(_irq >> 5UL)] = (uint32_t) (1UL << (_irq & 0x1FUL));
		// __COMPILER_BARRIER();

		if(print_true){
		Serial.print("\n irq ptr offset: 0x");	Serial.println((uint32_t)irq_ptr_offset,HEX);
		Serial.print(  " irq ptr: 0x");   		Serial.println((uint32_t)*irq_ptr,HEX);
		Serial.print(  " Table Address: 0x");	Serial.println((uint32_t)*(irq_ptr + irq_ptr_offset),HEX);
		Serial.print(  " Ptr to function: 0x");	Serial.println((uint32_t)pfunc,HEX);
		Serial.print(  " Return Value: 0x");	Serial.println(return_value,HEX);

		printCPURegisterValues((u_int32_t *)SCB->VTOR, 40, "Interrupt Table ");

		}
		return return_value;
	}

//===============================================================
// Prints desired values of CPU Registers
//
// 	reg_ptr: pointer to first register
//	number: number of Registers
//			[1 ... ]
//	text: text to display in each line

void printCPURegisterValues(u_int32_t *reg_ptr, int number, char *text)
{
u_int32_t regvalue;
u_int32_t * pointer;

uint8_t idx;
char banner[80];

strcpy(banner, "--- ");
strcat(banner, text);
strcat(banner, " --- address: 0x0");

Serial.println("");

for(idx = 0; idx < number; idx++){
	pointer = reg_ptr + (idx);
	regvalue = *(u_int32_t *)(pointer);
	Serial.print(banner);
	Serial.print((u_int32_t)pointer, HEX);
	Serial.print("  ");
	Serial.print(idx);
	Serial.print(" : 0x0");
	Serial.print(regvalue, HEX);
	Serial.println("");
}

}

void printCPURegisterValues(u_int16_t *reg_ptr, int number, char *text)
{
uint16_t regvalue;
uint8_t idx;
u_int16_t * pointer;

char banner[80];

strcpy(banner, "--- ");
strcat(banner, text);
strcat(banner, " --- address: 0x0");

Serial.println("");

for(idx = 0; idx < number; idx++){
	pointer = reg_ptr + (idx);
	regvalue = *(u_int16_t *)(pointer);
	Serial.print(banner);
	Serial.print((u_int32_t)pointer, HEX);
	Serial.print("  ");
	Serial.print(idx);
	Serial.print(" : 0x0");
	Serial.print(regvalue, HEX);
	Serial.println("");
}

}

void printCPURegisterValues(u_int8_t *reg_ptr, int number, char *text){
uint8_t regvalue;
uint8_t idx;
u_int8_t * pointer;

char banner[80];

strcpy(banner, "--- ");
strcat(banner, text);
strcat(banner, " --- address: 0x0");

Serial.println("");

for(idx = 0; idx < number; idx++){
	pointer = reg_ptr + (idx);
	regvalue = *(u_int16_t *)(pointer);
	Serial.print(banner);
	Serial.print((u_int32_t)pointer, HEX);
	Serial.print("  ");
	Serial.print(idx);
	Serial.print(" : 0x0");
	Serial.print(regvalue, HEX);
	Serial.println("");
}

}

//-----------------------------------------------------------------------------
// Initialices the Test pins for Arduino Uno R4
// Name		Pin
// TEST_1 	Analog Pin A4
// TEST_2 	Analog Pin A5
// TEST_3 	Digital Pin D13

void debugPinOutputTest_1_init(void) {

	// Output pin
	PFS_WRITE_ENABLE;

	R_PMISC->PWPR_b.B0WI = 1;
	R_PMISC->PWPR_b.PFSWE = 1;

	// Debug pins
	// Init all bits to 0, standard setting
	R_PFS->PORT[TEST_1_CPU_PORT].PIN[TEST_1_CPU_BIT].PmnPFS = 0UL;
	R_PFS->PORT[TEST_1_CPU_PORT].PIN[TEST_1_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

	 PFS_WRITE_DISABLE;
  }

  void debugPinOutputTest_2_init(void) {

	// Output pin
	PFS_WRITE_ENABLE;

	R_PMISC->PWPR_b.B0WI = 1;
	R_PMISC->PWPR_b.PFSWE = 1;

	// Debug pins
	// Init all bits to 0, standard setting
	R_PFS->PORT[TEST_2_CPU_PORT].PIN[TEST_2_CPU_BIT].PmnPFS = 0UL;
	R_PFS->PORT[TEST_2_CPU_PORT].PIN[TEST_2_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

	PFS_WRITE_DISABLE;
  }


  void debugPinOutputTest_3_init(void) {


	// Output pin
	PFS_WRITE_ENABLE;

	R_PMISC->PWPR_b.B0WI = 1;
	R_PMISC->PWPR_b.PFSWE = 1;

	// Debug pins
	// Init all bits to 0, standard setting

	R_PFS->PORT[TEST_3_CPU_PORT].PIN[TEST_3_CPU_BIT].PmnPFS = 0UL;
	R_PFS->PORT[TEST_3_CPU_PORT].PIN[TEST_3_CPU_BIT].PmnPFS_b.PDR = 1;  //Output

	 PFS_WRITE_DISABLE;
  }


//-----------------------------------------------------------------------------
// Inline function for Enabling SCI (2 or 9) TXI interruption
// Process as detailed in (Renesas RA4M1 Group User’s Manual: Hardware, 28.3.3 (1), page 748)

void EnableTXIinterruption (void){

	R_SCI->SCR = (R_SCI->SCR) & 0x5F;  // Clear TE and TIE
	__NOP(); __NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP(); __NOP();
	R_SCI->SCR = (R_SCI->SCR) | 0xA0;  // Set TE and TIE
	__NOP(); __NOP(); __NOP(); __NOP();
}

//-----------------------------------------------------------------------------
// Create Software delay
//

void _delay_ms(unsigned int ms){
	_delay_us_cpu ((uint32_t) (1000 * ms));
}

void _delay_us(unsigned int us){
	_delay_us_cpu ((uint32_t) us);
}
/*******************************************************************************************************************//**
 * Delay for the given number of micro seconds at the given frequency
 *
 * Copied from: r_flash_lp_delay_us(...)
 *
 * This is used instead of R_BSP_SoftwareDelay because that may be linked in code flash.
 *
 * us              Number of microseconds to delay
 * mhz             The frequency of the system clock
 **********************************************************************************************************************/

#define FLASH_LP_DELAY_LOOP_CYCLES	(4U)
#define FREQ_MHZ (F_CPU/1000000)

 static void _delay_us_cpu (uint32_t us)
{
    uint32_t loop_cnt;

    // @12 MHz, one loop is 332 ns. A delay of 5 us would require 15 loops. 15 * 332 = 4980 ns or ~ 5us

    /* Calculation of a loop count */
    loop_cnt = ((us * FREQ_MHZ) / FLASH_LP_DELAY_LOOP_CYCLES);

    if (loop_cnt > 0U)
    {
        __asm volatile ("delay_loop:\n"
#if defined(__ICCARM__) || defined(__ARMCC_VERSION) || (defined(__llvm__) && !defined(__CLANG_TIDY__))
                        "   subs %[loops_remaining], #1         \n"                 ///< 1 cycle
#elif defined(__GNUC__)
                        "   sub %[loops_remaining], %[loops_remaining], #1      \n" ///< 1 cycle
#endif
                        "cmp %[loops_remaining], #0\n"                              // 1 cycle

/* CM0 and CM23 have different instruction sets */
#if defined(__CORE_CM0PLUS_H_GENERIC) || defined(__CORE_CM23_H_GENERIC)
                        "   bne delay_loop   \n"                                    ///< 2 cycles
#else
                        "   bne.n delay_loop \n"                                    ///< 2 cycles
#endif
                        :                                                           // No outputs
                        :[loops_remaining] "r" (loop_cnt)
                        :                                                           // No clobbers
                        );
    }
}
