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
#include "registers.h"
#include <assert.h>
#include "arm_constants.h"
#include <stdlib.h>
#include "debug.h"
#include <stdio.h>

struct registers_data {
  uint32_t reg[37];
};

int8_t matriceReg[32][18] = {{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},
                            {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,-1}, // USR == 16
                            {0,1,2,3,4,5,6,7,29,30,31,32,33,34,35,15,16,36}, // FIQ ==17
                            {0,1,2,3,4,5,6,7,8,9,10,11,12,26,27,15,16,28}, // IRQ == 18
                            {0,1,2,3,4,5,6,7,8,9,10,11,12,17,18,15,16,19}, // SVC == 19
                            {},{},{},
                            {0,1,2,3,4,5,6,7,8,9,10,11,12,20,21,15,16,22}, // ABT = 23
                            {},{},{},{},
                            {0,1,2,3,4,5,6,7,8,9,10,11,12,23,24,15,16,25},// UND == 28
                            {},{},
                            {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,-1}}; // SYSTEM == 31

registers registers_create() {
    registers r = NULL;
    r = malloc(sizeof(struct registers_data));
    debug("create\n");
    return r;
}

void registers_destroy(registers r) {
  free(r);
}

uint8_t get_mode(registers r) {
    debug("get mode \n");
    return (read_cpsr(r) & 0x1F);
}

int current_mode_has_spsr(registers r) {
    uint8_t mode = get_mode(r);
    debug("current mode has spsr \n");
    return (mode!= USR && mode!=SYS);
}

int in_a_privileged_mode(registers r) {
    debug("in_a_privileged_mode \n");
    return get_mode(r)!=USR;
}

uint32_t read_register(registers r, uint8_t reg) {
    assert(reg<18);
    debug("read_register \n");
    int8_t num_rg = matriceReg[get_mode(r)][reg];
    assert(num_rg>=0);
    return r->reg[num_rg];
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    assert(reg<17);
    debug("read_usr_register \n");
    return r->reg[reg];
}

uint32_t read_cpsr(registers r) {
    uint32_t value=r->reg[16];
    debug("cpsr read : %x\n", value);
    return value;
}

uint32_t read_spsr(registers r) {
    assert(current_mode_has_spsr(r));
     debug("read_spsr \n");
    int8_t num_rg = matriceReg[get_mode(r)][17];
    assert(num_rg>=0);
    return r->reg[num_rg];
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    assert(reg<18);
    debug("write_register \n");
    debug("reg : %d\n", reg);
    debug("mode : %d\n", get_mode(r));
    if(get_mode(r)== USR)
    {
      write_usr_register(r,reg,value);
    }
    int8_t num_rg = matriceReg[get_mode(r)][reg];
    assert(num_rg>=0);
    r->reg[num_rg] = value;
    debug("num_rg : %d, value : %x\n", num_rg, value);
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
  assert(reg<17);
  r->reg[reg] = value;
  debug("write_usr_register \n");
}

void write_cpsr(registers r, uint32_t value) {
  debug("cpsr : %x\n",value);
  r->reg[16] = value;
}

void write_spsr(registers r, uint32_t value) {
  assert(current_mode_has_spsr(r));
   debug("write_spsr \n");
  write_register(r,17,value);
}
