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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "arm_utils.h"
#include "util.h"

static int arm_execute_instruction(arm_core p) {
    uint32_t val_inst;

    int result_value = arm_fetch(p,&val_inst);

    // check if result_value has no exceptions
    if (result_value == EXCEPTION)
    {
      return EXCEPTION;
    }

    uint8_t cond_field = get_bits(val_inst, 31, 28);
    uint8_t bits_avant_cond = get_bits(val_inst,27,25);
    uint8_t debut_opcode = get_bits(val_inst,24,23);
    uint8_t bit_s = get_bit(val_inst,20);
    uint8_t bit_quatre = get_bit(val_inst,4);

    int result_condition = arm_check_condition(p, cond_field);

    if (result_condition == UNPASSED)
    {
        return EXCEPTION;
    }

    switch (bits_avant_cond) {
      case 0:
        if(bit_quatre==0)
        {
          //Miscellaneous instructions
          if(debut_opcode==2 && bit_s==0)
          {
            return arm_miscellaneous(p, val_inst);
          }
          // Data processing immediate shift
          else
          {
            return arm_data_processing_shift(p, val_inst);
          }
        }
        // Data processing register shift
        else
        {
          if (get_bit(val_inst, 7)) {
            return arm_extra_load_store(p, val_inst);
          }
          return arm_data_processing_shift(p, val_inst);
        }
        break;
      case 1:
        // UNDEFINED_INSTRUCTION
        if(debut_opcode==2 && bit_s==0)
        {
          return arm_miscellaneous(p, val_inst);
        }
        // data processing immediate
        else
        {
          return arm_data_processing_immediate_msr(p, val_inst);
        }
        break;
      case 2:
        // load store immediate offset
        return arm_load_store(p, val_inst);
        break;
      case 3:
        // Load/Store register offset
        return arm_load_store(p, val_inst);
        break;
      case 4:
        return arm_load_store_multiple(p, val_inst);
        break;
      case 5:
        // Branch and branch with link
        return arm_branch(p, val_inst);
        break;
      case 6:
        return arm_coprocessor_load_store(p, val_inst);
        break;
      case 7:
        // Software interrupt
        return arm_coprocessor_others_swi(p, val_inst);
        break;
      default:
        return UNDEFINED_INSTRUCTION;
        break;
    }

}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    
    if (result)
        arm_exception(p, result);
    return result;
}
