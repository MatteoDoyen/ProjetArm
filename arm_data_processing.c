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
#include "arm_utils.h"
#include "debug.h"
#include <limits.h>

static inline int borrow_from(uint32_t operand_a, uint32_t operand_b)
{
	return (operand_a - operand_b) < 0;
}

// manual page 1131 "OverflowFrom" function
static inline int overflow_from(uint32_t operand_a, uint32_t operand_b, uint64_t result, uint8_t op_type) {
	// if op_type is an addition
	if (op_type) 
	{
		return get_bit(operand_a, 31) == get_bit(operand_b, 31) 
			&& get_bit(operand_a, 31) != get_bit(result, 31) 
			&& get_bit(operand_b, 31) != get_bit(result, 31);
	}
	// else op_type is a substraction
	return get_bit(operand_a, 31) != get_bit(operand_b, 31)
		&& get_bit(operand_a, 31) != get_bit(result, 31);
}

// manual page 1124 "CarryFrom" function
static inline int carry_from(uint32_t result) {
	return result > UINT_MAX;
}

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	int32_t result_value;

	uint32_t flags = arm_read_cpsr(p);

	uint8_t carry_bit = get_bit(flags, C);
	uint8_t overflow_bit = get_bit(flags, V);

	uint8_t shift_carry;

	uint8_t bit_i = get_bit(ins, 25);

	uint8_t opcode = get_bits(ins, 24, 21);

	uint8_t S = get_bit(ins, 20);

	uint8_t rn_register_number = get_bits(ins,19,16);
	uint32_t first_operand = arm_read_register(p, rn_register_number);

	uint8_t rd_register_number = get_bits(ins,15,12);

	uint32_t second_operand;

	uint8_t shift_amount;
	uint8_t shift_type;
	uint8_t immediate_shift;

	if (bit_i == 0)
	{
		uint8_t bit_quatre = get_bit(ins, 4);
		
		shift_type = get_bits(ins,6,5);

		uint8_t register_number = get_bits(ins,3,0);
		second_operand = arm_read_register(p, register_number);

		if (bit_quatre == 0)
		{
			shift_amount = get_bits(ins,11,7);
		}
		else
		{
			uint8_t register_shift_number = get_bits(ins,11,8);
			shift_amount = arm_read_register(p, register_shift_number);
		}
	}
	else
	{
		immediate_shift = 1;
		second_operand = get_bits(ins,7,0);
		shift_amount = get_bits(ins,11,8);
		shift_type = ROR;
	}

	second_operand = arm_decode_shift(p, shift_type, second_operand, shift_amount, &shift_carry, immediate_shift);

	switch (opcode)
	{
		case AND:
			result_value = first_operand & second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					shift_carry,
					overflow_bit
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case EOR:
			result_value = first_operand ^ second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					shift_carry,
					overflow_bit
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case SUB:
			result_value = first_operand - second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					!borrow_from(first_operand, second_operand),
					overflow_from(first_operand, second_operand, result_value, 0)
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case RSB:
			result_value = second_operand - first_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					!borrow_from(second_operand, first_operand),
					overflow_from(second_operand, first_operand, result_value, 0)
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case ADD:
			result_value = first_operand + second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					carry_from(result_value),
					overflow_from(first_operand, second_operand, result_value, 1)
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case ADC:
			result_value = first_operand + second_operand + carry_bit;

			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					carry_from(result_value),
					overflow_from(first_operand, second_operand+carry_bit, result_value, 1)
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case SBC:
			result_value = first_operand - second_operand - !carry_bit;
            if(S && rd_register_number == 15)
            {
                if (arm_current_mode_has_spsr(p))
				{
				     arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
				     //UNPREDICTABLE
				}
            }
            else if(S)
            {
                arm_update_flags(
                	p,
                    get_bit(result_value, 31),
                    (result_value == 0),
                    !borrow_from(first_operand, second_operand - !carry_bit),
                    overflow_from(first_operand, second_operand - !carry_bit, result_value, 0)
                );
            }
            arm_write_register(p, rd_register_number, result_value);
			break;
		case RSC:
			result_value = second_operand - first_operand - !carry_bit;
            if(S && rd_register_number == 15)
            {
                if (arm_current_mode_has_spsr(p))
				{
				     arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
				     //UNPREDICTABLE
				}
            }
            else if(S)
            {
                arm_update_flags(
                	p,
                    get_bit(result_value, 31),
                    (result_value == 0),
                    !borrow_from(second_operand, first_operand - !carry_bit),
                    overflow_from(second_operand,first_operand - !carry_bit, result_value, 0)
                );
            }
            arm_write_register(p, rd_register_number, result_value);
			break;
		case TST:
			 arm_update_flags(
			 	p,
                get_bit(first_operand & second_operand, 31),
                ((first_operand & second_operand) == 0),
                shift_carry,
                overflow_bit
            );
			break;
		case TEQ:
			arm_update_flags(
				p,
                get_bit(first_operand ^ second_operand, 31),
                ((first_operand ^ second_operand) == 0),
                shift_carry,
                overflow_bit
            );
			break;
		case CMP:
			arm_update_flags(
				p,
	            get_bit(first_operand - second_operand, 31),
	            ((first_operand - second_operand) == 0),
	            !borrow_from(first_operand, second_operand),
	            overflow_from(first_operand,second_operand,first_operand - second_operand,0)
	       	);
			break;
		case CMN:
			arm_update_flags(
				p,
				get_bit(first_operand+second_operand, 31),
				((first_operand+second_operand) == 0),
				carry_from(first_operand+second_operand),
				overflow_from(first_operand, second_operand, first_operand+second_operand, 1)
			);
			break;
		case ORR:
			result_value = first_operand | second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					shift_carry,
					overflow_bit
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case MOV:
			result_value = second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					shift_carry,
					overflow_bit
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case BIC:
			result_value = first_operand & ~second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					shift_carry,
					overflow_bit
				);
			}
			arm_write_register(p, rd_register_number, result_value);
			break;
		case MVN:
			result_value = ~second_operand;
			if (S && rd_register_number == 15) 
			{
				if (arm_current_mode_has_spsr(p))
				{
					arm_write_cpsr(p, arm_read_spsr(p));
				}
				else
				{
					//UNPREDICTABLE
				}
			}
			else if (S)
			{
				arm_update_flags(
					p,
					get_bit(result_value, 31),
					(result_value == 0),
					shift_carry,
					overflow_bit
				);
			}
			arm_write_register(p, rd_register_number, result_value);
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
