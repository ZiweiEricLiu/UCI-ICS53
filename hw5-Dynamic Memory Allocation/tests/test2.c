#include "debug.h"
#include "icsmm.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>

void press_to_cont() {                                                       
    printf("Press Enter to Continue");                                      
    while (getchar() != '\n')                                               
      ;                                                                     
    printf("\n");                                                           
}

int main(int argc, char *argv[]) {
  // Initialize the custom allocator
  ics_mem_init();

  // Tell the user about the fields
press_to_cont();

  // Print out title for first test
  ics_malloc(48);
ics_freelist_print();

 press_to_cont();
 // Now assign a value
  void* ptr = ics_malloc(32);
*((int*)ptr) = 520503;
ics_payload_print(ptr);
printf("%d\n", *((int*)ptr));

ics_freelist_print();
press_to_cont();


int* check = ics_realloc(ptr, 60);
ics_payload_print(check);
printf("%d\n", *check);

ics_freelist_print();
press_to_cont();

ics_malloc(144);
ics_freelist_print();
press_to_cont();

ics_malloc(32);
ics_freelist_print();
press_to_cont();

ics_malloc(3664);
ics_freelist_print();

press_to_cont();
  ics_free(ptr);
ics_freelist_print();
ics_malloc(16);
  ics_freelist_print();
ics_mem_fini();
return 0;
}
