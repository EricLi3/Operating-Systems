#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#include "mmu.h"
#include "input.h"

// DECIDE TO MAKE PAGE TABLE ENTRY IN ORDER OF PFN, VALID, READ/WRITE,AND PRESENT
//                                              +0  +1      +2             +3
#define MEMORY_SIZE 64
#define PAGE_SIZE 16
#define PTE_SIZE 4
#define DISK_SIZE 1024

#define PFN 0
#define VALID 1
#define WRITE 2
#define PRESENT 3

#define EMPTY -1
#define VAL 0
#define PT 1

#define DISK_SWAP_FILE_PATH "./disk.txt"
FILE *swap_file;

// global variables
unsigned char memory[MEMORY_SIZE];
unsigned char disk [DISK_SIZE];

int hardwareReg[4] = {-1, -1, -1, -1};
int hardwareRegdisk[4] = {-1, -1, -1, -1};

int memoryPids[4] = {-1, -1, -1, -1};
int memoryPages[4] = {EMPTY, EMPTY, EMPTY, EMPTY};
int diskPages[64]; /////////////////////////////////////////////MAYBE NEED TO INITIALIZE at bottom

bool freePages[4] = { true, true, true, true }; // Initially all pages are free(true)

int roundrobin = 0;

// Function prototypes
void init_swap_file();
void init_memory();
int Memsim_FirstFreePFN(int pid);
int Memsim_FirstFreePFNdisk();
int translate_address(unsigned char pid, unsigned char virtual_address);
int swap(int mempage, int diskpage);
int findpt(int pid);
int findpte(int pid, int virtual_address);
int evict(int pid);
int updatePTEForSwap(int pid, int frame, int diskpage);
int map(unsigned char pid, unsigned char virtual_address, unsigned char value);
int store(unsigned char pid, unsigned char virtual_address, unsigned char value);
int load(unsigned char pid, unsigned char virtual_address);

// Initalize swap file to be in write mode. Prints error if failure.
void init_swap_file(){
    swap_file = fopen(DISK_SWAP_FILE_PATH, "w+");   // write evicted page to disk file
    if (swap_file == NULL){
        perror("Error opening swap file");
        exit(EXIT_FAILURE);
    }
}
// Inialize memory array to all 0s. 
void init_memory(){
    memset(memory, 0, MEMORY_SIZE);
}

/**
 * @brief Runs the memory manager program
 * 
 * uses the starter codes input.c and input.h files to handle parsing the input. 
*/
int main () {
	init_memory();
    init_swap_file();

    // initalize the diskPages to EMPTY
    for (int i = 0; i < 64; i++){
        diskPages[i] = EMPTY;
    }

	/* Begin reading instructions and completing requested operations. Loops continuously. Returns when finished. */
	char* line;
	while (1) { // continue to read input until EOF
		if (Input_GetLine(&line) < 1) { // allocate memory for each line
			printf("End of File.\n");
			return 0;
		} else {
			Input_NextInstruction(line);
		}
		free(line);
	}
}

/**
 * @brief Finds the index of the first free page frame number (PFN) in memory or invokes eviction if necessary.
 *
 * This function iterates through the memory pages and returns the index of the first
 * free page frame number (PFN). 
 * 
 * If no free page is found, it calls the eviction process
 * to free up memory and returns the index of the evicted page. If eviction fails, it returns -1.
 *
 * @param pid The process ID for which the free page is being searched.
 * @return The index of the first free page frame number (PFN) in memory or the index of the evicted page, or -1 if eviction fails.
 */
int Memsim_FirstFreePFN(int pid) {
    for (int i = 0; i < 4; i++){    // loop through memoryPages array to see if we have a free slot.
        if (memoryPages[i] == EMPTY){
            return i;
        }
    }
    // if all pages are full, evict a page
    return evict(pid);
}

/**
 * @brief Finds the index of the first free page frame number (PFN) on the disk.
 *
 * This function iterates through the disk pages and returns the index of the first
 * free page frame number (PFN). If no free page is found, it returns -1.
 *
 * @return The index of the first free page frame number (PFN) on the disk, or -1 if no free page is found.
 */
int Memsim_FirstFreePFNdisk() {
    for (int i = 0; i < 64; i++){   // loop through diskPages array to see if we have a free slot.
        //printf("%d\n", diskPages[i]); //////////////////////////////NOT storing correctly. 
        if (diskPages[i] == EMPTY){
            return i;
        }
    }
    return -1;
}

