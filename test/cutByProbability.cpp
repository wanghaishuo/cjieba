#include "cjieba.h"
#include "log.h"
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace std;

static string GetDictPath(string dict);

static CJieBaT *g_cJieba = NULL;

static CutCfgT g_cutCfg = {.cutType = CUT_BY_PROBABILITY};

class CutByProbabilityTest : public ::testing::Test {
protected:
    // 整个测试套件执行前的初始化
    static void SetUpTestSuite() {
        LogInit(NULL, LOG_DEBUG);
        string path = GetDictPath("jieba.dict.utf8");
        JieBaParaT para = {.dictPath = path.c_str()};
        JieBaCfgT cfg = {.initTpye = JIEBA_LOAD_DICT};
        ASSERT_EQ(CreateCJieBa(para, cfg, &g_cJieba), JIEBA_EXE_OK);
    }
    // 整个测试套件执行后的清理
    static void TearDownTestSuite() {
        DestroyCJieBa(g_cJieba);
        LogClose();
    }
};

#define PATH_MAX FILENAME_MAX

// 获取cjieba路径，随后字符串拼接找到绝对路径
static string GetDictPath(string dict) {
    char buffer[PATH_MAX] = {};
    string path;
    if (getcwd(buffer, sizeof(buffer)) == nullptr) {
        std::cout << "getcwd wrong" << std::endl;
        assert(false);
        return path;
    }
    path = buffer;
    string basePath = "/cjieba";
    int pos = path.find(basePath);
    if (pos == -1) {
        std::cout << "exec path wrong, no cjieba found, path :" << path << std::endl;
        assert(false);
        return path;
    }
    path.resize(pos + basePath.size());
    string relativePath = "/dict/";
    path += relativePath;
    path += dict;
    return path;
}

void CutTest(const char *str, vector<string> &result) {
    WordListT *wordList = NULL;
    ASSERT_EQ(JieBaCut(g_cJieba, str, strlen(str), g_cutCfg, &wordList), JIEBA_EXE_OK);
    JieBaWordT word;
    int ret = JIEBA_EXE_OK;
    int num = 0;
    while ((ret = JieBaNext(wordList, &word)) == JIEBA_EXE_OK) {
        printf("%s %d ", word.word, word.length);
        EXPECT_EQ(string(word.word), result[num++]);
    }
}

TEST_F(CutByProbabilityTest, CutByProbabilityTest001) {
    char str[] = "他来到了网易杭研大厦";
    vector<string> result = {"他", "来到", "了", "网易", "杭", "研", "大厦"};
    CutTest("他来到了网易杭研大厦", result);
}
