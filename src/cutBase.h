#ifndef CUT_BASE_H
#define CUT_BASE_H

#include "prefixDict.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef DynArrT *DagT;
typedef DynArrT *PosArrT;
typedef DynArrT *WordOutArrT;

typedef struct SentToCut {
    ConstBufT buf;
    RuneStrArrT runes; // 范围为左闭右开
    uint32_t begin;    // runes 的起始位置
    uint32_t end;      // runes 的终止位置
} SentToCutT;

typedef struct WordOut {
    uint32_t offset; // 在句子中的偏移
    uint32_t len;    // 词长
} WordOutT;

// outDag为出参，为二维数组
ErrorT GenDag(PrefixDictT *dict, SentToCutT sentence, DagT *outDag);

static inline PosArrT DagItem(DagT dag, uint32_t i) {
    return *(PosArrT *)DynArrItem(dag, i);
}

static inline uint32_t PosArrItem(PosArrT arr, uint32_t i) {
    return *(uint32_t *)DynArrItem(arr, i);
}

void FreeDag(DagT dag);

// 成功后需调用DestroyDynArr释放资源
ErrorT BaseCut(PrefixDictT *dict, ConstBufT buf, WordOutArrT *arr);

static inline WordOutT WordArrItem(WordOutArrT arr, uint32_t index) {
    return *(WordOutT *)DynArrItem(arr, index);
}

#ifdef __cplusplus
}
#endif

#endif // CUT_BASE_H