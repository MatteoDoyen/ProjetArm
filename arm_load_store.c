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

int arm_load_store(arm_core p, uint32_t ins) {

    uint8_t encoding = get_bits(ins, 27, 20);

    if (get_bit(encoding, 5)) {
        //register shift
    }
    else {
        //immediate shift
        uint16_t shift = get_bits(encoding, 11, 0); // valeur du shift
        shift = get_bit(encoding, 3) ? shift : -shift; // test sur bit U
        
        uint8_t rn = get_bits(encoding, 19, 16);
        uint8_t rd = get_bits(encoding, 15, 12);

        if (get_bit(encoding, 0)) {
            //store
        }
        else {
            //load
            if (get_bit(encoding, 4)) { // bit P
                if (get_bit(encoding, 2)) { // bit B
                    // LDRB
                    uint8_t value;
                    arm_read_byte(p, rn + shift, &value);
                    arm_write_register(p, rd, value);
                }
                else {
                    // LDR
                    uint32_t value;
                    arm_read_word(p, rn + shift, &value);
                    arm_write_register(p, rd, value);
                }

                if (get_bit(encoding, 1)) {
                    arm_write_byte(p, rn, rn + shift);
                }
            }
            else {
                if (!get_bit(encoding, 1)) { // bit W
                    if (get_bit(encoding, 2)) { // bit B
                        // LDRB
                        uint8_t value;
                        arm_read_byte(p, rn, &value);
                        arm_write_register(p, rd, value);
                    }
                    else {
                        // LDR
                        uint32_t value;
                        arm_read_word(p, rn, &value);
                        arm_write_register(p, rd, value);
                    }
                    arm_write_byte(p, rn, rn + shift);
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
