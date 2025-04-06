#ifndef EASY_HASH_H
#define EASY_HASH_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HashMap HashMapT;

HashMapT *CreateHashmap(uint32_t valueSize, uint32_t mapSize);

void HashInsert(HashMapT *map, ConstBufT key, void *value);

void *GetHashValue(HashMapT *map, ConstBufT key);

void DestroyHashmap(HashMapT *map);

#ifdef __cplusplus
}
#endif

#endif // EASY_HASH_H