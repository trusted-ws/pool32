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

#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>

typedef struct _pool {
    int data[100];
    atomic_bool locked;
} pool_t;

void thread_routine(pool_t *pool)
{
    atomic_exchange_explicit(&pool->locked, true, memory_order_seq_cst);
    pool->data[0] = 2;  // do something with data.
    atomic_exchange_explicit(&pool->locked, false, memory_order_seq_cst);
}

void do_something(pool_t *pool)
{
    if (atomic_load_explicit(&pool->locked, memory_order_seq_cst) == true) {
        return; // abort!
    }

    pool->data[0] = 1; // do something with data.
}

int main(void)
{
    pool_t *pool = create_pool();
    pool->locked = ATOMIC_VAR_INIT(false);

    create_thread(thread_routine, pool);
    do_something(pool);

    destroy_pool(pool);
    return 0;
}