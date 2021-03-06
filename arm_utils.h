#ifndef _ARM_UTILS_H_
#define _ARM_UTILS_H_

#include <stdint.h>
#include "arm_core.h"

#define PASSED 1
#define UNPASSED 0

void arm_update_flags(arm_core p, uint8_t n_flag, uint8_t z_flag, uint8_t c_flag, uint8_t v_flag);

int arm_check_condition(arm_core p, uint8_t condition_field);

uint32_t arm_decode_shift(arm_core p, uint8_t shift_type, uint32_t operand, uint8_t shift_amount, uint8_t *shift_carry, uint8_t immediate_shift);

#endif
