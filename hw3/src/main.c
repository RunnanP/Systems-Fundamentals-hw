#include <stdio.h>
#include "sfmm.h"
#define UNUSED(x) (void)x


int main(int argc, char const *argv[]) {

    sf_mem_init();
  // sf_blockprint( sf_malloc(4096-16)-8);
 //  sf_blockprint( sf_malloc(4096*2-17)-8);
//sf_varprint(sf_malloc(2));
   void *a=sf_malloc(64);
   sf_malloc(1);

sf_varprint(a);

void *b=sf_realloc(a,16);
sf_blockprint(seg_free_list[0].head);
sf_snapshot();
//sf_varprint(b);
   void *c=sf_malloc(64);
  void*k= sf_malloc(1);



void *d=sf_realloc(c,16);
sf_snapshot();
sf_varprint(k);
UNUSED(c);
UNUSED(d);
UNUSED(b);
UNUSED(k);
    sf_mem_fini();

    return EXIT_SUCCESS;
}
