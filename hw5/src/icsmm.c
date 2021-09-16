/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 * If you want to make helper functions, put them in helpers.c
 */
#include "icsmm.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PAGE_SIZE 4096
#define HEADER_UNUSED 0xAAAAAAAA
#define FOOTER_UNUSED 0xFFFFFFFFFFFF
#define MINIMUM_BLOCK_SIZE 32
#define MAX_SIZE 16352

ics_free_header *freelist_head = NULL;
ics_free_header *freelist_next = NULL;
int first_call = 1;
ics_footer* prologue = NULL; 
ics_footer* epilogue = NULL;


void *ics_malloc(size_t size) {
	if(size > MAX_SIZE){
		errno = ENOMEM;
		return NULL;
	}
	if(size == 0){
		errno = EINVAL;
		return NULL;
	}
	if(freelist_head == NULL && first_call){
		first_call = 0;
		prologue = (ics_footer*)ics_inc_brk();
		prologue->block_size = 0x0001;
		epilogue = (ics_footer*)((uint8_t*)ics_get_brk() - 8);
		epilogue->block_size = 0x0001;
		
		freelist_head = (ics_free_header*)((uint8_t*)prologue+8);
		freelist_head->header.block_size = PAGE_SIZE-16;
		freelist_head->header.unused = HEADER_UNUSED;
		freelist_head->next = NULL;
		freelist_head->prev = NULL;
		
		uint16_t bs = freelist_head->header.block_size;
		ics_footer* footer = (ics_footer*)((uint8_t*)freelist_head + bs - 8);
		footer->block_size = bs;
		footer->unused = FOOTER_UNUSED;	
		footer = NULL;	

		freelist_next = freelist_head;
	}else if(freelist_head == NULL && !first_call){
		if((*(int*)(ics_inc_brk())) == -1){
			errno = ENOMEM;
			return NULL;
		}else{
			ics_free_header* new_block = (ics_free_header*)epilogue;
			epilogue = (ics_footer*)((uint8_t*)ics_get_brk() - 8);
			epilogue->block_size = 0x0001;
			new_block->header.block_size = PAGE_SIZE;
			new_block->header.unused = HEADER_UNUSED;
			new_block->next = NULL;
			new_block->prev = NULL;
				
			ics_footer* footer = epilogue - 1;
			footer->block_size = PAGE_SIZE;
			footer->unused = FOOTER_UNUSED;
			footer = NULL;
			
			freelist_head = new_block;
			freelist_next = freelist_head;
		}
	}
	ics_free_header* start_point = freelist_next;
	do{
		uint16_t bs = freelist_next->header.block_size;
		if(size <= bs - 16){
			uint16_t actual_size = roundUp(size, 16) + 16;
			if(bs - actual_size < MINIMUM_BLOCK_SIZE){
				actual_size = bs;
			}
		
			freelist_next->header.block_size = actual_size+1;
			freelist_next->header.requested_size = size;
			
			ics_footer* footer = (ics_footer*)((uint8_t*)freelist_next + actual_size - 8);
			footer->block_size = actual_size+1;
			footer->unused = FOOTER_UNUSED;
			footer = NULL; 			

			if(actual_size < bs){
				ics_free_header* new_block = (ics_free_header*)((uint8_t*)freelist_next + actual_size);
				new_block->header.block_size = bs-actual_size;
				new_block->header.unused = HEADER_UNUSED;
				
				footer = (ics_footer*)((uint8_t*)freelist_next + bs - 8);
				footer->block_size = bs-actual_size;
				footer->unused = FOOTER_UNUSED;
				footer = NULL;			

				new_block->next = freelist_head;
				new_block->prev = NULL;
				freelist_head->prev = new_block;
				freelist_head = new_block;
			}
			void* return_ptr = (void*)((uint8_t*)freelist_next + 8);
			if(freelist_next->prev == NULL){
				if(freelist_next->next == NULL){
					freelist_head = NULL;
					freelist_next = NULL;	
				}else{
					freelist_head = freelist_next->next;
					freelist_head->prev = NULL;
					freelist_next = freelist_head;
				}
			}else{
				freelist_next->prev->next = freelist_next->next;
				if(freelist_next->next == NULL){
					freelist_next = freelist_head;
				}else{
					freelist_next = freelist_next->next;
				}
			}
			return return_ptr;
		}else{
			if(freelist_next->next == NULL){
				freelist_next = freelist_head;
			}else{
				freelist_next = freelist_next->next;
			}
		}
	}while(freelist_next != start_point);
	
	if((*(int*)ics_inc_brk()) == -1){
		errno = ENOMEM;
		return NULL;
	}else{
		ics_free_header* new_block;
		ics_footer* footer = epilogue - 1;
		epilogue = (ics_footer*)((uint8_t*)ics_get_brk() - 8);
		epilogue->block_size = 0x0001;

		uint16_t bs = footer->block_size;
		if((bs & 1) == 1){
			new_block = (ics_free_header*)(footer+1);
			new_block->header.block_size = PAGE_SIZE;
		}else{
			new_block = (ics_free_header*)((uint8_t*)footer - bs + 8);
			new_block->header.block_size = bs + PAGE_SIZE;
		}
		new_block->header.unused = HEADER_UNUSED;
				
		footer = (ics_footer*)(epilogue - 1);
		footer->block_size = new_block->header.block_size;
		footer->unused = FOOTER_UNUSED;
		footer = NULL;			

		if((bs & 1) == 1){
			new_block->next = freelist_head;
			new_block->prev = NULL;
			freelist_head->prev = new_block;
			freelist_head = new_block;
		}
		freelist_next = new_block;
		return ics_malloc(size);
	}
}

