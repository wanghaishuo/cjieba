#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BloomFilter BloomFilterT;

// probability为期望的误判率
BloomFilterT *CreateBloomFilter(uint32_t itemNum, double probability);

void BloomFilterInsert(BloomFilterT *filter, ConstBufT key);

bool BloomFilterContain(BloomFilterT *filter, ConstBufT key);

void DestroyBloomFilter(BloomFilterT *filter);

#ifdef __cplusplus
}
#endif

#endif // BLOOM_FILTER_H