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

int arm_load_store(arm_core p, uint32_t ins) {

    int success = 1;
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
                case LSL:
                    offset = offset << shift_imm;
                    break;
                case LSR:
                    if (shift_imm == 0) {
                        offset = 0;
                    }
                    else {
                        offset = offset >> shift_imm;
                    }
                    break;
                case ASR:
                    offset = asr(offset, shift_imm);
                    break;
                case ROR:
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
        if (!bit_W && rn == 15) {
            base_address += 8;
        }
        base_address += offset;
    }

    if (bit_load) { // load
        if (bit_byte) {
            uint8_t byte = 0;
            success = success && !arm_read_byte(p, base_address, &byte);
            arm_write_register(p, rd, byte);
        }
        else {
            success = success && !arm_read_word(p, base_address, &value);
            if (rd == 15) {
                uint32_t cpsr_value = clr_bit(arm_read_cpsr(p), 0) | get_bit(value, 0);
                arm_write_cpsr(p, cpsr_value);
                value &= 0xFFFFFFFE;
            }
            arm_write_register(p, rd, value);
        }
    }
    else { // store
        value = arm_read_register(p, rd);
        if (bit_byte) {
            success = success && !arm_write_byte(p, base_address, (uint8_t)value);
        }
        else {
            success = success && !arm_write_word(p, base_address, value);
        }
    }

    if (bit_P == bit_W) {
        base_address = bit_P ? base_address : base_address + offset;
        arm_write_register(p, rn, base_address);
    }

    return success - 1;
}
// bit S ne peut pas être à 1 pour LDM(1) et STM(1)
int arm_load_store_multiple(arm_core p, uint32_t ins) {

    int success = 1;
    uint8_t bit_load = get_bit(ins, 20); // bit L
    uint8_t bit_write_back = get_bit(ins, 21); // bit W
    uint8_t bit_increment = get_bit(ins, 23); // bit U
    uint8_t bit_before = get_bit(ins, 24); // bit P

    uint8_t rn = get_bits(ins, 19, 16);
    uint16_t register_list = get_bits(ins, 14, 0);
    uint32_t base_address = arm_read_register(p, rn);
    uint32_t write_back_value = base_address;

    uint32_t value = 0;
    int8_t number_bit_set = 0;
    int8_t registers_tab[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    int i = 0;

    for (i = 0; i < 16; i++) {
        if (get_bit(register_list, i)) {
            registers_tab[number_bit_set] = i;
            number_bit_set++;
        }
    }

    int8_t step = bit_increment ? 4 : -4;
    write_back_value += number_bit_set * step;
    base_address = bit_increment ? base_address : base_address - (number_bit_set * 4) + 4;
    base_address = bit_before ? base_address + step : base_address;

    i = 0;
    while (registers_tab[i] != -1) {
        if (bit_load) { // load
            success = success && !arm_read_word(p, base_address + 4 * i, &value);

            if (registers_tab[i] == 15) {
                uint32_t cpsr_value = clr_bit(arm_read_cpsr(p), 0) | get_bit(value, 0);
                arm_write_cpsr(p, cpsr_value);
                value &= 0xFFFFFFFE;
            }
            arm_write_register(p, registers_tab[i], value);
        }
        else { // store
            value = arm_read_register(p, registers_tab[i]);
            success = success && !arm_write_word(p, base_address + 4 * i, value);
        }
        i++;
    }

    if (bit_write_back) {
        arm_write_register(p, rn, write_back_value);
    }

    return success - 1;
}

int arm_extra_load_store(arm_core p, uint32_t ins) {

    uint8_t bit_S = get_bit(ins, 6);
    uint8_t bit_H = get_bit(ins, 5);

    if (bit_S || !bit_H) {
        return -1;
        // pas un halfword
    }

    int success = 1;
    uint8_t bit_P = get_bit(ins, 24);
    uint8_t bit_add = get_bit(ins, 23);
    uint8_t bit_immediate = get_bit(ins, 22);
    uint8_t bit_W = get_bit(ins, 21);
    uint8_t bit_load = get_bit(ins, 20);

    uint8_t rn = get_bits(ins, 19, 16);
    uint8_t rd = get_bits(ins, 15, 12);
    uint8_t addr_mode_H = get_bits(ins, 11, 8);
    uint8_t addr_mode_L = get_bits(ins, 3, 0);

    int32_t offset = 0;
    uint32_t value = 0;
    uint32_t base_address = arm_read_register(p, rn);

    if (bit_immediate) {
        offset = (addr_mode_H << 4) | addr_mode_L;
    }
    else {
        offset = arm_read_register(p, addr_mode_L);
    }

    offset = bit_add ? offset : -offset;

    if (bit_P) {
        if (!bit_W && rn == 15) {
            base_address += 8;
        }
        base_address += offset;
    }

    if (bit_load) { // load
        uint16_t temp = 0;
        success = success && !arm_read_half(p, base_address, &temp);
        value |= temp;
        arm_write_register(p, rd, value);
    }
    else { // store
        value |= arm_read_register(p, rd);
        success = success && !arm_write_word(p, base_address, value);
    }

    if (bit_P == bit_W) { // write-back
        if (!bit_P)
            base_address += offset;
        arm_write_register(p, rn, base_address);
    }

    return success - 1;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
