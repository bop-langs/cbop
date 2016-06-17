#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

/* The program makes an array of randomly initialized integers and adds them together. */

#include "bop_api.h"

static void
initialize(double *array, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        array[i] = sin(i)*sin(i) + cos(i)*cos(i);
    }
}

static double
get_sum(double *array, size_t len)
{
    double sum = 0;
    for (size_t i = 0; i < len; ++i) {
        sum += array[i];
    }
    return sum;
}

int
main(int argc, char ** argv)
{
    int data_size, num_blocks;
    int block_size;

    /* processing the input */
    if (argc>3 || argc<2) {
        printf("Usage: %s array_size-in-millions num-blocks\n", argv[0]);
        exit(1);
    }
    data_size = (int) (atof(argv[1])*1000000);
    assert(data_size>0);
    num_blocks = atoi(argv[2]);
    assert(num_blocks>0);

    printf("%d: adding %d million numbers\n", getpid(), data_size/1000000);
    block_size = ceil((float)data_size / num_blocks);


    double sums[num_blocks];
    double *blocks[num_blocks];
    memset(&sums, 0, num_blocks * sizeof(double));
    for (size_t i = 0; i < num_blocks; ++i) {
        blocks[i] = malloc(block_size * sizeof(double));
        assert(blocks[i] != NULL);
    }

    int index = 0;
    while (data_size > 0) {
        int block_end = data_size;
        data_size -= block_size;
        int block_begin = data_size >= 0 ? data_size : 0 ;
        double *block = blocks[index];
        assert(block != NULL);

        BOP_ppr_begin(1);    /* Begin PPR */

        initialize(block, block_end - block_begin);
        BOP_promise(block, (block_end - block_begin) * sizeof(*block));

        sums[index] = get_sum(block, block_end - block_begin);
        BOP_promise(&sums[index], sizeof(double));

        BOP_ppr_end(1);    /* End PPR */
        index++;
    }

    double sum = get_sum(sums, num_blocks);

    printf("%d: The sum is %.0f million (%.0f) \n", getpid(), sum/1000000, sum);
    return 0;
}
