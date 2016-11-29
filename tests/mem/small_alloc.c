#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/* The program makes an array of randomly initialized integers and adds them together. */

#include "noomr.h"
#include "bop_api.h"

#ifdef BOP
unsigned int max_ppr = MAX_REQUEST;
#else
unsigned int max_ppr = 1 << 20;
#endif
#define num_arrays 4

int set(int ind){
  return ind;
}

int main(int argc, char ** argv)
{
  unsigned int alloc_size = sizeof(int);
  bop_debug("dm max size is %u", max_ppr);
  bop_debug("Allocation size for test %u", alloc_size);

  volatile int * some_arrays[num_arrays] = {0};
  int * raw;
  int ind = 0;
  for(ind = 0; ind < num_arrays; ind++){
    BOP_ppr_begin(1);
      raw = noomr_malloc(alloc_size); //something larger
      some_arrays[ind] = raw;
      *some_arrays[ind] = set(ind);
      bop_debug("allocation %d at : %p val @ 0: %d", ind, raw, some_arrays[ind][0]);
      BOP_promise(&(some_arrays[ind][0]), sizeof(int));
      BOP_promise(&(some_arrays[ind]), sizeof(some_arrays[ind]));
      sleep(3);
    BOP_ppr_end(1);
  }
  BOP_group_over(1);
  // BOP_use(&some_arrays, sizeof(some_arrays));
  int eval = 0, check, duplicate;
  for(ind = 0; ind < num_arrays; ind++){
    bop_debug("index %d begins at address %p value %d", ind, some_arrays[ind], *some_arrays[ind]);
    duplicate = 0;
    for (check = 0; duplicate == 0 && check < ind; check++) {
      if (some_arrays[ind] == some_arrays[check]) {
        bop_debug("array %d & %d point to the same location!", check, ind);
        duplicate = 1;
        eval = 1;
      }
    }
    if(!duplicate && *some_arrays[ind] != set(ind)){
      bop_debug("array %d has invalid values! mem not copied. expected: %d actual: %d", ind, set(ind), some_arrays[ind][0]);
      eval = 1;
    }
  }
  print_noomr_stats();
  exit(eval);
}
