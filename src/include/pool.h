#ifndef POOL_H
#define POOL_H
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

#define POOL_SIZE       200
#define POOL_BLOCK_A    0
#define POOL_BLOCK_B    (POOL_SIZE/2)
#define POOL_BLOCK_SIZE POOL_BLOCK_B
#define POOL_MAX_IT     50


typedef struct _pool {
    uint32_t pool[POOL_SIZE];
    uint32_t current_block;
    uint32_t cursor;
    uint32_t iterations;
    pthread_mutex_t lock;
    atomic_bool locked;
} pool_t;

pool_t *create_pool(void);
void destroy_pool(pool_t *pool);
void pool_fill(pool_t *pool, uint32_t value);
void pool_fill_area(pool_t *pool, uint32_t value, uint32_t index, uint32_t offset);
void pool_insert(pool_t *pool, uint32_t value);
void pool_insert_at(pool_t *pool, uint32_t value, uint32_t index);
int pool_switch_block(pool_t *pool);
void pool_set_cursor(pool_t *pool, uint32_t new_cursor);
uint32_t pool_get(pool_t *pool, uint32_t index);
int pool_switch_block_s(pool_t *pool);


#endif /* POOL_H */