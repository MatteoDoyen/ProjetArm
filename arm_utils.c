#include "arm_utils.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"
#include "debug.h"


void arm_update_flags(arm_core p, uint8_t n_flag, uint8_t z_flag, uint8_t c_flag, uint8_t v_flag) {
    uint32_t flags = arm_read_cpsr(p);
    flags &= ~(0xF << 28);
    flags |= ((n_flag << 3 | z_flag << 2 | c_flag << 1 | v_flag) << 28);

    // doesn't work (idk why!)
    //set_bits(flags, 31, 28, (n_flag << 3 | z_flag << 2 | c_flag << 1 | v_flag));

    arm_write_cpsr(p, flags);
}

int arm_check_condition(arm_core p, uint8_t condition_field) {

	int condition_state = UNPASSED;

    uint32_t flags = arm_read_cpsr(p);

	uint8_t flag_N = get_bit(flags, N);
    uint8_t flag_Z = get_bit(flags, Z);
    uint8_t flag_C = get_bit(flags, C);
    uint8_t flag_V = get_bit(flags, V);

    switch(condition_field) {
        case 0:
            if (flag_Z) {
                condition_state = PASSED;
            }
            break;

        case 1:
            if (!flag_Z) {
                condition_state = PASSED;
            }
            break;

        case 2:
            if (flag_C) {
                condition_state = PASSED;
            }
            break;

        case 3:
            if (!flag_C) {
                condition_state = PASSED;
            }
            break;

        case 4:
            if (flag_N) {
                condition_state = PASSED;
            }
            break;

        case 5:
            if (!flag_N) {
                condition_state = PASSED;
            }
            break;

        case 6:
            if (flag_V) {
                condition_state = PASSED;
            }
            break;

        case 7:
            if (!flag_V) {
                condition_state = PASSED;
            }
            break;

        case 8:
            if (flag_C && !flag_Z) {
                condition_state = PASSED;
            }
            break;

        case 9:
            if (!flag_C || flag_Z) {
                condition_state = PASSED;
            }
            break;

        case 10:
            if (flag_N == flag_V) {
                condition_state = PASSED;
            }
            break;

        case 11:
            if (flag_N != flag_V) {
                condition_state = PASSED;
            }
            break;

        case 12:
            if (!flag_Z && flag_N == flag_V) {
                condition_state = PASSED;
            }
            break;

        case 13:
            if (flag_Z || (flag_N != flag_V)) {
                condition_state = PASSED;
            }
            break;

        default:
            condition_state = PASSED;
            break;
    }

    return condition_state;
}

uint32_t arm_decode_shift(arm_core p, uint8_t shift_type, uint8_t shift_amount, uint32_t second_operand) {

	switch (shift_type)
	{
		case LSL:
			second_operand <<= shift_amount;
			break;
		case LSR:
			second_operand >>= shift_amount;
			break;
		case ASR:
			second_operand = asr(second_operand, shift_amount);
			break;
		case ROR:
			second_operand = ror(second_operand, shift_amount);
			break;
		default:
			return second_operand;
			break;
	}

	return -1;
}