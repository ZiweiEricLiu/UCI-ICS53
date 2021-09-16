/**
 * === DO NOT MODIFY THIS FILE ===
 * If you need some other prototypes or constants in a header, please put them in
 * helpers.h
 *
 * When we grade, we will be replacing this file with our own copy.
 * You have been warned.
 * === DO NOT MODIFY THIS FILE ===
 */
#ifndef ICSMM_H
#define ICSMM_H
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define REQUEST_SIZE_BITS 16
#define UNUSED_SIZE_BITS 32
#define BLOCK_SIZE_BITS 16
#define UNUSED_FOOT_SIZE_BITS 48


typedef struct __attribute__((__packed__)) {
    uint64_t     block_size : BLOCK_SIZE_BITS;
    uint64_t         unused : UNUSED_SIZE_BITS;
    uint64_t requested_size : REQUEST_SIZE_BITS;
} ics_header;

typedef struct __attribute__((__packed__)) ics_free_header {
  ics_header header;
  struct ics_free_header *next;
  struct ics_free_header *prev;
} ics_free_header;

typedef struct __attribute__((__packed__)) ics_footer {
    uint64_t     block_size : BLOCK_SIZE_BITS;
    uint64_t         unused : UNUSED_FOOT_SIZE_BITS;
} ics_footer;

extern ics_free_header *freelist_head;
extern ics_free_header *freelist_next;

void *ics_malloc(size_t size);
void *ics_realloc(void *ptr, size_t size);
int ics_free(void *ptr);
void ics_mem_init();
void ics_mem_fini();
void *ics_get_brk();
void *ics_inc_brk();
void ics_freelist_print();
int ics_header_print(void *header);
int ics_payload_print(void *payload);

#endif
