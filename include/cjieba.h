#ifndef C_JIEBA_H
#define C_JIEBA_H

#include <stdint.h>
#include "baseType.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct JieBaPara {
    const char *dictPath; // 目前仅支持单个词典，后续进行拓展
} JieBaParaT;

#define JIEBA_LOAD_DICT 0x1

typedef struct JieBaCfg {
    uint32_t initTpye;
} JieBaCfgT;

typedef struct CJieBa CJieBaT;

typedef struct CutCfg {
    CutTypeT cutType;
} CutCfgT;

typedef struct JieBaWord {
    const char *word;
    uint32_t length;
} JieBaWordT;

typedef struct WordList WordListT;

#define JIEBA_EXE_OK 0
#define SENTENCE_INVALID 1401
#define JIEBA_NEXT_END 1402

// 创建结巴实例
int CreateCJieBa(JieBaParaT para, JieBaCfgT cfg, CJieBaT **cJieba);

// 销毁结巴实例
void DestroyCJieBa(CJieBaT *cJieba);

// 执行分词, list为出参
int JieBaCut(CJieBaT *cJieba, const char *sentence, uint32_t length, CutCfgT cfg, WordListT **wordList);

// 从cut得到的list里面获取下一个词
int JieBaNext(WordListT *list, JieBaWordT *word);

// 释放wordlist
void FreeWordList(WordListT *list);

#ifdef __cplusplus
}
#endif

#endif // C_JIEBA_H