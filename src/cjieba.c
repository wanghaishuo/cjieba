#include "cjieba.h"
#include "cutBase.h"
#include "log.h"

typedef struct CJieBa {
    PrefixDictT dict;
} CJieBaT;

// 创建结巴实例
int CreateCJieBa(JieBaParaT para, JieBaCfgT cfg, CJieBaT **cJieba) {
    JIEBA_UNUSED(cfg);
    CJieBaT *jieba = (CJieBaT *)malloc(sizeof(CJieBaT));
    if (jieba == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CreateCJieBa| malloc jieba wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    *jieba = (CJieBaT){};
    ErrorT ret = BuildPrefixDixt(para.dictPath, NULL, &jieba->dict);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|CreateCJieBa| BuildPrefixDixt wrong");
        free(jieba);
        return ret;
    }
    *cJieba = jieba;
    return JIEBA_OK;
}

void DestroyCJieBa(CJieBaT *cJieba) {
    if (cJieba == NULL) {
        return;
    }
    DestroyPrefixDixt(&cJieba->dict);
    free(cJieba);
}

typedef struct WordList {
    char *sentence;
    uint32_t length;
    WordOutArrT arr;
    uint32_t index; // 访问到第几个元素
    char lastCh;
} WordListT;

// 释放wordlist
void FreeWordList(WordListT *list) {
    if (list == NULL) {
        return;
    }
    if (list->sentence != NULL) {
        free(list->sentence);
        list->sentence = NULL;
    }
    if (list->arr != NULL) {
        DestroyDynArr(list->arr);
        list->arr = NULL;
    }
}

int JieBaCut(CJieBaT *cJieba, const char *sentence, uint32_t length, CutCfgT cfg, WordListT **wordList) {
    JIEBA_UNUSED(cfg);
    if (sentence == NULL || length == 0) {
        LOG_ERROR(SENTENCE_INVALID, "|JieBaCut| sentence invalid");
        return SENTENCE_INVALID;
    }
    WordListT *list = (WordListT *)malloc(sizeof(WordListT));
    if (list == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|JieBaCut| malloc list wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    *list = (WordListT){};
    list->sentence = (char *)malloc(length + 1);
    if (list->sentence == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|JieBaCut| malloc sentence wrong");
        free(list);
        return JIEBA_MEMORY_OP_WRONG;
    }
    memcpy(list->sentence, sentence, length);
    list->sentence[length] = '\0';
    list->lastCh = sentence[0];
    ConstBufT buf = {.buf = sentence, .bufLen = length};
    ErrorT ret = BaseCut(&cJieba->dict, buf, &list->arr);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|JieBaCut| BaseCut wrong");
        goto EXIT;
    }
    *wordList = list;
    return JIEBA_OK;
EXIT:
    FreeWordList(list);
    return ret;
}

int JieBaNext(WordListT *list, JieBaWordT *word) {
    if (list->index >= DynArrSize(list->arr)) {
        return JIEBA_NEXT_END;
    }
    WordOutT wordOut = WordArrItem(list->arr, list->index);
    word->word = list->sentence + wordOut.offset;
    word->length = wordOut.len;
    list->sentence[wordOut.offset] = list->lastCh;
    list->lastCh = list->sentence[wordOut.offset + wordOut.len];
    list->sentence[wordOut.offset + wordOut.len] = '\0';
    list->index++;
    return JIEBA_OK;
}
