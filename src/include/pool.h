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

/**
 * @brief Create a pool object
 * 
 * @return pool_t* a heap instance of the pool object.
 */
pool_t *create_pool(void);

/**
 * @brief Deallocate and destroy the pool object.
 * 
 * @param *pool instance. 
 */
void destroy_pool(pool_t *pool);

/**
 * @brief Fill the entire pool with the `value`.
 * Avoid use this function since it's not thread-safe.
 * Prefer using `pool_fill_area` instead.
 * 
 * @param *pool instance 
 * @param value value to be filled
 */
void pool_fill(pool_t *pool, uint32_t value);

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
void pool_fill_area(pool_t *pool, uint32_t value, uint32_t index, uint32_t offset);

/**
 * @brief Insert `value` in the pool using its own cursor.
 * This function is thread-safe, since the cursor always point
 * to the main block.
 * 
 * @param *pool instance of pool.
 * @param value uint32_t value to be inserted.
 */
void pool_insert(pool_t *pool, uint32_t value);

/**
 * @brief Insert `value` in the pool at `index` position.
 * This function is thread-safe since the index is translated
 * to the main block.
 * 
 * @param *pool instance. 
 * @param value uint32_t value to be inserted.
 * @param index position on the pool.
 */
void pool_insert_at(pool_t *pool, uint32_t value, uint32_t index);

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
int pool_switch_block(pool_t *pool);

/**
 * @brief Set the pool cursor to `new_cursor`.
 * 
 * @param *pool instance. 
 * @param new_cursor new cursor position.
 */
void pool_set_cursor(pool_t *pool, uint32_t new_cursor);

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
uint32_t pool_get(pool_t *pool, uint32_t index);

/**
 * @brief Swtiches the main block of pool.
 * This function is thread-safe. If the current
 * block is using by some thread the operation is
 * aborted and the function returns -1. This function
 * returns 0 when it can successfully switch the block.
 * 
 * @param *pool instance. 
 */
int pool_switch_block_s(pool_t *pool);


#endif /* POOL_H */