/**
 * @brief Translates from virtual address to physical address.
 * 
 * @param pid   proccess id 
 * @param virtual_address   virtual address to translate
*/
int translate_address(unsigned char pid, unsigned char virtual_address){
    int offset = virtual_address % PAGE_SIZE;
    int start = findpte(pid, virtual_address);
    
    // invalid mapping through findpte, Valid bit, and present bit return -1 in error
    if (start == -1 || memory[start + VALID] == 0 || memory[start + PRESENT] == 0){
        return -1;
    }
    // Calculate the physical address through formula given in starter code.
    int paddr = memory[start + PFN] * PAGE_SIZE + offset;   
    return paddr;
}

/**
 * @brief Swaps a page between memory and disk.
 *
 * This function swaps a page between memory and disk. It copies the contents of the
 * page in memory to the corresponding page on disk, and vice versa. It then writes the swapped page to 
 * the disk file.
 *
 * @param mempage The index of the page in memory to be swapped.
 * @param diskpage The index of the page on disk to be swapped.
 *
 * @return 0 on success, -1 on failure.
 */
int swap(int mempage, int diskpage){
    int memstart = mempage * PAGE_SIZE; // calculate the starting index of memory and disk via page num.
    int diskstart = diskpage * PAGE_SIZE;
    unsigned char temp;
    for (int i = 0; i < PAGE_SIZE; i++){
        temp = memory[memstart + i];    // swap through common temp trick in C bit by bit
        memory[memstart + i] = disk[diskstart + i];
        disk[diskstart + i] = temp;
    }
    // Write swapped page to disk file. Observe output in disk.txt file
    fseek(swap_file, diskpage * PAGE_SIZE, SEEK_SET);
    fwrite(&memory[memstart], sizeof(unsigned char), PAGE_SIZE, swap_file);
    return 0;
}

/**
 * @brief Finds the page table index for a given process ID.
 *
 * This function finds the page table index for a given process ID. If the page table is not
 * currently in memory, it swaps it from the disk to memory and returns its index.
 *
 * @param pid The process ID for which to find the page table index.
 *
 * @return The index of the page table, or -1 if the page table does not exist.
 */
int findpt(int pid){
    // if PT not in memory
    if (hardwareReg[pid] == -1){
        // if PT doesn't exist in either disk or mem Registers, return -1 as in error
        if (hardwareRegdisk[pid] == -1){
            return -1;
        }
        // ELSE case: PT is on disk, swap it to the memory and return PT idx
        int mempage = Memsim_FirstFreePFN(pid);
        int diskpage = hardwareRegdisk[pid];
        swap(mempage, diskpage);
        hardwareReg[pid] = mempage;
        memoryPages[mempage] = PT;  // set state as we swapped into memory
        memoryPids[mempage] = pid;
        diskPages[diskpage] = EMPTY;
        printf("Swapped disk page %d to memory page %d\n", diskpage, mempage);
    }
    // return the PT index
    return hardwareReg[pid];
}

/**
 * @brief Updates the page table entry (PTE) when swapping a page between memory and disk.
 *
 * This function updates the page table entry (PTE) for a process when a page is swapped between
 * memory and disk. 
 * 
 * If the page is currently in memory, the function updates the corresponding PTE
 * in the memory array. 
 * 
 * If the page is currently in disk, it updates the corresponding PTE in the
 * disk array.
 *
 * @param pid The process ID associated with the page.
 * @param frame The frame number of the page being swapped.
 * @param diskpage The disk page number to which the page is being swapped.
 *
 * @return 0 if the PTE is successfully updated.
 */
int updatePTEForSwap(int pid, int frame, int diskpage){
    if (hardwareReg[pid] > -1){ // currently in disk/not in memory
        int start = hardwareReg[pid] * PAGE_SIZE;
        for (int i = 0; i < 4; i++){
            if (memory[start + PFN] == frame){
                printf("updating evicted idx...: %d\n", start);
                memory[start + PRESENT] = 0;    // set state
                memory[start + PFN] = diskpage;
                break;
            }
            start += PTE_SIZE;
        }
    } else {  // currently in memory
        int start = hardwareRegdisk[pid] * PAGE_SIZE;
        for (int i = 0; i < 4; i++){
            if (disk[start + PFN] == frame){
                printf("updating evicted idx...: %d\n", start);
                disk[start + PRESENT] = 0;  // set state
                disk[start + PFN] = diskpage;
                break;
            }
            start += PTE_SIZE;
        }
    }
    return 0;
}

