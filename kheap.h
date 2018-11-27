#ifndef __KHEAP_H__
#define __KHEAP_H__

#include <ktype.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct m_block* m_block_ptr;
struct m_block {
    size_t size;    // size of this block
    m_block_ptr prev;   // pointer to previous block
    m_block_ptr next;   // pointer to next block
    int is_free;  // this block is free or not
    void* m_ptr;  // magic pointer, point to data
    char data[1];   // first byte of this block
};

struct m_block2 {
    size_t size;    // size of this block
    m_block_ptr prev;   // pointer to previous block
    m_block_ptr next;   // pointer to next block
    char is_free;  // this block is free or not
    char padding[3]; // 3 bytes padding for memory alignment
    void* m_ptr;  // magic pointer, point to data
    char data[1];   // first byte of this block
};


k_uint32_t heap_init(void);
void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void free(void* ptr);


#ifdef __cplusplus
}
#endif

#endif
