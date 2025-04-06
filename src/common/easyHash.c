#include "easyHash.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <xxhash.h>

#define HASH_SIZE_RATE 1.4 // 申请空间与node所需真实空间的比例
#define BASE_HASH_SEED 0
#define SECOND_HASH_SEED 31
#define HASH_SIZE sizeof(uint32_t)

// 哈希表结构体
typedef struct HashMap {
    uint32_t valueSize; // 定长 valueSize，如果要变长传入指针即可
    uint32_t mapSize;   // 哈希表含有最多node的数量
    uint32_t nodeNum;   // 当前num数量，用于校验配置合理
    void *nodes;
} HashMapT;

// 创建哈希表，不支持动态扩充，如果要支持可以将hash多保存一份到node里
HashMapT *CreateHashmap(uint32_t valueSize, uint32_t mapSize) {
    HashMapT *map = (HashMapT *)malloc(sizeof(HashMapT));
    if (map == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CreateHashmap| malloc map wrong");
        return NULL;
    }
    map->mapSize = mapSize * HASH_SIZE_RATE;
    map->valueSize = valueSize;
    map->nodes = calloc(map->mapSize, valueSize + HASH_SIZE);
    if (map->nodes == NULL) {
        free(map);
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CreateHashmap| malloc nodes wrong");
        return NULL;
    }
    return map;
}

// 插入键值对, key可变长，value不可以
void HashInsert(HashMapT *map, ConstBufT key, void *value) {
    if (++map->nodeNum > map->mapSize) {
        LOG_WARNING(JIEBA_CONFIG_WRONG, "|HashInsert| nodeNum exceed set value");
    }
    uint32_t hash1 = XXH32(key.buf, key.bufLen, BASE_HASH_SEED);   // 用于找到位置
    uint32_t hash2 = XXH32(key.buf, key.bufLen, SECOND_HASH_SEED); // 用于确认是否是相同key
    uint32_t baseSize = map->valueSize + HASH_SIZE;

    uint32_t idx = hash1 % map->mapSize;
    while ((hash1 = *(uint32_t *)(map->nodes + idx * baseSize)) != 0) { // 找到第一个没被赋值的地方
        if (hash1 == hash2) {                                           // 已有相同的key，直接更新value
            memmove(map->nodes + idx * baseSize + HASH_SIZE, value, map->valueSize);
            return;
        }
        idx = (idx + 1) % map->mapSize;
    }
    memmove(map->nodes + idx * baseSize, &hash2, HASH_SIZE);
    memmove(map->nodes + idx * baseSize + HASH_SIZE, value, map->valueSize);
}

// 查找键对应的值
void *GetHashValue(HashMapT *map, ConstBufT key) {
    uint32_t hash1 = XXH32(key.buf, key.bufLen, BASE_HASH_SEED);   // 用于找到位置
    uint32_t hash2 = XXH32(key.buf, key.bufLen, SECOND_HASH_SEED); // 用于确认是否是相同key
    uint32_t baseSize = map->valueSize + HASH_SIZE;

    uint32_t idx = hash1 % map->mapSize;
    while ((hash1 = *(uint32_t *)(map->nodes + idx * baseSize)) != 0) { // 找到空闲位置停止寻找
        if (hash1 == hash2) {                                           // 找到相同的key
            return map->nodes + idx * baseSize + HASH_SIZE;
        }
        idx = (idx + 1) % map->mapSize;
    }
    return NULL; // 未找到
}

// 销毁哈希表
void DestroyHashmap(HashMapT *map) {
    if (map == NULL) {
        return;
    }
    if (map->nodes != NULL) {
        free(map->nodes);
        map->nodes = NULL;
    }
    free(map);
}
