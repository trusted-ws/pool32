#include <assert.h>
#include <stdbool.h>
#include "../src/include/pool.h"


void test_create_pool(void)
{
    pool_t *pool = create_pool();

    assert(pool->cursor == 0);
    assert(pool->iterations == 0);
    assert(pool->locked == false);
    assert(pool->current_block == POOL_BLOCK_A);

    destroy_pool(pool);
}

void test_fill(void)
{
    pool_t *pool = create_pool();
    uint32_t i;

    pool_fill(pool, 0xffffffff);
    for (i = 0; i < POOL_SIZE; i++) {
        assert(pool->pool[i] == 0xffffffff);
    }

    pool_fill_area(pool, 0xcccccccc, POOL_BLOCK_B, POOL_BLOCK_SIZE);
    i = POOL_BLOCK_B-1;
    assert(pool->pool[i] == 0xffffffff);
    for (i = POOL_BLOCK_B; i < POOL_BLOCK_SIZE; i++) {
        assert(pool->pool[i] == 0xcccccccc);
    }

    pool_fill(pool, 0xffffffff);
    pool_fill_area(pool, 0xaaaaaaaa, POOL_BLOCK_A + 2, 32);

    assert(pool->pool[POOL_BLOCK_A] == 0xffffffff);
    assert(pool->pool[POOL_BLOCK_A + 1] == 0xffffffff);
    assert(pool->pool[POOL_BLOCK_A + 2] == 0xaaaaaaaa);

    for (i = POOL_BLOCK_A + 2; i < 32; i++) {
        assert(pool->pool[i] == 0xaaaaaaaa);
    }

    assert(pool->pool[POOL_BLOCK_A + 2 + 31] == 0xaaaaaaaa);
    assert(pool->pool[POOL_BLOCK_A + 2 + 32] == 0xffffffff);

    destroy_pool(pool);
}

void test_positioning(void)
{
    pool_t *pool = create_pool();

    pool_set_cursor(pool, 32);
    pool_insert(pool, 0xffffffff);

    pool_insert_at(pool, 0xcccccccc, 5);

    assert(pool_get(pool, 32) == 0xffffffff);
    assert(pool_get(pool, 5) == 0xcccccccc);

    destroy_pool(pool);
}

void test_switch(void)
{
    pool_t *pool = create_pool();

    pool_fill(pool, 0xcccccccc);
    pool_fill_area(pool, 0xffffffff, POOL_BLOCK_A, POOL_BLOCK_SIZE);

    pool_switch_block(pool);

    assert(pool_get(pool, 0) == 0xcccccccc);
    
    pool_switch_block_s(pool);

    assert(pool_get(pool, 0) == 0xffffffff);

    destroy_pool(pool);
}


int main(void)
{
    test_create_pool();
    test_fill();
    test_positioning();
    test_switch();

    return 0;
}