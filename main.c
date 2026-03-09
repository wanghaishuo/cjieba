#include "cjieba.h"
#include "log.h"
#include <stdlib.h>

#define PATH_MAX FILENAME_MAX

// 获取cjieba路径，随后字符串拼接找到绝对路径
static void GetDictPath(char *path) {
    const char *jiebaPath = getenv("CJIEBA_PATH");
    if (jiebaPath != NULL) {
        printf("CJIEBA_PATH = %s\n", jiebaPath);
    } else {
        printf("环境变量 CJIEBA_PATH 未设置\n");
        return;
    }
    memcpy(path, jiebaPath, strlen(jiebaPath));
    const char *dictPath = "/dict/jieba.dict.utf8";
    memcpy(path + strlen(jiebaPath), dictPath, strlen(dictPath));
}

void JiebaDemo() {
    char dictPath[PATH_MAX] = {};
    GetDictPath(dictPath);

    JieBaParaT para = {.dictPath = dictPath};
    JieBaCfgT cfg1 = {};
    CJieBaT *cJieba = NULL;
    int ret = CreateCJieBa(para, cfg1, &cJieba);

    char str[] = "我整日张大嘴巴打着呵欠，散漫地走在田间小道上，我的拖鞋吧哒吧哒，把那些小道弄得尘土飞扬。";
    CutCfgT cfg2 = {};
    WordListT *wordList = NULL;
    ret = JieBaCut(cJieba, str, strlen(str), cfg2, &wordList);
    if (ret != JIEBA_EXE_OK) {
        printf("JieBa Cut wrong\n");
        DestroyCJieBa(cJieba);
        return;
    }
    JieBaWordT word = {};
    while ((ret = JieBaNext(wordList, &word)) == JIEBA_EXE_OK) {
        printf("%s ", word.word);
    }
    printf("\n");
    if (ret != JIEBA_NEXT_END) {
        printf("JieBa Next wrong\n");
    }
    FreeWordList(wordList);
    DestroyCJieBa(cJieba);
}

int main() {
    LogInit(NULL, LOG_DEBUG);
    JiebaDemo();
    LogClose();
}
