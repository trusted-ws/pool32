/* * Pool32 - Multi-threaded pools
 * Copyright (C) 2023, 2023 Murilo Augusto <murilo@bad1337.com>
 *
 * This file is part of Pool32.
 *
 * Pool32 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pool32 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pool32.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "include/pool.h"

/**
 * @brief Create a pool object
 * 
 * @return pool_t* a heap instance of the pool object.
 */
pool_t *create_pool(void)
{
    pool_t *p = malloc(sizeof(pool_t));
    
    if (p == NULL) {
        perror("Cannot allocate memory!");
        exit(1);
    }

    for (uint32_t i = 0; i < POOL_SIZE; i++) {
        p->pool[i] = 0;
    }

    p->current_block = POOL_BLOCK_A;
    p->cursor = 0;
    p->iterations = 0;

    pthread_mutex_init(&p->lock, NULL);
    p->locked = ATOMIC_VAR_INIT(false);

    return p;
}

/**
 * @brief Deallocate and destroy the pool object.
 * 
 * @param *pool instance. 
 */
void destroy_pool(pool_t *pool) {
    if (pool != NULL) {
        pthread_mutex_destroy(&pool->lock);
        free(pool);
        pool = NULL;
    }
}

/**
 * @brief Fill the entire pool with the `value`.
 * Avoid use this function since it's not thread-safe.
 * Prefer using `pool_fill_area` instead.
 * 
 * @param *pool instance 
 * @param value value to be filled
 */
void pool_fill(pool_t *pool, uint32_t value)
{
    for (uint32_t i = 0; i < POOL_SIZE; i++) {
        pool->pool[i] = value;
    }
}

/**
 * @brief Fill an specific area of the pool with `value`.
 * Use this with care, since it may not be thread-safe
 * depending of the `index` and the `offset`.
 * 
 * @param *pool instance 
 * @param value value to fill
 * @param index initial index position
 * @param offset length of position
 */
void pool_fill_area(pool_t *pool, uint32_t value, uint32_t index, uint32_t offset)
{
    for (uint32_t i = index; i < index + offset; i++) {
        pool->pool[i] = value;
    }
}

/**
 * @brief Insert `value` in the pool using its own cursor.
 * This function is thread-safe, since the cursor always point
 * to the main block.
 * 
 * @param *pool instance of pool.
 * @param value uint32_t value to be inserted.
 */
void pool_insert(pool_t *pool, uint32_t value)
{
    pool->pool[pool->cursor + pool->current_block] = value;
    pool->cursor = (pool->cursor + 1) % POOL_BLOCK_SIZE;
}

/**
 * @brief Insert `value` in the pool at `index` position.
 * This function is thread-safe since the index is translated
 * to the main block.
 * 
 * @param *pool instance. 
 * @param value uint32_t value to be inserted.
 * @param index position on the pool.
 */
void pool_insert_at(pool_t *pool, uint32_t value, uint32_t index)
{
    pool->pool[(index % POOL_BLOCK_SIZE) + pool->current_block] = value;
}

/**
 * @brief Retrieve the value at `index` position.
 * This function is thread-safe since the index is translated
 * to a valid main block position. It is recommended to use
 * this function instead of accessing the pool object directly
 * since it may lead to inconsistent values.
 * 
 * @param *pool instance. 
 * @param index position.
 * @return uint32_t retrieves the value at index.
 */
uint32_t pool_get(pool_t *pool, uint32_t index)
{
    pool->iterations++;

    if (pool->iterations >= POOL_MAX_IT) {
        pool_switch_block_s(pool);
    }

    return pool->pool[(index % POOL_BLOCK_SIZE) + pool->current_block];
}

/**
 * @brief Switches the main block of pool.
 * This function is not thread-safe. It is not
 * possible to guarantee that some thread is performing
 * any operation based on the main block.
 * 
 * It is highly recommended to use pool_switch_block_s
 * instead.
 * 
 * This function always succeed.
 * 
 * @param *pool instance. 
 */
int pool_switch_block(pool_t *pool)
{
    if (pool->current_block == POOL_BLOCK_A) {
        pool->current_block = POOL_BLOCK_B;
    } else {
        pool->current_block = POOL_BLOCK_A;
    }
    return 0;
}

/**
 * @brief Swtiches the main block of pool.
 * This function is thread-safe. If the current
 * block is using by some thread the operation is
 * aborted and the function returns -1. This function
 * returns 0 when it can successfully switch the block.
 * 
 * @param *pool instance. 
 */
int pool_switch_block_s(pool_t *pool)
{
    if (atomic_load_explicit(&pool->locked, memory_order_seq_cst) == true) {
        /* When the locked flag is set, we will abort the switch. */
        return -1;
    }

    if (pool->current_block == POOL_BLOCK_A) {
        pool->current_block = POOL_BLOCK_B;
    } else {
        pool->current_block = POOL_BLOCK_A;
    }
    return 0;
}

/**
 * @brief Set the pool cursor to `new_cursor`.
 * 
 * @param *pool instance. 
 * @param new_cursor new cursor position.
 */
void pool_set_cursor(pool_t *pool, uint32_t new_cursor)
{
    pool->cursor = new_cursor % POOL_BLOCK_SIZE;
}