void *ics_realloc(void *ptr, size_t size) {
	if(ptr <= (void*)prologue || ptr >= (void*)epilogue){
		errno = EINVAL;
		return NULL;
	}
	
	ics_header* header = (ics_header*)((uint8_t*)ptr - 8);
	ics_footer* footer = (ics_footer*)((uint8_t*)header + header->block_size-1 - 8);
	
	if(header->unused != HEADER_UNUSED){
		errno = EINVAL;
		return NULL;
	}
	if(footer->unused != FOOTER_UNUSED){
		errno = EINVAL;
		return NULL;
	}
	if(header->block_size != footer->block_size){
		errno = EINVAL;
		return NULL;
	}
	if(header->requested_size >= header->block_size){
		errno = EINVAL;
		return NULL;
	}
	if((header->block_size & 1) != 1){
		errno = EINVAL;
		return NULL;
	}
	if((footer->block_size & 1) != 1){
		errno = EINVAL;
		return NULL;
	}
	
	if(size == 0){
		ics_free(ptr);
		return NULL;
	}

	ics_header* realloc_block = (ics_header*)ics_malloc(size);
	if(realloc_block == NULL){
		errno = ENOMEM;
		return NULL;
	};
	ics_header* old_header = (ics_header*)ptr - 1;
	size_t old_size = old_header->block_size-1;
	size_t copy_size = size;
	if(old_size < copy_size){
		copy_size = old_size;
	}
	copy_payload(realloc_block, ptr, copy_size);
	return realloc_block;
}

int ics_free(void *ptr) {
	if(ptr <= (void*)prologue || ptr >= (void*)epilogue){
		errno = EINVAL;
		return -1;
	}
	
	ics_header* header = (ics_header*)((uint8_t*)ptr - 8);
	ics_footer* footer = (ics_footer*)((uint8_t*)header + header->block_size-1 - 8);
	
	if(header->unused != HEADER_UNUSED){
		errno = EINVAL;
		return -1;
	}
	if(footer->unused != FOOTER_UNUSED){
		errno = EINVAL;
		return -1;
	}
	if(header->block_size != footer->block_size){
		errno = EINVAL;
		return -1;
	}
	if(header->requested_size >= header->block_size){
		errno = EINVAL;
		return -1;
	}
	if((header->block_size & 1) != 1){
		errno = EINVAL;
		return -1;
	}
	if((footer->block_size & 1) != 1){
		errno = EINVAL;
		return -1;
	}

	ics_header* new_header = header;
	ics_footer* new_footer = footer;
	uint16_t new_block_size = header->block_size - 1;	

	ics_footer* left_footer = (ics_footer*)(header-1);
	ics_header* right_header = (ics_header*)(footer+1);
	if((left_footer->block_size & 1) == 0){
		new_header = (ics_header*)((uint8_t*)header - left_footer->block_size);
		new_block_size = new_block_size + left_footer->block_size;
	}
	if((right_header->block_size & 1) == 0){
		new_footer = (ics_footer*)((uint8_t*)footer + right_header->block_size);
		new_block_size = new_block_size + right_header->block_size;
	}
	new_header->block_size = new_block_size;
	new_header->unused = HEADER_UNUSED;
	new_footer->block_size = new_block_size;
	new_footer->unused = FOOTER_UNUSED;
	
	ics_free_header* new_block = (ics_free_header*)new_header;
	if(freelist_head == NULL && freelist_next == NULL){
		new_block->next = NULL;
		freelist_next = new_block;
	}else if((void*)freelist_head >= (void*)new_header && (void*)freelist_head <= (void*)new_footer){
		new_block->next = freelist_head->next;
	}else{
		new_block->next = freelist_head;
	}
	new_block->prev = NULL;
	if(new_block->next !=NULL){
		new_block->next->prev = new_block;
	}
	freelist_head = new_block;
	
	if((void*)freelist_next >= (void*)new_header && (void*)freelist_next <= (void*)new_footer){
		freelist_next = freelist_head;
	}
	
	return 0;
}
