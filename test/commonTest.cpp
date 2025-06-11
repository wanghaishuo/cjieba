#include "bloomFilter.h"
#include "dynamicArray.h"
#include "easyHash.h"
#include "log.h"
#include "unicode.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace std;

class CommonTest : public ::testing::Test {
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

// test for DynArr
TEST_F(CommonTest, DynArrTest001) {
    int num = 100;
    int valueSize = sizeof(uint32_t);
    DynArrT *arr = CreateDynArr(valueSize, num);
    ASSERT_TRUE(arr != NULL);
    for (uint32_t i = 0; i < num; ++i) {
        DynArrPushBack(arr, &i);
    }
    ASSERT_EQ(DynArrSize(arr), num);
    for (uint32_t i = 0; i < num; ++i) {
        EXPECT_EQ(*(uint32_t *)DynArrItem(arr, i), i);
    }
    DestroyDynArr(arr);
}

// test for hashmap
TEST_F(CommonTest, Hashmap001) {
    int num = 100;
    int valueSize = sizeof(uint32_t);
    HashMapT *map = CreateHashmap(valueSize, num);
    ConstBufT key = {};
    key.bufLen = sizeof(uint32_t);
    for (uint32_t i = 0; i < num; ++i) {
        key.buf = &i;
        HashInsert(map, key, &i);
    }
    for (uint32_t i = 0; i < num; ++i) {
        key.buf = &i;
        EXPECT_EQ(*(uint32_t *)GetHashValue(map, key), i);
    }
    DestroyHashmap(map);
}

// unicode test
TEST_F(CommonTest, unicode001) {
    char str[] = "这是C结巴";
    DynArrT *arr = NULL;
    ASSERT_EQ(DecodeUTF8RunesInString(str, strlen(str), &arr), JIEBA_OK);
    ASSERT_EQ(DynArrSize(arr), 5); // 5个utf-8
    vector<string> strs = {"这", "是", "C", "结", "巴"};
    for (uint32_t i = 0; i < DynArrSize(arr); ++i) {
        RuneStrT *runeStr = RunesItem(arr, i);
        string tmp(str + runeStr->offset, runeStr->len);
        EXPECT_EQ(tmp, strs[i]);
        EXPECT_EQ(runeStr->unicodeLength, 1);
        EXPECT_EQ(runeStr->unicodeOffset, i);
    }
    DestroyDynArr(arr);
}

// bloomFilter test
TEST_F(CommonTest, bloomFilter001) {
    int num = 100;
    BloomFilterT *filter = CreateBloomFilter(num, 1e-4);
    ASSERT_TRUE(filter != NULL);
    ConstBufT key = {};
    key.bufLen = sizeof(uint32_t);
    for (uint32_t i = 0; i < num; ++i) {
        key.buf = &i;
        BloomFilterInsert(filter, key);
    }
    for (uint32_t i = 0; i < num; ++i) {
        key.buf = &i;
        EXPECT_TRUE(BloomFilterContain(filter, key));
    }
    // 设置的假阳率为万分之一，检测前1k个都能正确判断存在性
    for (uint32_t i = num; i < 1024; ++i) {
        key.buf = &i;
        EXPECT_FALSE(BloomFilterContain(filter, key));
    }
    DestroyBloomFilter(filter);
}
