Introduction
This project implements a simple Memory Management Unit (MMU) simulation in C, providing functionalities to map, store, and load values in memory. It includes a basic memory management scheme with paging and swapping to disk when necessary.

Dependencies
Standard C libraries: stdio.h, stdlib.h, sys/types.h, string.h, stdbool.h
Custom header files: mmu.h, input.h

Constants
MEMORY_SIZE: Total size of physical memory (in bytes)
VMEM_SIZE: Total size of virtual memory (in bytes)
PAGE_SIZE: Size of a page (in bytes)
PTE_SIZE: Size of a Page Table Entry (in bytes)
DISK_SIZE: Size of the disk (in bytes)
PFN, VALID, WRITE, PRESENT: Offset indices for different fields in a Page Table Entry. Add to start to access. 
EMPTY, VAL, PT: Constants to represent different page states. VAL being Page with values and PT being Page table.

File Management
The disk is simulated using a file named disk.txt.
A file pointer swap_file is used for disk operations.

Global Variables
memory[]: Array representing physical memory.
disk[]: Array representing disk storage. We made this large at size 1024, becuase the instructions said to assume a large disk. 
hardwareReg[]: Hardware registers to store page table indices for each process.
hardwareRegdisk[]: Hardware registers to store disk page table indices for each process.
memoryPids[]: Array to store PIDs corresponding to physical memory pages.
memoryPages[]: Array to track the status of physical memory pages.
diskPages[]: Array to track the status of disk pages.
freePages[]: Array to track the availability of physical memory pages.
roundrobin: Variable to implement round-robin page eviction policy.

Functions
init_swap_file(): Initializes the disk file.
init_memory(): Initializes physical memory.
Memsim_FirstFreePFN(): Finds the first free physical frame number (PFN).
Memsim_FirstFreePFNdisk(): Finds the first free disk page.
swap(): Swaps a page between memory and disk.
translate_address(): Translates a virtual address to a physical address.
findpt(): Finds the page table index for a process.
updatePTEForSwap(): Updates Page Table Entry (PTE) during page eviction.
findpte(): Finds the index of a Page Table Entry (PTE) for a virtual address.
evict(): Evicts a page from memory based on a round-robin policy.
map(): Maps a virtual address to a physical frame.
store(): Stores a value in memory at a given virtual address.
load(): Loads a value from memory at a given virtual address.

Usage
The main() function provides a simple interface for reading instructions and performing operations based on those instructions.
Instructions are read from standard input and parsed using the Input_GetLine() and Input_NextInstruction() functions from the input.h header.
Instructions should follow the format: PID, INSTRUCTION_TYPE, VIRTUAL_ADDRESS, VALUE.
Supported instruction types are map, store, and load.

Compilation and running
To compile the program, ensure all source files (mmu.c, input.c, etc.) are included and linked appropriately. Then type 'make'

to run: type
'./mmu_simulation'

______________________________________________________________________________________________________________________________________________________________________________
Testing: 
We used the test files from the starter code and ran it against our program. We believe that we achieved the desired output on these runs. I attached the outputs
of these inputs into testSwap.txt and testPart1.txt, so please check there if you want to. 
______________________________________________________________________________________________________________________________________________________________________________
Notes
- // DECIDE TO MAKE PAGE TABLE ENTRY IN ORDER OF PFN, VALID, READ/WRITE,AND PRESENT
//                                                +0  +1      +2             +3
- We took the input part of the starter code and used it in our program. 

-  Please note that when we do run the evict function, we can see that there is output into the disk.txt file, meaning that it found spot and copied the page there. 
