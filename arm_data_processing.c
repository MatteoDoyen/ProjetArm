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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	uint32_t result_value;

	uint8_t carry_bit = get_bit(ins, 28);

	uint8_t bit_i = get_bit(ins, 25);

	uint8_t opcode = get_bits(ins, 24, 21);

	uint8_t rn_register_number = get_bits(ins,19,16);
	uint32_t first_operand = arm_read_register(p, rn_register_number);

	uint8_t rd_register_number = get_bits(ins,15,12);

	uint32_t second_operand;

	if (bit_i == 0)
	{
		uint8_t bit_quatre = get_bit(ins, 4);
		uint8_t shift_type = get_bits(ins,6,5);

		uint8_t register_number = get_bits(ins,3,0);
		second_operand = arm_read_register(p, register_number);

		uint8_t shift_amount;

		if (bit_quatre == 0)
		{
			shift_amount = get_bits(ins,11,7);

		}
		else
		{
			uint8_t register_number = get_bits(ins,11,8);
			shift_amount = arm_read_register(p, register_number);
		}

		switch (shift_type)
		{
			case LSL:
				second_operand <<= shift_amount;
				break;
			case LSR:
				second_operand >>= shift_amount;
				break;
			case ASR:
				second_operand = asr(second_operand, shift_amount);
				break;
			case ROR:
				second_operand = ror(second_operand, shift_amount);
				break;
			default:
				break;
		}
	}
	else
	{
		second_operand = get_bits(ins,7,0);
		uint8_t rotate_value = get_bits(ins,11,8);

		second_operand = ror(second_operand, rotate_value);
	}

	//  write_register(registers r, uint8_t reg, uint32_t value)

	switch (opcode)
	{
		case AND:
			result_value = first_operand & second_operand;
			break;
		case EOR:
			result_value = first_operand ^ second_operand;
			break;
		case SUB:
			result_value = first_operand - second_operand;
			break;
		case RSB:
			result_value = second_operand - first_operand;
			break;
		case ADD:
			result_value = first_operand + second_operand;
			break;
		case ADC:
			// oupsi
			break;
		case SBC:
			// oupsi
			break;
		case RSC:
			// oupsi
			break;
		case TST:
			break;
		case TEQ:
			break;
		case CMP:
			break;
		case CMN:
			break;
		case ORR:
			result_value = first_operand | second_operand;
			break;
		case MOV:
			result_value = second_operand;
			break;
		case BIC:
			result_value = first_operand & ~second_operand;
			break;
		case MVN:
			result_value = ~second_operand;
			break;
		default:
			return UNDEFINED_INSTRUCTION;
			break;

	}

    return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}
