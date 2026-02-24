#ifndef BASE_TYPE_H
#define BASE_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CutType {
    CUT_BY_PROBABILITY = 0, // 基于概率分词
    CUT_ALL,
    CUT_TYPE_END
} CutTypeT;

#ifdef __cplusplus
}
#endif

#endif // BASE_TYPE_H
