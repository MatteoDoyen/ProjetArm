#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "memory.h"
#include "arm_load_store.h"
#include "arm_core.h"

int main() {

    memory mem = memory_create(0x10000, 0);
    arm_core p = arm_create(mem);

    uint32_t word;
    uint16_t halfword;
    uint8_t byte;

    printf("START load_store_test.c :\n");

    // LDR R1, [R0]
    assert(arm_write_word(p, 0x500, (uint32_t)456) == 0);
    arm_write_register(p, 0, 0x500);

    arm_load_store(p, 0xe5901000);
    assert(arm_read_register(p, 1) == 456);

    printf("LDR R1, [R0] : PASSED\n");

    // LDR R12, [R13, #-4]!
    assert(arm_write_word(p, 0x600, (uint32_t)8745) == 0);
    arm_write_register(p, 13, 0x604);

    arm_load_store(p, 0xe53dc004);
    assert(arm_read_register(p, 12) == 8745);
    assert(arm_read_register(p, 13) == 0x600);
    
    printf("LDR R12, [R13, #-4]! : PASSED\n");

    // LDRB R3, [R8, #3]
    assert(arm_write_word(p, 0x700, 0x89456212) == 0);
    arm_write_register(p, 8, 0x6fd);
    arm_write_register(p, 3, 0x545a6823);

    arm_load_store(p, 0xe5d83003);
    assert(arm_read_register(p, 3) == 0x00000012);
    
    printf("LDRB R3, [R8, #3] : PASSED\n");

    // LDRH R12, [R13, #-6]
    assert(arm_write_word(p, 0x800, 0x89456212) == 0);
    arm_write_register(p, 13, 0x806);

    arm_extra_load_store(p, 0xe15dc0b6);
    assert(arm_read_register(p, 12) == 0x00006212);
    
    printf("LDRH R12, [R13, #-6] : PASSED\n");

    // STR R2, [R1, #0x100]
    arm_write_register(p, 2, 0x56565656);
    arm_write_register(p, 1, 0x900);

    arm_load_store(p, 0xe5812100);
    arm_read_word(p, 0x900 + 0x100, &word);
    assert(word == 0x56565656);
    
    printf("STR R2, [R1, #0x100] : PASSED\n");

    // STRB R4, [R10, #0x200]
    arm_write_register(p, 4, 0x894232ab);
    arm_write_register(p, 10, 0x900);

    arm_load_store(p, 0xe5ca4200);
    arm_read_byte(p, 0x900 + 0x200, &byte);
    assert(byte == 0xab);
    
    printf("STRB R4, [R10, #0x200] : PASSED\n");

    // STRB R7, [R6, #-4]!
    arm_write_register(p, 7, 0x894232ab);
    arm_write_register(p, 6, 0x900);

    arm_load_store(p, 0xe5667004);
    arm_read_byte(p, 0x900 - 4, &byte);
    assert(byte == 0xab);
    assert(arm_read_register(p, 6) == 0x900 - 4);
    
    printf("STRB R7, [R6, #-4]! : PASSED\n");

    // STRH R2, [R5], #8
    arm_write_register(p, 2, 0x894232ab);
    arm_write_register(p, 5, 0x1000);

    arm_extra_load_store(p, 0xe0c520b8);
    arm_read_half(p, 0x1000, &halfword);
    assert(halfword == (uint16_t)0x894232ab);
    assert(arm_read_register(p, 5) == 0x1000 + 8);
    
    printf("STRH R2, [R5], #8 : PASSED\n");

    // ldmia r4, {r0 - r2, r6, r9}
    uint32_t base_address = 0x2000;
    arm_write_register(p, 4, 0x2000);

    for (int i = 0; i < 5; i++) {
        arm_write_word(p, base_address + 4 * i, 56 * (i + 1));
    }

    arm_load_store_multiple(p, 0xe8940247);

    assert(arm_read_register(p, 0) == 56);
    assert(arm_read_register(p, 1) == 56 * 2);
    assert(arm_read_register(p, 2) == 56 * 3);
    assert(arm_read_register(p, 6) == 56 * 4);
    assert(arm_read_register(p, 9) == 56 * 5);
    
    printf("LDMIA R4, {R0 - R2, R6, R9} : PASSED\n");
    
    // ldmdb r4!, {r0 - r1, r7}
    base_address = 0x2000;
    arm_write_register(p, 4, 0x2000);

    for (int i = 0; i < 3; i++) {
        arm_write_word(p, (base_address - 12) + 4 * i, 56 * (i + 1));
    }

    arm_load_store_multiple(p, 0xe9340083);

    assert(arm_read_register(p, 0) == 56);
    assert(arm_read_register(p, 1) == 56 * 2);
    assert(arm_read_register(p, 7) == 56 * 3);
    assert(arm_read_register(p, 4) == base_address - 12);
    
    printf("LDMDB R4!, {R0 - R1, R7} : PASSED\n");

    // stmdb r4!, {r0 - r1, r7}
    base_address = 0x2000;
    arm_write_register(p, 4, 0x2000);

    arm_write_register(p, 0, 89);
    arm_write_register(p, 1, 89 * 2);
    arm_write_register(p, 7, 89 * 3);

    arm_load_store_multiple(p, 0xe9240083);

    for (int i = 0; i < 3; i++) {
        arm_read_word(p, (base_address - 12) + 4 * i, &word);
        assert(word == 89 * (i + 1));
    }
    assert(arm_read_register(p, 4) == base_address - 12);
    
    printf("STMDB R4!, {R0 - R1, R7} : PASSED\n");

    // STR R8, [R5, -R4]!
    arm_write_register(p, 8, 0x56565656);
    arm_write_register(p, 5, 0x900);
    arm_write_register(p, 4, 0x10);

    arm_load_store(p, 0xe7258004);
    arm_read_word(p, 0x900 - 0x10, &word);
    assert(word == 0x56565656);
    assert(arm_read_register(p, 5) == 0x900 - 0x10);
    
    printf("STR R8, [R5, -R4]! : PASSED\n");

    // LDR R11, [R3, R5, LSL #2]
    assert(arm_write_word(p, 0x600, (uint32_t)8745) == 0);
    arm_write_register(p, 5, 0x10);
    arm_write_register(p, 3, 0x600 - (0x10 << 2));

    arm_load_store(p, 0xe793b105);
    assert(arm_read_register(p, 11) == 8745);
    
    printf("LDR R11, [R3, R5, LSL #2] : PASSED\n");

    // STRH R8, [R5], -R4
    arm_write_register(p, 8, 0x894232ab);
    arm_write_register(p, 5, 0x1000);
    arm_write_register(p, 4, 0x15);

    arm_extra_load_store(p, 0xe00580b4);
    arm_read_half(p, 0x1000, &halfword);
    assert(halfword == (uint16_t)0x894232ab);
    assert(arm_read_register(p, 5) == 0x1000 - 0x15);
    
    printf("STRH R8, [R5], -R4 : PASSED\n");

    printf("END load_store_test.c\n");

    return EXIT_SUCCESS;
}