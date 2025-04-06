#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DynArr DynArrT;

typedef DynArrT *DynArrPtr;

// 初始可以设置item的数量，如果入参为0，那么将付一个默认初始值
DynArrT *CreateDynArr(uint32_t itemSize, uint32_t itemNum);

ErrorT DynArrPushBack(DynArrT *arr, const void *item);

void DynArrSetItem(DynArrT *arr, uint32_t index, const void *item);

void DynArrPopBack(DynArrT *arr);

// 如果原本的大小大于等于新的大小，可不判断返回值
ErrorT DynArrResize(DynArrT *arr, uint32_t itemNum);

void *DynArrItem(DynArrT *arr, uint32_t index);

void DestroyDynArr(DynArrT *arr);

void DynArrClear(DynArrT *arr);

// 如果原本的大小大于等于新的大小，可不判断返回值
ErrorT DynArrReserve(DynArrT *arr, uint32_t itemNum);

uint32_t DynArrSize(DynArrT *arr);

#ifdef __cplusplus
}
#endif

#endif // DYNAMIC_ARRAY_H