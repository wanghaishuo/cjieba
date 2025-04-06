#include "unicode.h"
#include "log.h"

typedef struct RuneStrLite {
    uint32_t rune; // unicode值
    uint32_t len;
} RuneStrLiteT;

// rp len 为0表示非法
RuneStrLiteT DecodeUTF8ToRune(const char *str, size_t len) {
    RuneStrLiteT rp = {};
    if (str == NULL || len == 0) {
        JIEBA_ASSERT(false);
        return rp;
    }
    if (!(str[0] & 0x80)) { // 0xxxxxxx
        // 7bit, total 7bit
        rp.rune = (uint8_t)(str[0]) & 0x7f;
        rp.len = 1;
    } else if ((uint8_t)str[0] <= 0xdf && 1 < len) {
        // 110xxxxxx
        // 5bit, total 5bit
        rp.rune = (uint8_t)(str[0]) & 0x1f;

        // 6bit, total 11bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[1]) & 0x3f;
        rp.len = 2;
    } else if ((uint8_t)str[0] <= 0xef && 2 < len) { // 1110xxxxxx
        // 4bit, total 4bit
        rp.rune = (uint8_t)(str[0]) & 0x0f;

        // 6bit, total 10bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[1]) & 0x3f;

        // 6bit, total 16bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[2]) & 0x3f;

        rp.len = 3;
    } else if ((uint8_t)str[0] <= 0xf7 && 3 < len) { // 11110xxxx
        // 3bit, total 3bit
        rp.rune = (uint8_t)(str[0]) & 0x07;

        // 6bit, total 9bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[1]) & 0x3f;

        // 6bit, total 15bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[2]) & 0x3f;

        // 6bit, total 21bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[3]) & 0x3f;

        rp.len = 4;
    } else {
        rp.rune = 0;
        rp.len = 0;
    }
    return rp;
}

// runs为出参，将句子进行解析，并按unicode保存在数组中
ErrorT DecodeUTF8RunesInString(const char *s, size_t len, RuneStrArrT *runes) {
    if (s == NULL || len == 0) {
        LOG_ERROR(JIEBA_PARAMETER_WRONG, "|DecodeUTF8RunesInString| sentence invalid");
        return JIEBA_PARAMETER_WRONG;
    }
    DynArrT *tmpRunes = CreateDynArr(sizeof(RuneStrT), len >> 1);
    if (tmpRunes == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|DecodeUTF8RunesInString| CreateDynArr wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    for (uint32_t i = 0, j = 0; i < len;) {
        RuneStrLiteT rp = DecodeUTF8ToRune(s + i, len - i);
        if (rp.len == 0) {
            DestroyDynArr(tmpRunes);
            LOG_ERROR(JIEBA_PARAMETER_WRONG, "|DecodeUTF8RunesInString| DecodeUTF8ToRune len :%u, i: %u wrong", len, i);
            return JIEBA_PARAMETER_WRONG;
        }
        RuneStrT runeStr = {rp.rune, i, rp.len, j, 1};
        ErrorT ret = DynArrPushBack(tmpRunes, &runeStr);
        if (ret != JIEBA_OK) {
            LOG_ERROR(ret, "|DecodeUTF8RunesInString| DynArrPushBack wrong");
            return ret;
        }
        i += rp.len;
        ++j;
    }
    *runes = tmpRunes;
    return JIEBA_OK;
}