/**
 * @brief Find the index of a page table entry (PTE) in memory.
 *
 * This function calculates the index of the page table entry (PTE) corresponding
 * to a given process ID (PID) and virtual address within the memory. It takes into
 * account the page table structure and the size of a page table entry.
 *
 * @param pid             The process ID for which the PTE index is to be found.
 * @param virtual_address The virtual address for which the PTE index is to be found.
 *
 * @return The index of the PTE in memory, or -1 if the page table for the given PID
 *         does not exist or if the virtual address is not mapped.
 */
int findpte(int pid, int virtual_address){
    int pagetable = findpt(pid);
    if (pagetable == -1){
        return -1;
    }
    int virtual_page_number = virtual_address / PAGE_SIZE;
    return pagetable*PAGE_SIZE + virtual_page_number*PTE_SIZE;
}

/**
 * @brief Evict a page from memory using round-robin method.
 *
 * This function evicts a page from memory using the round-robin method,
 * ensuring fair eviction of both data pages and page tables. It returns
 * the index of the evicted page or page table.
 *
 * @param pid The process ID for which eviction is being performed.
 *
 * @return The index of the evicted page or page table, or -1 if attempting
 *         to evict an empty page.
 */
int evict(int pid) {
    roundrobin = roundrobin % 4;
    int evicttype = memoryPages[roundrobin];
    int evictpid = memoryPids[roundrobin];
    int diskpage = Memsim_FirstFreePFNdisk();
    if (evicttype == VAL){  // we have a data page to evict
        // find the PTE to update
        updatePTEForSwap(evictpid, roundrobin, diskpage);
        // evict the page
        swap(roundrobin, diskpage);
        diskPages[diskpage] = VAL;
    } 
    else if (evicttype == PT) {  
        // we have a page table to be evicted
        if (evictpid == pid){ // If the page table belongs to the process we want to allocate a page for it
            roundrobin++;
            evict(pid); // recursively
            return roundrobin - 1;
        }
        else{
            // Update the page owner array to indicate the page table is now on disk
            hardwareReg[evictpid] = -1; // no longer there
            hardwareRegdisk[evictpid] = diskpage;
            // Swap the page table to disk
            swap(roundrobin, diskpage);
            // Mark the disk page as occupied
            diskPages[diskpage] = PT;
        } 
    }
    else {
        printf("Trying to evict an empty page\n");
        return -1;
    }
    printf("Swapped frame %d out to disk at swap slot %d\n", roundrobin, diskpage);
    return roundrobin++;
}

/**
 * @brief Maps a virtual address to a physical frame for a given process.
 *
 * This function maps a virtual address to a physical frame for a given process.
 * It performs bounds checking on the virtual address and ensures that the value
 * for mapping is either 0 or 1. If the virtual address is already mapped, it
 * updates permissions if necessary or returns an error. Otherwise, it maps the
 * virtual page to a physical frame.
 *
 * @param pid The process ID for which mapping is being performed.
 * @param virtual_address The virtual address to be mapped.
 * @param value The value indicating the permission for the mapped page (0 for read-only, 1 for read/write).
 *
 * @return 0 if mapping is successful, -1 if there's an error (invalid virtual address, page already mapped).
 */
