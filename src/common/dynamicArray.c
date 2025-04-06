#include "dynamicArray.h"
#include "log.h"

typedef struct DynArr {
    void *items;
    uint32_t itemSize;
    uint32_t itemNum;
    uint32_t maxItemNum;
} DynArrT;

#define DEFAULT_ITEM_NUM (16)

DynArrT *CreateDynArr(uint32_t itemSize, uint32_t itemNum) {
    if (itemSize == 0) {
        LOG_ERROR(JIEBA_CONFIG_WRONG, "|CreateDynArr| itemSize 0");
        return NULL;
    }
    DynArrT *arr = (DynArrT *)malloc(sizeof(DynArrT));
    if (arr == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CreateDynArr| malloc arr wrong");
        return NULL;
    }
    *arr = (DynArrT){0};
    arr->maxItemNum = itemNum ? itemNum : DEFAULT_ITEM_NUM;
    arr->itemSize = itemSize;
    arr->itemNum = 0;
    arr->items = calloc(arr->maxItemNum, itemSize);
    if (arr->items == NULL) {
        free(arr);
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CreateDynArr| malloc items wrong");
        return NULL;
    }
    return arr;
}

void DestroyDynArr(DynArrT *arr) {
    if (arr == NULL) {
        return;
    }
    if (arr->items != NULL) {
        free(arr->items);
        arr->items = NULL;
    }
    free(arr);
}

static ErrorT DynArrExpand(DynArrT *arr) {
    arr->maxItemNum <<= 1;
    void *ptr = realloc(arr->items, arr->maxItemNum * arr->itemSize);
    if (ptr == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|DynArrResize| malloc items wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    arr->items = ptr;
    return JIEBA_OK;
}

ErrorT DynArrPushBack(DynArrT *arr, const void *item) {
    if (arr->itemNum == arr->maxItemNum) {
        ErrorT ret = DynArrExpand(arr);
        if (ret != JIEBA_OK) {
            LOG_ERROR(ret, "|DynArrPushBack| DynArrResize wrong");
            return ret;
        }
    }
    JIEBA_ASSERT(arr->itemNum < arr->maxItemNum);
    memcpy(arr->items + arr->itemSize * arr->itemNum, item, arr->itemSize);
    arr->itemNum++;
    return JIEBA_OK;
}

// 删除末尾元素
void DynArrPopBack(DynArrT *arr) {
    JIEBA_ASSERT(arr->itemNum > 0);
    arr->itemNum--;
}

void *DynArrItem(DynArrT *arr, uint32_t index) {
    JIEBA_ASSERT(index < arr->itemNum);
    return arr->items + index * arr->itemSize;
}

void DynArrClear(DynArrT *arr) {
    arr->itemNum = 0;
}

ErrorT DynArrReserve(DynArrT *arr, uint32_t itemNum) {
    if (itemNum <= arr->maxItemNum) {
        return JIEBA_OK;
    }
    void *ptr = calloc(itemNum, arr->itemSize);
    if (ptr == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|DynArrReserve| calloc wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    memcpy(ptr, arr->items, arr->itemSize * arr->itemNum);
    free(arr->items);
    arr->items = ptr;
    arr->maxItemNum = itemNum;
    return JIEBA_OK;
}

uint32_t DynArrSize(DynArrT *arr) {
    return arr->itemNum;
}

ErrorT DynArrResize(DynArrT *arr, uint32_t itemNum) {
    if (itemNum > arr->maxItemNum) {
        ErrorT ret = DynArrReserve(arr, itemNum);
        if (ret != JIEBA_OK) {
            LOG_ERROR(ret, "|DynArrResize| DynArrReserve wrong");
            return ret;
        }
        arr->itemNum = itemNum;
        return ret;
    }
    if (itemNum < arr->itemNum) {
        memset(arr->items + arr->itemSize * arr->itemNum, 0, (arr->itemNum - itemNum) * arr->itemSize);
    }
    arr->itemNum = itemNum;
    return JIEBA_OK;
}

void DynArrSetItem(DynArrT *arr, uint32_t index, const void *item) {
    JIEBA_ASSERT(index < arr->itemNum);
    memcpy(arr->items + arr->itemSize * index, item, arr->itemSize);
}
