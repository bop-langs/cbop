//NOTE: do not compile with optimizations on. The hack to get calloc working (used in glibc/dlsym) is relies on undefined behavior, but has been tested to work correctly on redhat, fedora and ubuntu using GCC, and ubuntu with CLANG as well
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "external/malloc.h"
#include "dmmalloc.h"
#include "malloc_wrapper.h"
#include "bop_api.h"

#ifndef NDEBUG
#define VISUALIZE(s)
#else

#endif

#define SEQUENTIAL (BOP_task_status() == SEQ || BOP_task_status() == UNDY)
#define SPEC_VISUALIZE(s) if(!SEQUENTIAL) bop_msg(5, s)


#define TABLESIZE 100000
#define CHARSIZE 100

//Macros for the custom allocator
#define u_malloc dm_malloc
#define u_realloc dm_realloc
#define u_free dm_free
#define u_calloc dm_calloc
#define u_malloc_usable_size dm_malloc_usable_size



//prototypes for the dlsym using calloc workaround
void* tempcalloc(size_t, size_t);
static inline void calloc_init();


static void *(*libc_malloc)(size_t) = NULL;
static void *(*libc_realloc)(void*, size_t) = NULL;
static void (*libc_free)(void*) = NULL;
static void *(*libc_calloc)(size_t, size_t) = NULL;
static size_t (*libc_malloc_usable_size)(void*) = NULL;
static int (*libc_posix_memalign)(void**, size_t, size_t) = NULL;
static void *(*calloc_func)(size_t, size_t) = tempcalloc; //part of dlsym workaround

static char calloc_hack[CHARSIZE];
static short initializing = 0;

//Warning: Unsupported functions are currently ignored unless UNSUPPORTED_MALLOC is defined at compile time
#ifdef UNSUPPORTED_MALLOC
//unsupported malloc operations are aborted immediately
void* memalign(size_t size, size_t boundary) {
    printf("\nUNSUPPORTED OPERATION memalign\n");
    _exit(0);
}
int posix_memalign (void **memptr, size_t alignment, size_t size) {
    printf("\nUNSUPPORTED OPERATION posix_memalign\n");
    _exit(0);
}
void* aligned_malloc(size_t size, size_t boundary) {
    printf("\nUNSUPPORTED OPERATION: aligned_malloc\n");
    _exit(0);
}
void* valloc(size_t size) {
    printf("\nUNSUPPORTED OPERATION: valloc\n");
    _exit(0);
}
struct mallinfo mallinfo() {
    printf("\nUNSUPPORTED OPERATION: mallinfo\n");
    _exit(0);
}
#else
#warning "not overriding unsupported"
#endif

void* malloc(size_t s) {


    void* p = u_malloc(s);
#ifdef PTR_CHECK
    mallocs[mc] = p;
    mc++;
    dm_check(p);
#endif
    return p;
}
void* realloc(void *p , size_t s) {
    assert (p != calloc_hack);
    void* p2 = u_realloc(p, s);
    return p2;
}
void free(void * p) {
    if(p == NULL || p == calloc_hack) return;
    u_free(p);
}

size_t malloc_usable_size(void* ptr) {
    assert(ptr != calloc_hack);
    size_t size = u_malloc_usable_size(ptr);
    return size;
}


void * calloc(size_t sz, size_t n) {
    calloc_init();
    //make sure the right calloc_func is being used
    assert(calloc_func != NULL);
    assert( (initializing && calloc_func == tempcalloc) ||
            (!initializing && calloc_func == dm_calloc) );
    void* p = calloc_func(sz, n);
    if(calloc_func == u_calloc) {
      dm_check(p);
    }
    return p;
}

static inline void calloc_init() {
    if(libc_calloc == NULL && !initializing) {
        //first allocation
        initializing = 1; //don't recurse
        calloc_func = tempcalloc;
        libc_calloc = dlsym(RTLD_NEXT, "calloc");
        initializing = 0;
        assert(libc_calloc != NULL);
        calloc_func = u_calloc;
    }
}
void* tempcalloc(size_t s, size_t n) {
    if(s * n > CHARSIZE) {
        _exit(0);
        return NULL;
    }
    int i;
    for(i = 0; i < CHARSIZE; i++)
        calloc_hack[i] = '\0';
    return calloc_hack;
}

inline void * sys_malloc(size_t s) {
    if(libc_malloc == NULL)
        libc_malloc = dlsym(RTLD_NEXT, "malloc");
    assert(libc_malloc != NULL);
    return libc_malloc(s);
}
inline void * sys_realloc(void * p , size_t size) {
    assert(p != calloc_hack);
    if(libc_realloc == NULL)
        libc_realloc = dlsym(RTLD_NEXT, "realloc");
    assert(libc_realloc != NULL);
    void* p2 = libc_realloc(p, size);
    return p2;
}
inline void sys_free(void * p) {
    if(p == calloc_hack)
        return;
    if(libc_free == NULL)
        libc_free = dlsym(RTLD_NEXT, "free");
    assert(libc_free != NULL);
    libc_free(p);
}
inline size_t sys_malloc_usable_size(void* p) {
    if(libc_malloc_usable_size == NULL)
        libc_malloc_usable_size = dlsym(RTLD_NEXT, "malloc_usable_size");
    assert (libc_malloc_usable_size != NULL);
    return libc_malloc_usable_size(p);
}
inline int sys_posix_memalign(void** p, size_t a, size_t s) {
    if(libc_posix_memalign == NULL)
        libc_posix_memalign = dlsym(RTLD_NEXT, "posix_memalign");
    assert(libc_posix_memalign != NULL);
    return libc_posix_memalign(p, a, s);
}

inline void * sys_calloc(size_t s, size_t n) {
    calloc_init();
    assert(libc_calloc != NULL);
    void* p = libc_calloc(s, n);
    return p;
}
static inline char* check_pointer(void* raw_pointer) {
    if(raw_pointer == NULL)
      return "null, always valid\n";
#ifdef PTR_CHECK
    long long m = 0LL;
    long long c =  0LL;
    long long r = 0LL;
    bool found = false;
    for(m = 0LL; !found && m < mc; m++) {
        if(mallocs[m] == raw_pointer)
            return "mallocd";
    }
    for(c = 0LL; !found && c < cc; c++) {
        if(callocs[c] == raw_pointer)
            return "callocd";
    }
    for(r = 0LL; !found && r < rc; r++) {
        return "reallocd";
    }
    printf("Freed unallocated block: %p", raw_pointer);
    _exit(0);
    return NULL;
#else
    return "not tracking pointers";
#endif
}
#undef CHARSIZE
