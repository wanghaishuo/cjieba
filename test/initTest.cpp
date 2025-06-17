#include "cjieba.h"
#include "log.h"
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace std;

class InitTest : public ::testing::Test {
protected:
    // 整个测试套件执行前的初始化
    static void SetUpTestSuite() {
        LogInit(NULL, LOG_DEBUG);
    }
    // 整个测试套件执行后的清理
    static void TearDownTestSuite() {
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

static void GetMem() {
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 6) == "VmSize:") {
            std::cout << "Virtual memory size: " << line.substr(line.find(':') + 1) << std::endl;
        } else if (line.substr(0, 6) == "VmRSS:") {
            std::cout << "Resident Set Size: " << line.substr(line.find(':') + 1) << std::endl;
        }
    }
    file.close();
}

// JIEBA_LOAD_DICT
TEST_F(InitTest, InitTest001) {
    string path = GetDictPath("jieba.dict.utf8");
    JieBaParaT para = {.dictPath = path.c_str()};
    JieBaCfgT cfg = {.initTpye = JIEBA_LOAD_DICT};
    CJieBaT *cJieba = NULL;
    cout << "mem before CreateCJieBa:" << endl;
    GetMem();
    ASSERT_EQ(CreateCJieBa(para, cfg, &cJieba), JIEBA_EXE_OK);
    cout << "mem after CreateCJieBa:" << endl;
    GetMem();
    DestroyCJieBa(cJieba);
    cout << "mem after DestroyCJieBa:" << endl;
    GetMem();
}
