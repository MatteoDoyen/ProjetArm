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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"
// TODO : halfword
int arm_load_store(arm_core p, uint32_t ins) {

    uint8_t bit_register_offset = get_bit(ins, 25); // bit I
    uint8_t bit_load = get_bit(ins, 20); // bit L
    uint8_t bit_byte = get_bit(ins, 22); // bit B
    uint8_t bit_add = get_bit(ins, 23); // bit U
    uint8_t bit_P = get_bit(ins, 24); // bit P
    uint8_t bit_W = get_bit(ins, 21); // bit W

    uint8_t rn = get_bits(ins, 19, 16);
    uint8_t rd = get_bits(ins, 15, 12);
    uint32_t base_address = arm_read_register(p, rn);

    int32_t offset = 0;
    uint32_t value = 0;

    if (bit_register_offset) { // register offset 
        offset = arm_read_register(p, get_bits(ins, 3, 0));
        if (get_bits(ins, 11, 4)) { // scaled register offset
            uint8_t shift_imm = get_bits(ins, 11, 7);

            switch(get_bits(ins, 6, 5)) {
                case 0:
                    offset = offset << shift_imm;
                    break;
                case 1:
                    if (shift_imm == 0) {
                        offset = 0;
                    }
                    else {
                        offset = offset >> shift_imm;
                    }
                    break;
                case 2:
                    offset = asr(offset, shift_imm);
                    break;
                case 3:
                    offset = ror(offset, shift_imm);
                    break;
            }
        }
    }
    else { // immediate
        offset = get_bits(ins, 11, 0);
    }

    offset = bit_add ? offset : -offset;

    if (bit_P) {
        base_address = base_address + offset;
    }

    if (bit_load) { // load
        if (bit_byte) {
            arm_read_byte(p, base_address, &value);
        }
        else {
            arm_read_word(p, base_address, &value);
        }
        
        arm_write_register(p, rd, value);
    }
    else { // store
        value = arm_read_register(p, rd);
        if (bit_byte) {
            arm_write_byte(p, base_address, (uint8_t)value);
        }
        else {
            arm_write_word(p, base_address, value);
        }
    }

    if (bit_P == bit_W) {
        base_address = bit_P ? base_address : base_address + offset; 
        arm_write_register(p, rn, base_address);
    }

    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {

    uint8_t bit_load = get_bit(ins, 20); // bit L
    uint8_t bit_write_back = get_bit(ins, 21); // bit W
    uint8_t bit_S = get_bit(ins, 22); // bit S
    uint8_t bit_increment = get_bit(ins, 23); // bit U
    uint8_t bit_before = get_bit(ins, 24); // bit P

    uint8_t rn = get_bits(ins, 19, 16);
    uint16_t register_list = get_bits(ins, 15, 0);
    uint32_t base_address = arm_read_register(p, rn);

    int8_t step = 4;
    uint32_t value = 0;
    int8_t number_bit_set = 0;

    for (int i = 0; i < 16; i++) {
        if (get_bit(register_list, i))
            number_bit_set++;
    }    

    if (bit_increment) {
        if (bit_before)
            base_address += step;
    }
    else {
        base_address -= (number_bit_set * 4) + 4;
        if (bit_before)
            base_address -= step;
    }

    for (int i = 0; i < 16; i++) {
        if (get_bit(register_list, i)) {
            if (bit_load) { // load
                arm_read_word(p, base_address, &value);
                arm_write_register(p, i, value);
            }
            else { // store
                value = arm_read_register(p, i);
                arm_write_word(p, base_address, value);
            }
            base_address += step;
        }
    }
    
    if (bit_write_back) {
        if (bit_increment) {
            if (bit_before)
                arm_write_register(p, rn, base_address);
            else
                arm_write_register(p, rn, base_address + step);
        }
        else {
            if (bit_before)
                arm_write_register(p, rn, base_address + 4 - (number_bit_set * 4));
            else
                arm_write_register(p, rn, base_address - (number_bit_set * 4));
        }
    }

    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
