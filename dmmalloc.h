#ifndef DM_MALLOC_H
#define DM_MALLOC_H

#include <stdbool.h>
#include <stddef.h>

// Dm structs, unions etc
typedef struct {
    struct header *next;
#ifdef DM_REM_ALLOC
    bool allocated;
#endif
    union {
        size_t blocksize;
        struct header *prev;
    };
} header;

#ifdef DM_REM_ALLOC
#define SET_ALLOCATED(h, a) (h->allocated = a)
#else
#define SET_ALLOCATED(h, a)
#endif

// Prototypes
void *dm_malloc(size_t);
void *
dm_realloc(void *, size_t);
void
dm_free(void *);
void *dm_calloc(size_t, size_t);
void
dm_print_info(void);
size_t
dm_malloc_usable_size(void *);
void
dm_check(void *);

// Bop-related functions
void
carve(); // Divide up avaliable memory
void
initialize_group(); // Set end pointers for this ppr task

// Data accessors for merge time
void
malloc_merge(void);
void
malloc_merge_counts(bool); // Counts get updated AFTER abort status is known

// Alignment based on word size
#if __WORDSIZE == 64
#define ALIGNMENT 8
#elif __WORDSIZE == 32
#define ALIGNMENT 4
#else
#error "need 32 or 64 bit word size"
#endif

// Malloc config macros
#ifndef DM_BLOCK_SIZE
#define DM_BLOCK_SIZE 750
#endif

// Alignement/ header macros
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define HSIZE (ALIGN((sizeof(header))))
#define HEADER(vp) ((header *)(((char *)(vp)) - HSIZE))
#define CAST_UH(h) ((struct header *)(h))
#define CAST_H(h) ((header *)(h))
#define CHARP(p) (((char *)(p)))
#define PAYLOAD(hp) ((header *)(((char *)(hp)) + HSIZE))
#define PTR_MATH(ptr, d) ((CHARP(ptr)) + d)
#define ASSERTBLK(head) bop_assert((head)->blocksize > 0);

// Class size macros
#define DM_NUM_CLASSES 16
#define DM_CLASS_OFFSET \
    4 // How much extra to shift the bits for size class, ie class k is 2 ^ (k
      // + DM_CLASS_OFFSET)
#define MAX_SIZE SIZE_C(DM_NUM_CLASSES)
#define SIZE_C(k) \
    (ALIGN((1 << (k + DM_CLASS_OFFSET)))) // Allows for iterative spliting
#define DM_MAX_REQ (ALIGN((MAX_SIZE) - (HSIZE)))

#endif
