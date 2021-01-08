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
#include <debug.h>
#include <stdlib.h>

#define PASSED 1
#define UNPASSED 0

int arm_branch(arm_core p, uint32_t ins)
{
    int conditionPassed = UNPASSED;
    uint32_t flags = arm_read_cpsr(p);
    int flag_N = get_bit(flags, N);
    int flag_Z = get_bit(flags, Z);
    int flag_C = get_bit(flags, C);
    int flag_V = get_bit(flags, V);

    switch(get_bits(ins, 31, 28)) {
        case 0:
            if (flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 1:
            if (!flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 2:
            if (flag_C) {
                conditionPassed = PASSED;
            }
            break;

        case 3:
            if (!flag_C) {
                conditionPassed = PASSED;
            }
            break;

        case 4:
            if (flag_N) {
                conditionPassed = PASSED;
            }
            break;

        case 5:
            if (!flag_N) {
                conditionPassed = PASSED;
            }
            break;

        case 6:
            if (flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 7:
            if (!flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 8:
            if (flag_C && !flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 9:
            if (!flag_C || flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 10:
            if (flag_N == flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 11:
            if (flag_N != flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 12:
            if (!flag_Z && flag_N == flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 13:
            if (flag_Z || (flag_N != flag_V)) {
                conditionPassed = PASSED;
            }
            break;

        default:
            conditionPassed = PASSED;
            break;
    }

    if (conditionPassed == PASSED) {
        int32_t address = get_bits(ins, 23, 0);
        int32_t pc = arm_read_register(p, 15);

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
            arm_write_register(p, 14, pc);
        }

        if (negatif)
        {
            arm_write_register(p, 15, pc - address);
        }
        else
        {
            arm_write_register(p, 15, pc + address);
        }
        
        return 1;
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
    int conditionPassed = UNPASSED;
    uint32_t flags = arm_read_cpsr(p);
    int flag_N = get_bit(flags, N);
    int flag_Z = get_bit(flags, Z);
    int flag_C = get_bit(flags, C);
    int flag_V = get_bit(flags, V);

    switch(get_bits(ins, 31, 28)) {
        case 0:
            if (flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 1:
            if (!flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 2:
            if (flag_C) {
                conditionPassed = PASSED;
            }
            break;

        case 3:
            if (!flag_C) {
                conditionPassed = PASSED;
            }
            break;

        case 4:
            if (flag_N) {
                conditionPassed = PASSED;
            }
            break;

        case 5:
            if (!flag_N) {
                conditionPassed = PASSED;
            }
            break;

        case 6:
            if (flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 7:
            if (!flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 8:
            if (flag_C && !flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 9:
            if (!flag_C || flag_Z) {
                conditionPassed = PASSED;
            }
            break;

        case 10:
            if (flag_N == flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 11:
            if (flag_N != flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 12:
            if (!flag_Z && flag_N == flag_V) {
                conditionPassed = PASSED;
            }
            break;

        case 13:
            if (flag_Z || (flag_N != flag_V)) {
                conditionPassed = PASSED;
            }
            break;

        default:
            conditionPassed = PASSED;
            break;
    }

    if (conditionPassed == PASSED) {
        int reg = get_bits(ins, 15, 12);
        if (reg == 15)
        {
            return 0;
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
            return 1;
        }
    }

    return 0;
}
