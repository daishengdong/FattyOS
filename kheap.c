#include <kheap.h>

#define BLOCK_SIZE   20

#define ROUND_UP(x, align) (((int) (x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align) ((int)(x) & ~(align - 1))

extern char __end;
#define MAX_HEAP_SIZE		(0x40000000 - ROUND_UP((k_uint32_t)&__end, 32))
#define HEAP_START			ROUND_UP((k_uint32_t)&__end, 32)

void *first_block = NULL;

void *program_break = NULL;

void *sbrk(int s)
{
	if (program_break == (void *)0)
		program_break = (void *)ROUND_UP((k_uint32_t)&__end, 32);

	if (s == 0)
		return program_break;
	else if ((k_uint32_t)program_break - HEAP_START + s < MAX_HEAP_SIZE) {
		return program_break += s;
	} else
		return (void *)-1;
}

int brk(void *addr)
{
	if ((k_uint32_t)addr - HEAP_START < MAX_HEAP_SIZE) {
		program_break = addr;
		return 0;
	} else
		return -1;
}

m_block_ptr find_block_first_fit(m_block_ptr* last, size_t size) {
    m_block_ptr cur_block = first_block;
    while (cur_block && !(cur_block->is_free && cur_block->size >= size)) {
        *last = cur_block;
        cur_block = cur_block->next;
    }
    return cur_block;
}

m_block_ptr extend_heap(m_block_ptr last, size_t size) {
    m_block_ptr cur = sbrk(0);
    if (sbrk(BLOCK_SIZE + size) == (void*)-1)
        return NULL;
    cur->size = size;
    cur->next = NULL;
    if (last) {
        cur->prev = last;
        last->next = cur;
    }
    cur->is_free = 0;
    cur->m_ptr = cur->data;
    return cur;
}

void split_block(m_block_ptr block, size_t size) {
    m_block_ptr new_block;
    new_block = (m_block_ptr)(block->data + size);
    new_block->size = block->size - size - BLOCK_SIZE;
    new_block->prev = block;
    new_block->next = block->next;
    if (block->next)
        block->next->prev = new_block;
    new_block->is_free = 1;
    block->size = size;
    block->next = new_block;
}

size_t align4(size_t size) {
    if (size & 0x3 == 0) return size;
    return ((size >> 2) + 1) << 2;
}

void* malloc(size_t size) {
    m_block_ptr cur, last;
    size_t s = align4(size);
    if (first_block) {
        last = first_block;
        cur = find_block_first_fit(&last, s);
        if (cur) {
            if ((cur->size - s) >= (BLOCK_SIZE + 8))
                split_block(cur, s);
            cur->is_free = 0;
        } else {
            cur = extend_heap(last, s);
            if (!cur) return NULL;
        }
    } else {
        cur = extend_heap(NULL, s);
        if (!cur) return NULL;
        first_block = cur;
    }
    return cur->data;
}

void* calloc(size_t nmemb, size_t size) {
    size_t* new_block;
    size_t s4, i;
    new_block = malloc(nmemb * size);
    if (new_block) {
        s4 = align4(nmemb * size) >> 2;
        for (i = 0; i < s4; ++i)
            new_block[i] = 0;
    }
    return new_block;
}

m_block_ptr get_block(void* ptr) {
    char* tmp = ptr;
    return (ptr = tmp -= BLOCK_SIZE);
}

int valid_addr(void* ptr) {
    if (first_block) {
        if (ptr > first_block && ptr < sbrk(0)) {
            return ptr == (get_block(ptr))->m_ptr;
        }
    }
    return 0;
}

m_block_ptr merge(m_block_ptr block) {
    if (block->next && block->next->is_free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }
    return block;
}

void free(void* ptr) {
    m_block_ptr block;
    if (valid_addr(ptr)) {
        block = get_block(ptr);
        block->is_free = 1;
        if (block->prev && block->prev->is_free)
            block = merge(block->prev);
        if (block->next)
            merge(block);
        else {
            if (block->prev)
                block->prev->next = NULL;
            else 
                first_block = NULL;
            brk(block);
        }
    }
}

void copy_block(m_block_ptr src, m_block_ptr dst) {
    size_t *sdata, *ddata;
    size_t i;
    sdata = src->m_ptr;
    ddata = dst->m_ptr;
    for (i = 0; (i * 4) < src->size && (i * 4) < dst->size; ++i)
        ddata[i] = sdata[i];
}

void* realloc(void* ptr, size_t size) {
    size_t s;
    m_block_ptr block, new_block;
    void* new_ptr;
    if (!ptr) return malloc(size);
    if (valid_addr(ptr)) {
        s = align4(size);
        block = get_block(ptr);
        if (block->size >= s) {
            if (block->size - s >= (BLOCK_SIZE + 8))
                split_block(block, s);
        } else {
            if (block->next && block->next->is_free
                && (block->size + BLOCK_SIZE + block->next->size) >= s) {
                merge(block);
                if (block->size - s >= (BLOCK_SIZE + 8))
                    split_block(block, s);
            } else {
                new_ptr = malloc(s);
                if (!new_ptr) return NULL;
                new_block = get_block(new_ptr);
                copy_block(block, new_block);
                free(ptr);
                return (new_ptr);
            }
        }
        return (ptr);
    }
    return NULL;
}
