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


    if (bit_register_offset) { // register shift
        if (get_bits(ins, 11, 4)) { // scaled register offset
            int32_t shift = arm_read_register(p, get_bits(ins, 3, 0)); // valeur du shift
            uint8_t shift_imm = get_bits(ins, 11, 7);

            switch(get_bits(ins, 6, 5)) {
                case 0:
                    shift = shift << shift_imm;
                    break;
                case 1:
                    if (shift_imm == 0) {
                        shift = 0;
                    }
                    else {
                        shift = shift >> shift_imm;
                    }
                    break;
                case 2:
                    shift = asr(shift, shift_imm);
                    break;
                case 3:
                    shift = ror(shift, shift_imm);
                    break;
            }

            shift = bit_add ? shift : -shift;

            uint8_t rn = get_bits(ins, 19, 16);
            uint8_t rd = get_bits(ins, 15, 12);

            uint32_t base_address = arm_read_register(p, rn);

            if (bit_load) { // load
                if (bit_P) { // bit P
                    if (bit_byte) { // bit B
                        // LDRB
                        uint8_t value;
                        arm_read_byte(p, base_address + shift, &value);
                        arm_write_register(p, rd, value);
                    }
                    else {
                        // LDR
                        uint32_t value;
                        arm_read_word(p, base_address + shift, &value);
                        arm_write_register(p, rd, value);
                    }

                    if (bit_W) {
                        arm_write_byte(p, rn, base_address + shift);
                    }
                }
                else {
                    if (!bit_W) { // bit W
                        if (bit_byte) { // bit B
                            // LDRB
                            uint8_t value;
                            arm_read_byte(p, base_address, &value);
                            arm_write_register(p, rd, value);
                        }
                        else {
                            // LDR
                            uint32_t value;
                            arm_read_word(p, base_address, &value);
                            arm_write_register(p, rd, value);
                        }
                        arm_write_byte(p, rn, base_address + shift);
                    }
                }
            }
            else { // store

                if (bit_P) { // bit P
                    if (bit_byte) { // bit B
                        // STRB
                        uint8_t value;
                        value = (uint8_t)arm_read_register(p, rd);
                        arm_write_byte(p, base_address + shift, value);
                    }
                    else {
                        // STR
                        uint32_t value;
                        value = arm_read_register(p, rd);
                        arm_write_word(p, base_address + shift, value);
                    }

                    if (bit_W) {
                        arm_write_register(p, rn, base_address + shift);
                    }
                }
                else {
                    if (!bit_W) { // bit W
                        if (bit_byte) { // bit B
                            // STRB
                            uint8_t value;
                            value = (uint8_t)arm_read_register(p, rd);
                            arm_write_byte(p, base_address + shift, value);
                        }
                        else {
                            // STR
                            uint32_t value;
                            value = arm_read_register(p, rd);
                            arm_write_word(p, base_address + shift, value);
                        }
                        arm_write_register(p, rn, base_address + shift);
                    }
                }
            }
        }
        else { // register offset

            int32_t shift = arm_read_register(p, get_bits(ins, 3, 0)); // valeur du shift
            shift = bit_add ? shift : -shift;
            
            uint8_t rn = get_bits(ins, 19, 16);
            uint8_t rd = get_bits(ins, 15, 12);

            uint32_t base_address = arm_read_register(p, rn);

            if (bit_load) { // load

                if (bit_P) { // bit P
                    if (bit_byte) { // bit B
                        // LDRB
                        uint8_t value;
                        arm_read_byte(p, base_address + shift, &value);
                        arm_write_register(p, rd, value);
                    }
                    else {
                        // LDR
                        uint32_t value;
                        arm_read_word(p, base_address + shift, &value);
                        arm_write_register(p, rd, value);
                    }

                    if (bit_W) {
                        arm_write_byte(p, rn, base_address + shift);
                    }
                }
                else {
                    if (!bit_W) { // bit W
                        if (bit_byte) { // bit B
                            // LDRB
                            uint8_t value;
                            arm_read_byte(p, base_address, &value);
                            arm_write_register(p, rd, value);
                        }
                        else {
                            // LDR
                            uint32_t value;
                            arm_read_word(p, base_address, &value);
                            arm_write_register(p, rd, value);
                        }
                        arm_write_byte(p, rn, base_address + shift);
                    }
                }
            }
            else { // store

                if (bit_P) { // bit P
                    if (bit_byte) { // bit B
                        // LDRB
                        uint8_t value;
                        value = (uint8_t)arm_read_register(p, rd);
                        arm_write_byte(p, base_address + shift, value);
                    }
                    else {
                        // LDR
                        uint32_t value;
                        value = arm_read_register(p, rd);
                        arm_write_word(p, base_address + shift, value);
                    }

                    if (bit_W) {
                        arm_write_register(p, rn, base_address + shift);
                    }
                }
                else {
                    if (!bit_W) { // bit W
                        if (bit_byte) { // bit B
                            // LDRB
                            uint8_t value;
                            value = (uint8_t)arm_read_register(p, rd);
                            arm_write_byte(p, base_address + shift, value);
                        }
                        else {
                            // LDR
                            uint32_t value;
                            value = arm_read_register(p, rd);
                            arm_write_word(p, base_address + shift, value);
                        }
                        arm_write_register(p, rn, base_address + shift);
                    }
                }
            }
        }
    }
    else { // immediate offset

        int16_t shift = get_bits(ins, 11, 0); // valeur du shift
        shift = bit_add ? shift : -shift;
        
        uint8_t rn = get_bits(ins, 19, 16);
        uint8_t rd = get_bits(ins, 15, 12);

        uint32_t base_address = arm_read_register(p, rn);

        if (bit_load) { // load

            if (bit_P) { // bit P
                if (bit_byte) { // bit B
                    // LDRB
                    uint8_t value;
                    arm_read_byte(p, base_address + shift, &value);
                    arm_write_register(p, rd, value);
                }
                else {
                    // LDR
                    uint32_t value;
                    arm_read_word(p, base_address + shift, &value);
                    arm_write_register(p, rd, value);
                }

                if (bit_W) {
                    arm_write_byte(p, rn, base_address + shift);
                }
            }
            else {
                if (!bit_W) { // bit W
                    if (bit_byte) { // bit B
                        // LDRB
                        uint8_t value;
                        arm_read_byte(p, base_address, &value);
                        arm_write_register(p, rd, value);
                    }
                    else {
                        // LDR
                        uint32_t value;
                        arm_read_word(p, base_address, &value);
                        arm_write_register(p, rd, value);
                    }
                    arm_write_byte(p, rn, base_address + shift);
                }
            }
        }
        else { // store

            if (bit_P) { // bit P
                if (bit_byte) { // bit B
                    // LDRB
                    uint8_t value;
                    value = (uint8_t)arm_read_register(p, rd);
                    arm_write_byte(p, base_address + shift, value);
                }
                else {
                    // LDR
                    uint32_t value;
                    value = arm_read_register(p, rd);
                    arm_write_word(p, base_address + shift, value);
                }

                if (bit_W) {
                    arm_write_register(p, rn, base_address + shift);
                }
            }
            else {
                if (!bit_W) { // bit W
                    if (bit_byte) { // bit B
                        // LDRB
                        uint8_t value;
                        value = (uint8_t)arm_read_register(p, rd);
                        arm_write_byte(p, base_address, value);
                    }
                    else {
                        // LDR
                        uint32_t value;
                        value = arm_read_register(p, rd);
                        arm_write_word(p, base_address, value);
                    }
                    arm_write_register(p, rn, base_address + shift);
                }
            }
        }
    }

    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
