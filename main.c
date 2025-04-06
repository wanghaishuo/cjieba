#include "cjieba.h"
#include "cutBase.h"
#include "easyHash.h"
#include "log.h"
#include "prefixDict.h"
#include "stdio.h"
#include "unicode.h"

void HashTest() {
    int num = 100;
    int valueSize = sizeof(uint32_t);
    HashMapT *map = CreateHashmap(valueSize, num);
    ConstBufT key = {};
    key.bufLen = sizeof(uint32_t);
    for (uint32_t i = 0; i < num; ++i) {
        key.buf = &i;
        HashInsert(map, key, &i);
    }
    for (uint32_t i = 0; i < num; ++i) {
        key.buf = &i;
        printf("%u\n", *(uint32_t *)GetHashValue(map, key));
    }
    DestroyHashmap(map);
}

void DynArrTest() {
    int num = 100;
    int valueSize = sizeof(uint32_t);
    DynArrT *arr = CreateDynArr(valueSize, num);
    assert(arr != NULL);
    for (uint32_t i = 0; i < num; ++i) {
        DynArrPushBack(arr, &i);
    }
    for (uint32_t i = 0; i < num; ++i) {
        printf("%u\n", *(uint32_t *)DynArrItem(arr, i));
    }
    DestroyDynArr(arr);
}

void UnicodeTest() {
    char str[] = "我是谁";
    DynArrT *arr = NULL;
    DecodeUTF8RunesInString(str, strlen(str), &arr);
    arr;
}

void PrefixDictTest() {
    PrefixDictT dict;
    ErrorT ret = BuildPrefixDixt("/home/whs/code/tokenizer/cjieba/dict/jieba.dict.utf8", NULL, &dict);
    DestroyPrefixDixt(&dict);
    ret;
}

void CutBaseTest() {
    PrefixDictT dict;
    ErrorT ret = BuildPrefixDixt("/home/whs/code/tokenizer/cjieba/dict/jieba.dict.utf8", NULL, &dict);
    char str[] = "..喜欢夏天，喜欢冬天。";
    ConstBufT buf = {str, strlen(str)};
    // BaseCut(&dict, buf);
    DestroyPrefixDixt(&dict);
    ret;
}

void JiebaTest() {
    JieBaParaT para = {.dictPath = "/home/whs/code/tokenizer/cjieba/dict/jieba.dict.utf8"};
    JieBaCfgT cfg1;
    CJieBaT *cJieba = NULL;
    int ret = CreateCJieBa(para, cfg1, &cJieba);

    char str[] = "..喜欢夏天，喜欢冬天。好！好";
    CutCfgT cfg2;
    WordListT *wordList = NULL;
    ret = JieBaCut(cJieba, str, strlen(str), cfg2, &wordList);
    JieBaWordT word;
    while (ret == JIEBA_EXE_OK) {
        ret = JieBaNext(wordList, &word);
        printf("%s %d\n", word.word, word.length);
    }
}

int main() {
    LogInit(NULL, LOG_DEBUG);
    JiebaTest();
    LogClose();
}
