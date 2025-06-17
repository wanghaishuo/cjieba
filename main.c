#include "cjieba.h"
#include "cutBase.h"
#include "easyHash.h"
#include "log.h"
#include "prefixDict.h"
#include "stdio.h"
#include "unicode.h"

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
