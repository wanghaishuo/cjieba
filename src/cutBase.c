#include "cutBase.h"
#include "log.h"
#include <ctype.h>
#include <math.h>

#define BASE_WORD_LEN 2

// dag为uint32_t的一维数组, dag[i] 表示第i个字能组成的最长词的长度
// "清华大学" dag {{4}，{2} ,{2},{1}}
// 不直接生成可成词的dag是因为这样会在hashmap中寻找两遍，现在只需寻找一遍，同时一维数组即可，无需二维
ErrorT GenDag(PrefixDictT *dict, SentToCutT sentence, DagT *outDag) {
    RuneStrArrT runes = sentence.runes;
    uint32_t begin = sentence.begin;
    uint32_t end = sentence.end;
    const void *buf = sentence.buf.buf;
    DagT dag = CreateDynArr(sizeof(uint32_t), end - begin);
    if (dag == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|Gen Dag| Create base DynArr wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    for (uint32_t i = begin; i < end; ++i) {
        uint32_t offset = RunesItem(runes, i)->offset;
        uint32_t len = RunesItem(runes, i)->len;
        uint32_t wordMaxLen = 1;
        for (; i + wordMaxLen < end; ++wordMaxLen) {
            len += RunesItem(runes, i + wordMaxLen)->len;
            ConstBufT key = {buf + offset, len};
            if (!BloomFilterContain(dict->filter, key)) {
                break;
            }
        }
        DynArrPushBack(dag, &wordMaxLen);
    }
    // PrintIntArr(dag);
    *outDag = dag;
    return JIEBA_OK;
}

void FreeDag(DagT dag) {
    DestroyDynArr(dag);
}

// 获取词频的对数（未找到返回1，对应的概率为0）
double GetFreq(PrefixDictT *dict, ConstBufT key) {
    WordInfoT info;
    if (GetWordInfo(dict, key, &info)) {
        return log(info.freq);
    }
    return 1.0;
}

typedef struct RouteNode {
    double score;     // 当前路径得分（对数概率）
    uint32_t cutLen;  // 最佳切分长度, unicode长度
    uint32_t wordLen; // 词在字符串中的长度
    uint32_t offset;  // 词在字符串中的偏移
    double freq;
} RouteNodeT;

typedef DynArrT *RouteNodeArrT;

static inline RouteNodeT *RouteNode(RouteNodeArrT arr, uint32_t index) {
    return (RouteNodeT *)DynArrItem(arr, index);
}

// 对于“我喜欢夏天”，返回的数组为[1,2,1,2,1,0]
ErrorT CalcRouteNodeArr(PrefixDictT *dict, SentToCutT sentence, DagT dag, RouteNodeArrT *outArr) {
    RuneStrArrT runes = sentence.runes;
    uint32_t begin = sentence.begin;
    uint32_t end = sentence.end;
    uint32_t unicodeLen = end - begin;
    const void *buf = sentence.buf.buf;
    double totalFreq = dict->totalFreq;
    RouteNodeArrT arr = CreateDynArr(sizeof(RouteNodeT), unicodeLen + 1);
    if (arr == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|CalcRoute NodeArr| Create DynArr wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    (void)DynArrResize(arr, unicodeLen + 1);
    for (int i = unicodeLen - 1; i >= 0; --i) {
        uint32_t wordMaxLen = DagItem(dag, i);
        uint32_t cutLen = 0;
        double score = -INFINITY;
        uint32_t len = 0;
        uint32_t wordLen = 0;
        uint32_t offset = RunesItem(runes, begin + i)->offset;
        for (uint32_t j = 0; j < wordMaxLen; ++j) {
            len += RunesItem(runes, begin + i + j)->len;
            ConstBufT key = {.buf = buf + offset, .bufLen = len};
            double tmpScore = GetFreq(dict, key) - totalFreq + RouteNode(arr, i + j + 1)->score;
            if (tmpScore > score) {
                score = tmpScore;
                cutLen = j;
                wordLen = len;
            }
        }
        RouteNode(arr, i)->cutLen = cutLen + 1;
        RouteNode(arr, i)->score = score;
        RouteNode(arr, i)->wordLen = wordLen;
        RouteNode(arr, i)->offset = offset;
    }
    *outArr = arr;
    return JIEBA_OK;
}

// 后续需掉用DestroyDynArr(separatorPos)
static ErrorT GetSentSeparator(RuneT *separators, RuneStrArrT runes, PosArrT *separatorPos) {
    PosArrT posArr = CreateDynArr(sizeof(uint32_t), 0);
    if (posArr == NULL) {
        LOG_ERROR(JIEBA_MEMORY_OP_WRONG, "|Get Sent Separator| Create DynArr wrong");
        return JIEBA_MEMORY_OP_WRONG;
    }
    ErrorT ret = JIEBA_OK;
    uint32_t runesSize = DynArrSize(runes);
    for (uint32_t i = 0; i < runesSize; ++i) {
        if (IsSeparator(separators, RunesItem(runes, i)->rune)) {
            ret = DynArrPushBack(posArr, &i);
            if (ret != JIEBA_OK) {
                LOG_ERROR(ret, "|GetSentSeparator| DynArrPushBack wrong");
                DestroyDynArr(posArr);
                return ret;
            }
        }
    }
    // 将最后一位也放入分隔符数组，作为右边界
    // ret = DynArrPushBack(posArr, &runesSize);
    // if (ret != JIEBA_OK) {
    //     LOG_ERROR(ret, "|GetSentSeparator| DynArrPushBack wrong");
    //     DestroyDynArr(posArr);
    //     return ret;
    // }
    *separatorPos = posArr;
    return ret;
}

typedef struct CutPrepare {
    RuneStrArrT runes;
    PosArrT separatorPos;
    WordOutArrT wordOutArr;
} CutPrepareT;

static ErrorT BaseCutPrepare(PrefixDictT *dict, ConstBufT buf, CutPrepareT *cutPrepare) {
    RuneStrArrT runes;
    ErrorT ret = DecodeUTF8RunesInString(buf.buf, buf.bufLen, &runes);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|BaseCutPrepare| DecodeUTF8RunesInString wrong");
        return ret;
    }
    PosArrT separatorPos;
    ret = GetSentSeparator(dict->separator, runes, &separatorPos);
    if (ret != JIEBA_OK) {
        DestroyDynArr(runes);
        LOG_ERROR(ret, "|BaseCutPrepare| GetSentSeparator wrong");
        return ret;
    }
    WordOutArrT wordOutArr = CreateDynArr(sizeof(WordOutT), 0);
    if (ret != JIEBA_OK) {
        DestroyDynArr(runes);
        DestroyDynArr(separatorPos);
        LOG_ERROR(ret, "|BaseCutPrepare| cutLen wrong");
        return ret;
    }
    cutPrepare->runes = runes;
    cutPrepare->separatorPos = separatorPos;
    cutPrepare->wordOutArr = wordOutArr;
    return JIEBA_OK;
}

static inline void ClearCutPrepare(CutPrepareT cutPrepare, bool keepwordOutArr) {
    DestroyDynArr(cutPrepare.runes);
    DestroyDynArr(cutPrepare.separatorPos);
    if (!keepwordOutArr) {
        DestroyDynArr(cutPrepare.wordOutArr);
    }
}

// 将小数点、字母、数字分一起
static inline bool AsciiRule(char c) {
    return isalnum(c) || c == '.';
}

static ErrorT CutByProbability(PrefixDictT *dict, SentToCutT sentence, WordOutArrT wordOutArr) {
    if (sentence.begin == sentence.end) { // 说明在两个分隔符中间
        return JIEBA_OK;
    }
    DagT dag;
    ErrorT ret = GenDag(dict, sentence, &dag);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|CutBy Probability| Gen Dag wrong");
        return ret;
    }
    RouteNodeArrT nodeArr;
    ret = CalcRouteNodeArr(dict, sentence, dag, &nodeArr);
    FreeDag(dag);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|CutBy Probability| CalcRoute NodeArr wrong");
        return ret;
    }
    uint32_t nodeArrSize = DynArrSize(nodeArr) - 1; // 去除末尾的无效位
    for (uint32_t i = 0; i < nodeArrSize;) {
        uint32_t right = RunesItem(sentence.runes, i)->offset;
        RouteNodeT *node = RouteNode(nodeArr, i);
        WordOutT word = {.offset = node->offset, .len = node->wordLen};
        // 数字或字母开头
        if (node->wordLen != 1 || !isalnum(((const char *)sentence.buf.buf)[word.offset])) {
            i += node->cutLen; // unicode的偏移
        } else {
            uint32_t wordLen = node->wordLen;
            i++;
            while (i < nodeArrSize) {
                node = RouteNode(nodeArr, i);
                if (node->wordLen != 1 || !AsciiRule(((const char *)sentence.buf.buf)[word.offset])) {
                    break;
                }
                wordLen++;
                assert(node->cutLen == 1);
                i += node->cutLen; // unicode的偏移
            }
            word.len = wordLen;
        }
        ret = DynArrPushBack(wordOutArr, &word);
        if (ret != JIEBA_OK) {
            LOG_ERROR(ret, "|CutBy Probability| DynArr PushBack wrong");
            goto EXIT;
        }
    }
