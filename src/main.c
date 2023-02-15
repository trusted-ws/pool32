#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "include/hexdump.h"
#include "include/pool.h"

struct thread_args {
    pool_t *pool;
    uint32_t block;
    uint32_t value;
};

void *thread_routine(void *args)
{
    /* 1. Parse args to get the pool instance.          */
    struct thread_args *arguments = (struct thread_args*)args;
    pool_t *pool = (pool_t*)arguments->pool;
    uint32_t block = arguments->block;
    uint32_t value = arguments->value;

    /* 2. Atomically set pool->locked to true.          */
    atomic_exchange_explicit(&pool->locked, true, memory_order_seq_cst);

    /* 3. Perform operation on the specific pool block. */
    for (uint32_t i = block; i < block + POOL_BLOCK_SIZE; i++) {
        pool->pool[i] = 0xaaaaaaaa ^ value;
    }

    /* 4. Atomically set pool->locked to false.         */
    atomic_exchange_explicit(&pool->locked, false, memory_order_seq_cst);

    /* 5. Finish the thread.                            */
    return NULL;
}


int main(void)
{
    pool_t *pool = create_pool();
    pthread_t thread;

    pool_fill_area(pool, 0xffffffff, POOL_BLOCK_A, POOL_BLOCK_B);

    hexdump("Pool (before)", &pool->pool, POOL_SIZE * sizeof(uint32_t), 16);

    for (uint32_t i = 0; i < 500; i++)
    {
        if (pool->iterations >= POOL_BLOCK_SIZE) {
            if (atomic_load_explicit(&pool->locked, memory_order_seq_cst) == false)
            {
                struct thread_args args = {.pool = pool, .block = pool->current_block, .value = i};
                if (pthread_create(&thread, NULL, &thread_routine, &args)) {
                    perror("Cannot create thread");
                    exit(2);
                }
                pthread_detach(thread);
            }
            pool->iterations = 0;
        }
        pool_get(pool, i);
    }

    hexdump("Pool (after)", &pool->pool, POOL_SIZE * sizeof(uint32_t), 16);

    destroy_pool(pool);
    return 0;
}