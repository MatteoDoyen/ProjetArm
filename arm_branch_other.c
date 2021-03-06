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
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include "arm_utils.h"
#include <stdlib.h>

int arm_branch(arm_core p, uint32_t ins)
{
    int32_t address = get_bits(ins, 23, 0);
    uint32_t pc = arm_read_register(p, 15);

        // On teste si le bit de poids fort est négatif
        // S'il est négatif, on étend l'adresse sur 30 bits avec des 1
    int negatif = get_bit(address, 23);
    if (negatif) {
        address = 0x3F000000 | address;
    } else {
        address = 0x00000000 | address;
    }

        // Le decalage de deux bits à gauche permet de multiplier la valeur par 4
    address = address << 2;
        // Complément à 1
    address = ~address;
        // Complément à 2
    address = address + 1;

    int L = get_bit(ins, 24);
    if (L)
    {
        arm_write_register(p, 14, pc - 4);
    }

    if (negatif)
    {
        arm_write_register(p, 15, pc - address);
    }
    else
    {
        arm_write_register(p, 15, pc + address);
    }

    return 0;
}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins)
{
    if (get_bit(ins, 24))
    {
        /* Here we implement the end of the simulation as swi 0x123456 */
        if ((ins & 0xFFFFFF) == 0x123456)
            exit(0);
        return SOFTWARE_INTERRUPT;
    }
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins)
{
    if (get_bit(ins, 21)) {
        // MSR
        uint32_t operand = 0;
        if (get_bit(ins, 25)) {
            int8_t immediate = get_bits(ins, 7, 0);
            int8_t rotate_imm = get_bits(ins, 11, 8);
            operand = ror(immediate, (rotate_imm << 1));
        } else {
            uint8_t rm = get_bits(ins, 3, 0);
            operand = arm_read_register(p, rm);
        }

        if ((operand & UnallocMask) != 0) {
            return -1;
        }

        uint32_t mask = 0;
        uint32_t byte_mask = (get_bit(ins, 16) ? 0x000000FF : 0) |
                             (get_bit(ins, 17) ? 0x0000FF00 : 0) |
                             (get_bit(ins, 18) ? 0x00FF0000 : 0) |
                             (get_bit(ins, 19) ? 0xFF000000 : 0);

        if (!get_bit(ins, 22)) {
            if (arm_in_a_privileged_mode(p)) {
                if ((operand & StateMask) != 0) {
                    return -1;
                } else {
                    mask = byte_mask & (UserMask | PrivMask);
                }
            } else {
                mask = byte_mask & UserMask;
            }
            arm_write_cpsr(p, (arm_read_cpsr(p) & ~mask) | (operand & mask));
        } else {
            if (arm_current_mode_has_spsr(p)) {
                mask = byte_mask & (UserMask | PrivMask | StateMask);
                arm_write_spsr(p, (arm_read_spsr(p) & ~mask) | (operand & mask));
            } else {
                return -1;
            }
        }

        return 0;
    } else {
        // MRS
        int reg = get_bits(ins, 15, 12);
        if (reg == 15)
        {
            return -1;
        }
        else
        {
            int R = get_bit(ins, 22);
            if (R)
            {
                arm_write_register(p, reg, arm_read_spsr(p));
            }
            else
            {
                arm_write_register(p, reg, arm_read_cpsr(p));
            }
            return 0;
        }

        return -1;
    }
}
