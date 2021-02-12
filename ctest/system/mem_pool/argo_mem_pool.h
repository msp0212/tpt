#ifndef __ARGO_MEM_POOL_H__
#define __ARGO_MEM_POOL_H__

#ifdef ARGO_TEST_DEBUG
	#define DEBUG_PRINTF(format, ...) \
		printf("DEBUG:%s:%u:%s() "format"\n", \
				__FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
	#define DEBUG_PRINTF(format, ...) 
#endif

struct argo_mem_pool;

struct argo_mem_pool *argo_mem_pool_create(uint32_t obj_sz, uint32_t obj_cnt);
void argo_mem_pool_destroy(struct argo_mem_pool *mp);
void *argo_mem_pool_get(struct argo_mem_pool *mp);
void argo_mem_pool_put(struct argo_mem_pool *mp, void *addr);

#endif /*__ARGO_MEM_POOL_H__*/