EXIT:
    DestroyDynArr(nodeArr);
    return ret;
}

static inline WordOutT SeparatorWord(RuneStrArrT runes, uint32_t index) {
    RuneStrT *item = RunesItem(runes, index);
    return (WordOutT){.offset = item->offset, .len = item->len};
}

static ErrorT CutAll(PrefixDictT *dict, SentToCutT sentence, WordOutArrT wordOutArr) {
    if (sentence.begin == sentence.end) { // 说明在两个分隔符中间
        return JIEBA_OK;
    }
    DagT dag;
    ErrorT ret = GenDag(dict, sentence, &dag);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|Cut all| GenDag wrong");
        return ret;
    }
    RuneStrArrT runes = sentence.runes;
    uint32_t begin = sentence.begin;
    uint32_t dagSize = DynArrSize(dag);
    const void *buf = sentence.buf.buf;
    uint32_t endOffset = 0;
    for (uint32_t i = 0; i < dagSize; ++i) {
        uint32_t wordMaxLen = DagItem(dag, i);
        uint32_t len = RunesItem(runes, begin + i)->len;
        uint32_t offset = RunesItem(runes, begin + i)->offset;
        if (wordMaxLen == 1 && offset >= endOffset) {
            WordOutT word = {.offset = offset, .len = len};
            ret = DynArrPushBack(wordOutArr, &word);
            if (ret != JIEBA_OK) {
                LOG_ERROR(ret, "|Cut All| Dyn Arr Push Back wrong");
                goto EXIT;
            }
            endOffset = offset + len + 1;
            continue;
        }
        for (uint32_t j = 1; j < wordMaxLen; ++j) {
            len += RunesItem(runes, begin + i + j)->len;
            ConstBufT key = {.buf = buf + offset, .bufLen = len};
            if (WordInDict(dict, key)) {
                WordOutT word = {.offset = offset, .len = len};
                ret = DynArrPushBack(wordOutArr, &word);
                if (ret != JIEBA_OK) {
                    LOG_ERROR(ret, "|Cut All| Dyn Arr Push Back wrong");
                    goto EXIT;
                }
                endOffset = offset + len + 1;
            }
        }
    }
