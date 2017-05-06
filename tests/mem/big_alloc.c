#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

/* The program makes an array of randomly initialized integers and adds them together. */

#include "bop_api.h"
#include "ipa.h"

#define num_arrays 5

int set(int ind){
  return ind + 1;
}

int main(int argc, char ** argv)
{
  unsigned int alloc_size = MAX_REQUEST + 100;
  bop_debug("Allocation size for test %u", alloc_size);

  int * some_arrays[num_arrays] = {0};
  void * raw;
  int ind = 0;
  for(ind = 0; ind < num_arrays; ind++){
    BOP_ppr_begin(1);
      raw = calloc(alloc_size, 1); //something larger
      some_arrays[ind] = raw;
      some_arrays[ind][0] = set(ind);
      bop_debug("allocation %d at : %p val @ 0: %d", ind, raw, some_arrays[ind][0]);
      BOP_promise(&(some_arrays[ind][0]), sizeof(int));
      BOP_promise(&(some_arrays[ind]), sizeof(some_arrays[ind]));
      sleep(3);
    BOP_ppr_end(1);
  }
  BOP_group_over(1);
  // BOP_use(&some_arrays, sizeof(some_arrays));
  int eval = 0;
  for(ind = 0; ind < num_arrays; ind++){
    bop_debug("index %d begins at address %p", ind, &some_arrays[ind][0]);
    if(some_arrays[ind][0] != set(ind)){
      bop_debug("array %d has invalid values! mem not copied. expected: %d actual: %d", ind, ind+1, some_arrays[ind][0]);
      eval = 1;
    }
  }
  exit(eval);
}
