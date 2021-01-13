/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"
#include "debug.h"

// Not supported below ARMv6, should read as 0
#define CP15_reg1_EEbit 0

#define Exception_bit_9 (CP15_reg1_EEbit << 9)

#define High_vector_address_type 0
#define High_vector_address 0xFFFF0000
#define Normal_vector_address 0x00000000

#define reset_mask 0x1d3
#define undefined_mask 0x9b
#define software_interrupt_mask 0x93
#define prefetch_abort_data_abort_mask 0x197
#define interrupt_mask 0x1a2
#define fast_interrupt_mask 0x1d1

static void vector_address_type(arm_core p, int8_t address) {
	uint32_t address_32_bit = address;

	if (High_vector_address_type)
	{
		address_32_bit |= High_vector_address;
		arm_write_register(p, 15, address_32_bit);
		return;
	}
	address_32_bit |= Normal_vector_address;
	arm_write_register(p, 15, address_32_bit);
}

static int current_instruction_address(arm_core p)
{
	// in ARM mode the PC is two instructions ahead of the current instruction
	// We substract 8 to the pc value to get the current instruction
	return arm_read_register(p, 15) - 8;
}

static int next_instruction_address(arm_core p)
{
	// in ARM mode the PC is two instructions ahead of the current instruction
	// We substract 4 to the pc value to get the next instruction
	return arm_read_register(p, 15) - 4;
}

static void reset_exception(arm_core p, uint32_t cpsr) {
	arm_write_cpsr(p, reset_mask | (cpsr & 0xFFFFFC00) | Exception_bit_9);
	// write pc address
	vector_address_type(p, 0x0000);
}

static void undefined_exception(arm_core p, uint32_t cpsr) {
	arm_write_spsr(p, cpsr); // save cpsr in spsr_und
	arm_write_cpsr(p, undefined_mask | (cpsr & 0xFFFFFD40) | Exception_bit_9);
	arm_write_register(p, 14, next_instruction_address(p));

	// write pc address
	vector_address_type(p, 0x0004);
}

static void software_interrupt_exception(arm_core p, uint32_t cpsr) {
	arm_write_spsr(p, cpsr);
	arm_write_cpsr(p, software_interrupt_mask | (cpsr & 0xFFFFFD40) | Exception_bit_9);
	arm_write_register(p, 14, next_instruction_address(p));

	// write pc address
	vector_address_type(p, 0x0008);
}

static void prefetch_abort_exception(arm_core p, uint32_t cpsr) {
	arm_write_spsr(p, cpsr);
	arm_write_cpsr(p, prefetch_abort_data_abort_mask | (cpsr & 0xFFFFFC40) | Exception_bit_9);
	arm_write_register(p, 14, current_instruction_address(p) + 4); // replace by next_instructions() ?

	// write pc address
	vector_address_type(p, 0x000C);
}

static void data_abort_exception(arm_core p, uint32_t cpsr) {
	arm_write_spsr(p, cpsr);
	arm_write_cpsr(p, prefetch_abort_data_abort_mask | (cpsr & 0xFFFFFC40) | Exception_bit_9);
	arm_write_register(p, 14, current_instruction_address(p) + 8); // replace by PC's value ?

	// write pc address
	vector_address_type(p, 0x0010);
}

static void irq_exception(arm_core p, uint32_t cpsr) {

	// if IRQ are disabled
	if (get_bit(cpsr, 7))
	{
		return;
	}
	arm_write_spsr(p, cpsr);
	arm_write_cpsr(p, interrupt_mask | (cpsr & 0xFFFFFC40) | Exception_bit_9);
	arm_write_register(p, 14, next_instruction_address(p) + 4); // replace by PC's value ?

	// write pc address
	vector_address_type(p, 0x0018);
}

static void fast_interrupt_exception(arm_core p, uint32_t cpsr) {

	// if FIQ are disabled
	if (get_bit(cpsr, 6))
	{
		return;
	}

	arm_write_spsr(p, cpsr);
	arm_write_cpsr(p, fast_interrupt_mask | (cpsr & 0xFFFFFC00) | Exception_bit_9);
	arm_write_register(p, 14, next_instruction_address(p) + 4); // replace by PC's value ?

	// write pc address
	vector_address_type(p, 0x001C);
}



void arm_exception(arm_core p, unsigned char exception) {

	uint32_t old_cpsr = arm_read_cpsr(p);

	switch (exception) {
		case RESET:
			debug("RESET exception\n");
			reset_exception(p, old_cpsr);
			break;
		case UNDEFINED_INSTRUCTION:
			debug("UND exception\n");
			undefined_exception(p, old_cpsr);
			break;
		case SOFTWARE_INTERRUPT:
			debug("SOFTWARE_INTERRUPT exception\n");
			software_interrupt_exception(p, old_cpsr);
			break;
		case PREFETCH_ABORT:
			debug("PREFETCH_ABORT exception\n");
			prefetch_abort_exception(p, old_cpsr);
			break;
		case DATA_ABORT:
			debug("DATA_ABORT exception\n");
			data_abort_exception(p, old_cpsr);
			break;
		case INTERRUPT:
			debug("INTERRUPT exception\n");
			irq_exception(p, old_cpsr);
			break;
		case FAST_INTERRUPT:
			debug("FAST_INTERRUPT exception\n");
			fast_interrupt_exception(p, old_cpsr);
			break;
	}
}
