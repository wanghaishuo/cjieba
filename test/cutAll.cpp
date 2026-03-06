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
    FreeWordList(wordList);
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

TEST_F(CutAllTest, CutAllTest004) {
    CutTest("许三观", "许 三观 ");
}

TEST_F(CutAllTest, CutAllTest005) {
    const char *str =
        "许三观说：“我知道黄金瓜，那瓜肉特别香，就是不怎么甜，倒是中间的籽很甜，城里人吃黄金瓜都把籽吐掉，我从来不吐，"
        "从土里长出来的只要能吃，就都有营养……老太婆瓜，我也吃过，那瓜不甜，也不脆，吃到嘴里黏糊糊的，吃那种瓜有没有牙齿"
        "都一样……四叔，我好像还能吃，我再吃两个黄金瓜，再吃一个老太婆瓜……”许三观在他叔叔的瓜田里一坐就是一天，到了傍晚"
        "来到的时候，许三观站了起来，落日的光芒把他的脸照得像猪肝一样通红，他看了看远处农家屋顶上升起的炊烟，拍了拍屁股"
        "上的尘土，然后双手伸到前面去摸胀鼓鼓的肚子，里面装满了西瓜、黄金瓜、老太婆瓜，还有黄瓜和桃子。许三观摸着肚子对"
        "他的叔叔说：“我要去结婚了。”";
    string ans = "许 三观 说 ： “ 我 知道 黄金 金瓜 ， 那 瓜肉 特别 香 ， 就是 不怎么 怎么 甜 ， 倒 是 中间 的 籽 很甜 "
                 "， 城里 城里人 吃 黄金 金瓜 都 把 籽 吐掉 ， 我 从来 从来不 吐 ， 从 土里 里长 出来 的 只要 要能 吃 "
                 "， 就 都 有 营养 … … 老太 老太婆 太婆 瓜 ， 我 也 吃 过 ， 那 瓜 不 甜 ， 也 不 脆 ， 吃到 嘴里 黏糊 "
                 "黏糊糊 糊糊 的 ， 吃 那种 种瓜 有没有 没有 牙齿 都 一样 … … 四叔 ， 我 好像 还 能 吃 ， 我 再 吃 "
                 "两个 黄金 金瓜 ， 再 吃 一个 老太 老太婆 太婆 瓜 … … ” 许 三观 在 他 叔叔 的 瓜田 田里 一 坐 就是 "
                 "一天 ， 到 了 傍晚 晚来 来到 的 时候 ， 许 三观 站 了 起来 ， 落日 的 光芒 把 他 的 脸 照得 像 猪肝 "
                 "一样 通红 ， 他 看 了 看 远处 农家 家屋 屋顶 顶上 上升 升起 的 炊烟 ， 拍了拍 屁股 上 的 尘土 ， "
                 "然后 双手 伸到 前面 去 摸 胀鼓鼓 鼓鼓 鼓鼓的 肚子 ， 里面 装满 了 西瓜 、 黄金 金瓜 、 老太 老太婆 "
                 "太婆 瓜 ， 还有 黄瓜 和 桃子 。 许 三观 摸 着 肚子 对 他 的 叔叔 说 ： “ 我 要 去 结婚 了 。 ” ";
    CutTest(str, ans);
}
