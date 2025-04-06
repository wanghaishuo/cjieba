#include "prefixDict.h"
#include "log.h"
#include <math.h>

#define MAX_LINE_LEN 256
#define MAX_WORD_LEN 128
#define MAX_PART_OF_SPEECH_LEN 6

uint32_t GetFileLineNum(FILE *fd) {
    char buffer[MAX_LINE_LEN];
    uint32_t num = 0;
    while (fgets(buffer, sizeof(buffer), fd) != NULL) {
        num++;
    }
    rewind(fd); // 将句柄移回到开头
    return num;
}

ErrorT GetSeparator(PrefixDictT *dict) {
    DynArrT *separators = NULL;
    ErrorT ret = DecodeUTF8RunesInString(SENTENCE_SEPARATOR, strlen(SENTENCE_SEPARATOR), &separators);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|GetSeparator| DecodeUTF8RunesInString wrong");
        return ret;
    }
    for (uint32_t i = 0; i < DynArrSize(separators); ++i) {
        dict->separator[i] = RunesItem(separators, i)->rune;
    }
    DestroyDynArr(separators);
    return JIEBA_OK;
}

ErrorT AddWordToDict(FILE *fd, PrefixDictT *dict) {
    char buffer[MAX_LINE_LEN];
    char wordBuf[MAX_WORD_LEN];
    char SpeechBuf[MAX_PART_OF_SPEECH_LEN];
    uint16_t freq;
    uint64_t totalfreq = 0;
    HashMapT *map = dict->map;
    BloomFilterT *filter = dict->filter;
    while (fgets(buffer, sizeof(buffer), fd) != NULL) {
        if (sscanf(buffer, "%128s %hd %6s", wordBuf, &freq, SpeechBuf) != 3) {
            LOG_ERROR(JIEBA_CONFIG_WRONG, "|AddWordToMap| analyze buf wrong, buf : %s", buffer);
            return JIEBA_CONFIG_WRONG;
        };
        totalfreq += freq;
        int len = strlen(wordBuf);
        DynArrT *arr = NULL;
        ErrorT ret = DecodeUTF8RunesInString(wordBuf, len, &arr);
        if (ret != JIEBA_OK) {
            LOG_ERROR(ret, "|AddWordToMap| DecodeUTF8RunesInString wrong");
            return ret;
        }
        HashInsert(map, (ConstBufT){wordBuf, strlen(wordBuf)}, &(WordInfoT){freq});
        uint32_t endPos = 0;
        for (int i = 0; i < DynArrSize(arr); ++i) {
            RuneStrT *runeStr = (RuneStrT *)DynArrItem(arr, i);
            endPos += runeStr->len;
            BloomFilterInsert(filter, (ConstBufT){wordBuf, endPos});
        }
        JIEBA_ASSERT(endPos == strlen(wordBuf));
        DestroyDynArr(arr);
    }
    dict->totalFreq = log(totalfreq);
    return JIEBA_OK;
}

#define PREFIX_RATE 3            // 假设词平均词长3
#define FALSE_POSITIVE_RATE 1e-4 // 假阳率为万分之一

ErrorT BuildPrefixDixt(const char *dafaultDict, const char *userDict, PrefixDictT *dict) {
    LOG_INFO(JIEBA_OK, "|BuildPrefixDixt| build dict begin");
    JIEBA_UNUSED(userDict);
    *dict = (PrefixDictT){};
    FILE *fd1 = NULL;
    if (dafaultDict == NULL || (fd1 = fopen(dafaultDict, "r")) == NULL) {
        LOG_ERROR(JIEBA_CONFIG_WRONG, "|BuildPrefixDixt| file path wrong");
        return JIEBA_CONFIG_WRONG;
    }
    dict->wordNum = GetFileLineNum(fd1);
    HashMapT *map = CreateHashmap(sizeof(WordInfoT), dict->wordNum);
    if (map == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|BuildPrefixDixt| CreateHashmap wrong");
        fclose(fd1);
        return JIEBA_MEMORY_OP_WRONG;
    }
    BloomFilterT *filter = CreateBloomFilter(dict->wordNum * PREFIX_RATE, FALSE_POSITIVE_RATE);
    if (filter == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|BuildPrefixDixt| CreateBloomFilter wrong");
        DestroyHashmap(map);
        fclose(fd1);
        return JIEBA_MEMORY_OP_WRONG;
    }
    dict->map = map;
    dict->filter = filter;
    ErrorT ret = AddWordToDict(fd1, dict);
    if (ret != JIEBA_OK) {
        DestroyPrefixDixt(dict);
        fclose(fd1);
        LOG_ERROR(ret, "|BuildPrefixDixt| AddWordToMap wrong");
        return ret;
    }
    ret = GetSeparator(dict);
    if (ret != JIEBA_OK) {
        DestroyPrefixDixt(dict);
        fclose(fd1);
        LOG_ERROR(ret, "|BuildPrefixDixt| GetSeparator wrong");
        return ret;
    }
    return JIEBA_OK;
}

void DestroyPrefixDixt(PrefixDictT *dict) {
    if (dict == NULL) {
        return;
    }
    if (dict->filter != NULL) {
        DestroyBloomFilter(dict->filter);
    }
    if (dict->map != NULL) {
        DestroyHashmap(dict->map);
    }
    *dict = (PrefixDictT){};
}