int map(unsigned char pid, unsigned char virtual_address, unsigned char value){

    if(virtual_address > 63 || virtual_address < 0){
        printf("Virtual Addresses must be between 0 and 63\n");
        return -1;
    }
    int pagetable = findpt(pid); // find a page table
    int virtual_page_number = virtual_address / PAGE_SIZE;

    if (value != 0 && value != 1){
        printf("Invalid value for map instruction. Value must be 0 or 1.\n");
        return -1;
    }
    // the page table doesn't exist, try to allocate one page for PT
    if (pagetable == -1){
        // get a free page
        pagetable = Memsim_FirstFreePFN(pid);

        // set the hardware register and page status
        memoryPages[pagetable] = PT; // set state
        memoryPids[pagetable] = pid;
        hardwareReg[pid] = pagetable;
        // initialize the page table
        int start = pagetable * PAGE_SIZE;
        int end = start + PAGE_SIZE;
        for (int i = start; i < end; i++){
            memory[i] = 0;
        }
        printf("Put page table for PID %u into physical frame %d\n", pid, pagetable);
        freePages[pagetable] = false;
    }
    int startidx = pagetable*PAGE_SIZE + virtual_page_number*PTE_SIZE;

    // If the virtual page is already mapped, update permissions or return error -1
    if (memory[startidx + VALID] == 1){
        if (value == memory[startidx + WRITE]){
            printf("Error: virtual page %d already mapped with rw_bit = %u\n", virtual_page_number, value);
            return -1;
        } else {
            memory[startidx + WRITE] = value;
            printf("Updating permissions for virtual page %d\n", virtual_page_number);
            return 0;
        }
    }

    int frame_number = Memsim_FirstFreePFN(pid);
    // else,  map a virtual page to physical frame
    memoryPages[frame_number] = VAL;
    memoryPids[frame_number] = pid;

    memory[startidx + PFN] = frame_number;
    memory[startidx + VALID] = 1;
    memory[startidx + WRITE] = value;
    memory[startidx + PRESENT] = 1;
    printf("Mapped virtual address %d (page %d) into physical frame %d\n", virtual_address, virtual_page_number, frame_number);
    return 0;
}
/**
 * @brief Stores a value at a given virtual address for a specified process.
 *
 * This function stores a value at the provided virtual address for the given process.
 * It performs bounds checking on the value to make sure it falls within the range of [0, 255].
 * If the page containing the virtual address is not in memory, it swaps it into memory.
 * If there's no mapping for the virtual address, it returns an error.
 * If the page is not writable, it returns an error.
 *
 * @param pid The process ID for which the value is being stored.
 * @param virtual_address The virtual address where the value is to be stored.
 * @param value The value to be stored, must be an integer in the range [0, 255].
 *
 * @return 0 if the value is successfully stored, -1 if there's an error (e.g., invalid value, no mapping, page not writable).
 */
int store(unsigned char pid, unsigned char virtual_address, unsigned char value){
    if(value > 255 || value < 0){
        printf("Value must be an integer value in the range of [0,255].\n");
        return -1;
    }
    int startidx = findpte(pid, virtual_address);
    // if page on disk, swap it to the memory
    if (memory[startidx + PRESENT] == 0){   // not in memory
        int frame = Memsim_FirstFreePFN(pid);
        int diskframe = memory[startidx + PFN];
        swap(frame,diskframe);

        memoryPages[frame] = VAL;   // update values as neccessary
        memoryPids[frame] = pid;
        diskPages[diskframe] = EMPTY;
        memory[startidx + PRESENT] = 1;
        memory[startidx + PFN] = frame;
    }
    int paddr = translate_address(pid, virtual_address); // get physical address through translation
    if (paddr == -1){
        printf("No mapping for process %u virtual address %u\n", pid, virtual_address);
        return -1;
    }
    if (memory[startidx + WRITE] != 1){
        printf("Error: writes are not allowed to page %u\n", memory[startidx + PFN]);
        return -1;
    }
    // add value to physical address location of memory array
    memory[paddr] = value;
    printf("Stored value %u at virtual address %u (physical address %d)\n", value, virtual_address, paddr);
    return 0;
}
/**
 * @brief Loads the value at a given virtual address for a specified process.
 *
 * This function loads the value from the specified virtual address for the given process.
 * If the page containing the virtual address is not in memory, it swaps it into memory.
 * If there's no mapping for the virtual address, it returns an error.
 * Otherwise, it prints the value at the virtual address along with its physical address.
 *
 * @param pid The process ID for which the value is being loaded.
 * @param virtual_address The virtual address from which the value is to be loaded.
 *
 * @return 0 if the value is successfully loaded, -1 if there's an error (e.g., no mapping).
 */
int load(unsigned char pid, unsigned char virtual_address){
    int start = findpte(pid, virtual_address);
    // if page on disk, swap it to the memory
    if (memory[start + PRESENT] == 0){  // Not in memory
        int frame = Memsim_FirstFreePFN(pid);
        int diskframe = memory[start + PFN];
        swap(frame,diskframe);  // swap into mem.
        
        memoryPages[frame] = VAL;// update values as neccessary
        memoryPids[frame] = pid;
        diskPages[diskframe] = EMPTY;
        memory[start + PRESENT] = 1;
        memory[start + PFN] = frame;
    }
    int paddr = translate_address(pid, virtual_address);    // get physical address through translation
    if (paddr == -1){
        printf("No mapping for process %u virtual address %u\n", pid, virtual_address);
        return -1;
    }
    
    printf("The value %u is virtual address %u (physical address %d)\n", memory[paddr], virtual_address, paddr);
    return 0;
}
