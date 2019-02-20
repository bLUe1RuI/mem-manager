#include <stdlib.h>
#include "mem_manager.h"

int main(int argc, char *argv[])
{
	struct mempool pool;
        pool.pinit = init_mempool;
        pool.pinit(&pool);
        void *start_addr = (void *)malloc(sizeof(int));
        free(start_addr);
        pool.pmem_init(&pool, start_addr, 1000000);
        char *test1 = (char *)pool.pmem_alloc(&pool, 100);
	memcpy(test1, "abcdefg", sizeof("abcdefg"));
	memset(test1, 0, sizeof(test1));
        char *test2 = (char *)pool.pmem_alloc(&pool, 100);
	memcpy(test2, "abcdefg", sizeof("abcdefg"));
	memset(test2, 0, sizeof(test2));
        char *test3 = (char *)pool.pmem_alloc(&pool, 100);
	memcpy(test3, "abcdefg", sizeof("abcdefg"));
	memset(test3, 0, sizeof(test3));

        pool.pmem_free(&pool, test1);
	test1 = 0;
        pool.pmem_free(&pool, test2);
	test2 = 0;
        pool.pmem_free(&pool, test3);
	test3 = 0;

        test1 = (char *)pool.pmem_alloc(&pool, 100);
	memcpy(test1, "abcdefg", sizeof("abcdefg"));
	memset(test1, 0, sizeof(test1));
        test2 = (char *)pool.pmem_alloc(&pool, 100);
	memcpy(test2, "abcdefg", sizeof("abcdefg"));
	memset(test2, 0, sizeof(test2));
        test3 = (char *)pool.pmem_alloc(&pool, 100);
	memcpy(test3, "abcdefg", sizeof("abcdefg"));
	memset(test3, 0, sizeof(test3));

        pool.pmem_destroy(&pool, start_addr);

	return 0;
}
