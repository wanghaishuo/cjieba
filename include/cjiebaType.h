#ifndef C_JIEBA_TYPE_H
#define C_JIEBA_TYPE_H

#include <stdint.h>

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

typedef enum CutType {
    CUT_BY_PROBABILITY = 0, // 基于概率分词
} CutTypeT;

typedef struct CutCfg {
    CutTypeT CutType;
} CutCfgT;

typedef struct JieBaWord {
    const char *word;
    uint32_t length;
} JieBaWordT;

typedef struct WordList WordListT;

#define JIEBA_EXE_OK 0
#define SENTENCE_INVALID 1401
#define JIEBA_NEXT_END 1402

#ifdef __cplusplus
}
#endif

#endif // C_JIEBA_TYPE_H