EXIT:
    FreeDag(dag);
    return ret;
}

static ErrorT CutSegment(PrefixDictT *dict, CutTypeT type, SentToCutT sentence, WordOutArrT wordOutArr) {
    if (type == CUT_BY_PROBABILITY) {
        return CutByProbability(dict, sentence, wordOutArr);
    }
    if (type == CUT_ALL) {
        return CutAll(dict, sentence, wordOutArr);
    }
    return JIEBA_CONFIG_WRONG;
}

// 成功后需调用DestroyDynArr释放资源
ErrorT BaseCut(PrefixDictT *dict, CutTypeT type, ConstBufT buf, WordOutArrT *arr) {
    CutPrepareT cutPrepare;
    ErrorT ret = BaseCutPrepare(dict, buf, &cutPrepare);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|Base Cut| Base Cut Prepare wrong");
        return ret;
    }
    PosArrT separatorPos = cutPrepare.separatorPos;
    WordOutArrT wordOutArr = cutPrepare.wordOutArr;
    RuneStrArrT runes = cutPrepare.runes;
    SentToCutT sentence = {.buf = buf, .runes = runes};
    uint32_t begin = 0;
    for (uint32_t i = 0; i < DynArrSize(separatorPos); ++i) {
        sentence.begin = begin;
        sentence.end = PosArrItem(separatorPos, i);
        ret = CutSegment(dict, type, sentence, wordOutArr);
        if (ret != JIEBA_OK) {
            LOG_ERROR(ret, "|Base Cut| Cut Segment wrong");
            goto EXIT;
        }
        WordOutT word = SeparatorWord(runes, sentence.end); // 分隔符
        ret = DynArrPushBack(wordOutArr, &word);
        if (ret != JIEBA_OK) {
            LOG_ERROR(ret, "|Base Cut| DynArr PushBack wrong");
            goto EXIT;
        }
        begin = sentence.end + 1;
    }
    // 插入最后一个分隔符之后的句子
    sentence.begin = begin;
    sentence.end = DynArrSize(runes);
    ret = CutSegment(dict, type, sentence, wordOutArr);
    if (ret != JIEBA_OK) {
        LOG_ERROR(ret, "|Base Cut| last Cut Segment wrong");
        goto EXIT;
    }
    *arr = wordOutArr;
EXIT:
    ClearCutPrepare(cutPrepare, ret == JIEBA_OK);
    return ret;
}
