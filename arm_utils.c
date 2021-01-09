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

uint32_t arm_decode_shift(arm_core p, uint8_t shift_type, uint8_t shift_amount, uint32_t second_operand, uint8_t *shift_carry) {

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

static uint8_t get_shift_value(uint8_t shift_amount, uint8_t shift_type, uint32_t operand, uint8_t *shift_carry) {
    switch (shift_type)
    {
        case LSL:
            if (shift_amount == 32) 
            {
                *shift_carry = get_bit(operand, 0);
                operand = 0;
            } 
            else if (shift_amount < 32) 
            {
                *shift_carry = get_bit(operand, 32-shift_amount);
                operand <<= shift_amount;
            } 
            else 
            {
                *shift_carry = 0;
                operand = 0;
            }
            break;
        case LSR:
            if (shift_amount == 32) 
            {
                *shift_carry = get_bit(operand, 31);
                operand = 0;
            } 
            else if (shift_amount < 32) 
            {
                *shift_carry = get_bit(operand, shift_amount-1);
                 operand >>= shift_amount;
            }
            else
            {
                *shift_carry = 0;
                operand = 0;
            }
            break;
        case ASR:
            if (shift_amount >= 32)
            {
                *shift_carry = get_bit(operand, 31);
                operand = *shift_carry ? 0xFFFFFFFF : 0x0;
            } 
            else 
            {
                *shift_carry = get_bit(operand, shift_amount-1);
                operand = asr(operand, shift_amount);
            }
            break;
        case ROR:
            if (shift_amount == 32) 
            {
                *shift_carry = get_bit(operand, 31);
            } 
            else if (shift_amount > 32) 
            {
                *shift_carry = get_bit(operand, shift_amount-32);
            }
            break;
    }
}   

static uint32_t shift_t(arm_core p, uint8_t shift_type, uint32_t operand, uint8_t shift_amount, uint8_t *shift_carry) {
    if (shift_amount == 0)
    {
        uint32_t flags = arm_read_cpsr(p);
        switch (shift_type) {
            case ROR:
                *shift_carry = get_bit(operand, 0);
                operand = (get_bit(flags, C) << N) | (operand >> 1);
                break;
            default:
                *shift_carry = get_bit(flags, C);
                break;
        }
    }
    else
    {
        
    }

    return operand;
}