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
#ifndef __ARM_DATA_PROCESSING_H__
#define __ARM_DATA_PROCESSING_H__
#include <stdint.h>
#include <limits.h>
#include "util.h"
#include "arm_core.h"


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

int arm_data_processing_shift(arm_core p, uint32_t ins);
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);

#endif
