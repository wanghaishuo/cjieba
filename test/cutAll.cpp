#include "cjieba.h"
#include "log.h"
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace std;

static string GetDictPath(string dict);

static CJieBaT *g_cJieba = NULL;

static CutCfgT g_cutCfg = {.cutType = CUT_ALL};

class CutAllTest : public ::testing::Test {
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

static void CutTest(const char *str, string result) {
    WordListT *wordList = NULL;
    ASSERT_EQ(JieBaCut(g_cJieba, str, strlen(str), g_cutCfg, &wordList), JIEBA_EXE_OK);
    JieBaWordT word;
    int ret = JIEBA_EXE_OK;
    int num = 0;
    string tmp;
    while ((ret = JieBaNext(wordList, &word)) == JIEBA_EXE_OK) {
        tmp += string(word.word, word.length);
        tmp += " ";
    }
    cout << tmp << endl;
    EXPECT_EQ(tmp, result);
}

TEST_F(CutAllTest, CutAllTest001) {
    CutTest("清华大学", "清华 清华大学 华大 大学 ");
}

TEST_F(CutAllTest, CutAllTest002) {
    CutTest("c语言很cbc语言", "c语言 语言 很 cb c语言 语言 ");
}

TEST_F(CutAllTest, CutAllTest003) {
    CutTest("cbad bc哈哈cb,ui和ai", "cbad   bc 哈哈 cb , ui 和 ai ");
}
