#include <stdio.h>
#include <stdlib.h>
#include "registers.h"
#include "arm_constants.h"

void print_test(int result) {
    if (result)
        printf("Test succeded\n");
    else
        printf("TEST FAILED !!\n");
}

int compare(uint32_t a, uint32_t b) {
    return a == b;
}

int compare_with_sim(uint32_t a, registers r, uint8_t select) {
	uint32_t value = read_register(r, select);
        if (a != value)
            return 0;
    return 1;
}

int main(int argc, char const *argv[])
{
	int register_written_value = 1;
	int register_written_usr_value = 2;

	uint32_t written_value = 23;
	uint32_t written_usr_value = 32;
	uint32_t read_value = 0;
	uint32_t written_spsr = 75;

	registers r = registers_create();
	printf("Test registers_create : Creation de registre : ");
	print_test(r != NULL);

	if (r == NULL) {
		fprintf(stderr, "Erreur a la creation de registres\n");
		exit(1);
	}

	// Changement du mode en superviseur
	printf("Test write_cpsr : Ecriture du mode superviseur : ");
	write_cpsr(r, SVC);
	print_test(compare_with_sim(SVC, r, 16));

	printf("Test get_mode : Superviseur : ");
	print_test(get_mode(r) == SVC);

	printf("Test write_register : Ecriture de %d dans R%d : ", written_value, register_written_value);
	write_register(r, register_written_value, written_value);
	print_test(compare_with_sim(written_value, r, register_written_value));

	printf("Test write_usr_register : Ecriture de %d dans R%d : ", written_usr_value, register_written_usr_value);
	write_usr_register(r, register_written_usr_value, written_usr_value);
	print_test(compare_with_sim(written_usr_value, r, register_written_usr_value));

	printf("Test read_register : Lecture de R%d : ", register_written_value);
	read_value = read_register(r, register_written_value);
	print_test(compare(read_value, written_value));

	printf("Test read_usr_register : Lecture de R%d : ", register_written_usr_value);
	read_value = read_usr_register(r, register_written_usr_value);
	print_test(compare(read_value, written_usr_value));

	printf("Test read_cpsr : Lecture de R16 : ");
	read_value = read_cpsr(r);
	print_test(compare(read_value, SVC));

	printf("Test write_cpsr : Ecriture de %d dans R17 : ", written_spsr);
	write_spsr(r, written_spsr);
	print_test(compare_with_sim(written_spsr, r, 17));

	printf("Test read_spsr : Lecture de R17 : ");
	read_value = read_spsr(r);
	print_test(compare(read_value, written_spsr));

	printf("Test current_mode_has_spsr : ");
	read_value = current_mode_has_spsr(r);
	if (get_mode(r) != USR && get_mode(r) != SYS) {
		print_test(compare(read_value, 1));
	} else {
		print_test(compare(read_value, 0));
	}

	printf("Test in_a_privileged_mode : ");
	read_value = in_a_privileged_mode(r);
	if (get_mode(r) != USR) {
		print_test(compare(read_value, 1));
	} else {
		print_test(compare(read_value, 0));
	}

	registers_destroy(r);
	return 0;
}