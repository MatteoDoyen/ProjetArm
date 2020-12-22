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
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "memory.h"
#include "util.h"

struct memory_data {
    uint8_t *tab;
    uint32_t taille;
    int is_big_endian;
};

memory memory_create(size_t size, int is_big_endian) {
    memory mem=NULL;
    mem = malloc(sizeof(struct memory_data));
    if(mem){
        mem->taille = size;
        mem->is_big_endian = is_big_endian;
        mem->tab = malloc(sizeof(uint8_t)*size);
        if(!mem->tab)
        {
            free(mem);
            mem=NULL;
        }
    }
    return mem;
}

size_t memory_get_size(memory mem) {
    return mem->taille;
}

void memory_destroy(memory mem) {
    free(mem->tab);
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
    if(address>memory_get_size(mem)-1)
    {
            fprintf(stderr, "test\n");
            return 0;
    }
    *value = mem->tab[address];

    return 1;
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value) {
    uint8_t *val = (uint8_t *) value;

    if(mem->is_big_endian)
    {
        return memory_read_byte(mem,address,(val+1)) && memory_read_byte(mem,address+1,val);
    }
    return memory_read_byte(mem,address,(val)) && memory_read_byte(mem,address+1,(val+1));
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value) {
    uint16_t *val = (uint16_t *) value;
    if(mem->is_big_endian)
    {
            return memory_read_half(mem,address,(val+1)) && memory_read_half(mem,address+2,(val));
    }
    return memory_read_half(mem,address,val) && memory_read_half(mem,address+2,(val+1));
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    if(address>memory_get_size(mem)-1){
        return 0;
    }
    mem->tab[address] = value;
    return 1;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value) {
    uint8_t *val = (uint8_t *) &value;
    if(mem->is_big_endian)
    {
      return memory_write_byte(mem,address,*(1+val)) && memory_write_byte(mem,address+1,*val);
    }
    return memory_write_byte(mem,address,*val) && memory_write_byte(mem,address+1,*(val+1));
}

int memory_write_word(memory mem, uint32_t address, uint32_t value) {
    uint16_t *val = (uint16_t *) &value;
    if(mem->is_big_endian)
    {
      return memory_write_half(mem,address,*(val+1)) && memory_write_half(mem,address+2,*val);
    }
    return memory_write_half(mem,address,*val) && memory_write_half(mem,address+2,*(val+1));
}
