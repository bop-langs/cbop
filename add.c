#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>

#include "bop_api.h"

/* The program makes an array of randomly initialized integers and adds them together. */

static double *data;
static int datasize;

static double sum;

int main(int argc, char ** argv)
{
  int _blocksize;
  int _parallelism;

  /* processing the input */
  if (argc>3 || argc<2) {
    printf("Usage: %s array_size-in-millions num-blocks\n", argv[0]);
    exit(1);
  }
  datasize = (int) (atof(argv[1])*1000000);
  assert(datasize>0);
  _parallelism = atoi(argv[2]);
  assert(_parallelism>0);

  /* Initialization */
  printf("%d: initializing %d million numbers\n", getpid(), datasize/1000000);

  data = (double *) malloc(datasize*sizeof(double));
  for (int i = 0; i < datasize; i++)
    data[i] = i;

  printf("%d: adding %d million numbers\n", getpid(), datasize/1000000);

  _blocksize = ceil((float)datasize/_parallelism);


  for (int i=0; i<datasize; i+=_blocksize) {
    BOP_ppr_begin(1);
    BOP_record_read(&_blocksize, sizeof(_blocksize));
    BOP_record_read(&datasize, sizeof(datasize));
    int _n = i+_blocksize > datasize? datasize:i+_blocksize;
    double sump = 0.0;
    BOP_record_read(data + i, (_n - i) * sizeof(*data));
    for (int _j=i; _j<_n; _j++) {
      sump += sin(data[_j])*sin(data[_j])+cos(data[_j])*cos(data[_j]);
      
    }
    BOP_ordered_begin(2);
    BOP_record_read(&sum, sizeof(sum));
    BOP_record_write(&sum, sizeof(sum));
    sum += sump;
    BOP_ordered_end(2);
    BOP_ppr_end(1);
  }

  printf("%d: %d million numbers added.  The sum is %.0f million (%.0f) \n", getpid(), datasize/1000000, sum/1000000, sum);

  return 0;
}
