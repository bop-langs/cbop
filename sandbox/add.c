#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>

#include "bop_api.h"
#include "dmmalloc.h"

/* The program makes an array of randomly initialized integers and adds them together. */

int
main(int argc, char **argv)
{
    int blocksize, parallelism, datasize;
    double sum, *data;

    /* Processing the input */
    if (argc != 3) {
        printf("Usage: %s array_size-in-millions num-blocks\n", argv[0]);
        exit(1);
    }

    datasize = (int)(atof(argv[1])*1000000);
    assert(datasize>0);
    parallelism = atoi(argv[2]);
    assert(parallelism>0);

    /* Initialization */
    printf("%d: initializing %d million numbers\n", getpid(), datasize/1000000);

    data = (double *)malloc(datasize * sizeof(*data));
    for (int i = 0; i < datasize; i++) {
        data[i] = i;
    }

    printf("%d: adding %d million numbers\n", getpid(), datasize/1000000);

    blocksize = ceil((float)datasize/parallelism);

    for (int i = 0; i < datasize; i += blocksize) {
        int n;
        double sump = 0.0;

        BOP_ppr_begin(1);

        BOP_record_read(&blocksize, sizeof(blocksize));
        BOP_record_read(&datasize, sizeof(datasize));
        n = i + blocksize > datasize? datasize : i + blocksize;

        BOP_record_read(data + i, (n - i) * sizeof(*data));
        for (int j = i; j<n; j++) {
            sump += sin(data[j])*sin(data[j]) + cos(data[j])*cos(data[j]);
        }

        BOP_ordered_begin(1);
        BOP_record_read(&sum, sizeof(sum));
        BOP_record_write(&sum, sizeof(sum));
        sum += sump;
        BOP_ordered_end(1);

        BOP_ppr_end(1);
    }

    printf("%d: %d million numbers added. The sum is %.0f million (%.0f) \n", getpid(), datasize/1000000, sum/1000000, sum);

    free(data);

    dm_print_info();

    return 0;
}
