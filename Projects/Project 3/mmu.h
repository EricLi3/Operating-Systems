#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// Function prototypes
int Memsim_FirstFreePFN(int pid);
int evict(int pid);
int findpte(int pid, int virtual_address);
void init_swap_file();
void init_memory();
int swap(int mempage, int diskpage);
int translate_address(unsigned char pid, unsigned char virtual_address);
int findpt(int pid);
int updatePTEForSwap(int pid, int frame, int diskpage);
int store(unsigned char pid, unsigned char virtual_address, unsigned char value);
int load(unsigned char pid, unsigned char virtual_address);
int map(unsigned char pid, unsigned char virtual_address, unsigned char value);

#endif /* VIRTUAL_MEMORY_H */
