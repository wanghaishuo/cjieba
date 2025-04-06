#ifndef UNICODE_H
#define UNICODE_H

#include "dynamicArray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t RuneT; // utf-8 一个字符最多4字节

typedef struct RuneStr {
    RuneT rune;
    uint32_t offset;
    uint32_t len;
    uint32_t unicode_offset;
    uint32_t unicode_length;
} RuneStrT;

typedef DynArrT *RuneStrArrT;
typedef DynArrT *BaseRuneArr;

// 将句子解析为unicode分别保存到数组中，需要调用DestroyDynArr释放资源, 每个元素均为RuneStrT
ErrorT DecodeUTF8RunesInString(const char *s, size_t len, RuneStrArrT *runes);

static inline RuneStrT *RunesItem(RuneStrArrT runes, uint32_t index) {
    return (RuneStrT *)DynArrItem(runes, index);
}

static inline RuneT *RuneArrItem(BaseRuneArr baseRunes, uint32_t index) {
    return (RuneT *)DynArrItem(baseRunes, index);
}

#ifdef __cplusplus
}
#endif

#endif // UNICODE_H