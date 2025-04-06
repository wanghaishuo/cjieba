#ifndef C_JIEBA_H
#define C_JIEBA_H

#include "cjiebaType.h"

#ifdef __cplusplus
extern "C" {
#endif

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