#ifndef PREFIX_DICT_H
#define PREFIX_DICT_H

#include "bloomFilter.h"
#include "easyHash.h"
#include "unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEPARATOR_NUM 30
// 常用中英文分隔符
#define SENTENCE_SEPARATOR "，。！？：；“”!,.?\"‘’':&"

typedef struct PrefixDict {
    HashMapT *map;        // 词->wordinfo
    BloomFilterT *filter; // 词前缀的存在性
    uint32_t wordNum;     // 词总数
    double totalFreq;     // 总和取对数
    RuneT separator[SEPARATOR_NUM];
} PrefixDictT;

typedef struct WordInfo {
    uint16_t freq;
} WordInfoT;

ErrorT BuildPrefixDixt(const char *dafaultDict, const char *userDict, PrefixDictT *dict);

void DestroyPrefixDixt(PrefixDictT *dict);

// true 表示找到
static inline bool GetWordInfo(PrefixDictT *dict, ConstBufT key, WordInfoT *info) {
    void *ptr = GetHashValue(dict->map, key);
    if (ptr == NULL) {
        return false;
    }
    *info = *(WordInfoT *)ptr;
    return true;
}

static inline bool IsSeparator(RuneT *separators, RuneT rune) {
    for (uint32_t i = 0; separators[i] != 0; ++i) {
        if (rune == separators[i]) {
            return true;
        }
    }
    return false;
}

#ifdef __cplusplus
}
#endif

#endif // PREFIX_DICT_H