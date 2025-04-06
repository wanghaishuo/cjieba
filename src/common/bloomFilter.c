#include "bloomFilter.h"
#include "log.h"
#include <math.h>
#include <xxhash.h>

typedef struct BloomFilter {
    uint32_t hashNum;    // hash函数的数量
    uint32_t itemCount;  // 当前元素的数量，用于校验创建时的配置是否正确
    uint32_t itemMaxNum; // 设置时的元素数量
    uint32_t bitSize;    // bit arr 大小 单位为字节
    uint8_t *bitArr;
} BloomFilterT;

// 计算所需的位数组大小（基于预期元素数量和误判率）
static inline uint32_t CalculateBitSize(uint32_t itemNum, double p) {
    return (uint32_t)(-(double)itemNum * log(p) / (log(2) * log(2))) / 8 + 1;
}

// 计算所需的哈希函数数量
static inline uint32_t CalculateHashNum(uint32_t itemNum, uint32_t bitSize) {
    return (uint32_t)((double)bitSize * 8 * log(2) / itemNum);
}

static inline void CheckProbability(uint32_t itemNum, double probability, uint32_t bitSize, uint32_t hashNum) {
#ifndef NDEBUG
    double p1 = 1 - exp(-(double)hashNum / bitSize / 8 * itemNum);
    double p = pow(p1, hashNum);
    JIEBA_ASSERT(p < probability * 2 && p > probability * 0.5); // 简单验证
#endif
}

BloomFilterT *CreateBloomFilter(uint32_t itemNum, double probability) {
    BloomFilterT *filter = malloc(sizeof(BloomFilterT));
    if (filter == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CreateBloomFilter| malloc filter wrong");
        return NULL;
    }
    *filter = (BloomFilterT){};
    filter->bitSize = CalculateBitSize(itemNum, probability);
    filter->hashNum = CalculateHashNum(itemNum, filter->bitSize);
    filter->itemMaxNum = itemNum;
    CheckProbability(itemNum, probability, filter->bitSize, filter->hashNum);
    filter->bitArr = calloc(filter->bitSize, 1);
    if (filter->bitArr == NULL) {
        free(filter);
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CreateBloomFilter| malloc bitArr wrong");
        return NULL;
    }
    return filter;
}

// 设置位
static inline void SetBit(BloomFilterT *filter, uint32_t pos) {
    uint32_t byte = pos >> 3; // 相当于/8
    uint32_t bit = pos & 0x7; // mod 8
    filter->bitArr[byte] |= (1 << bit);
}

// 检查位
static inline bool BitIsSet(BloomFilterT *filter, uint32_t pos) {
    uint32_t byte = pos >> 3; // 相当于/8
    uint32_t bit = pos & 0x7; // mod 8
    return (filter->bitArr[byte] & (1 << bit)) != 0;
}

// 添加元素
void BloomFilterInsert(BloomFilterT *filter, ConstBufT key) {
    if (++filter->itemCount > filter->itemMaxNum) {
        LOG_WARNING(JIEBA_CONFIG_WRONG, "itemCount exceed the set value !, set num:%d, count now:%d",
                    filter->itemMaxNum, filter->itemCount);
    }
    for (uint32_t i = 0; i < filter->hashNum; ++i) {
        uint32_t hash = XXH32(key.buf, key.bufLen, i);
        uint32_t pos = hash % (filter->bitSize << 3);
        SetBit(filter, pos);
    }
}

bool BloomFilterContain(BloomFilterT *filter, ConstBufT key) {
    for (uint32_t i = 0; i < filter->hashNum; ++i) {
        uint32_t hash = XXH32(key.buf, key.bufLen, i);
        uint32_t pos = hash % (filter->bitSize << 3);
        if (!BitIsSet(filter, pos)) {
            return false;
        }
    }
    return true;
}

void DestroyBloomFilter(BloomFilterT *filter) {
    if (filter == NULL) {
        return;
    }
    if (filter->bitArr != NULL) {
        free(filter->bitArr);
        filter->bitArr = NULL;
    }
    *filter = (BloomFilterT){};
    free(filter);
}
