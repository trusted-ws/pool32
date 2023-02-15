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