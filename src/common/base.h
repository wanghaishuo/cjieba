#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ConstBuf {
    const void *buf;
    uint32_t bufLen;
} ConstBufT;

#ifndef NDEBUG
#define JIEBA_ASSERT(eq) assert(eq)
#else
#define JIEBA_ASSERT(eq)
#endif

#define JIEBA_UNUSED(a)

typedef enum Error {
    JIEBA_OK = 0,
    JIEBA_FILE_OP_WRONG = 100,
    JIEBA_MEMORY_OP_WRONG = 200,
    JIEBA_CONFIG_WRONG = 300,
    JIEBA_PARAMETER_WRONG = 400,
} ErrorT;

#ifdef __cplusplus
}
#endif

#endif // BASE_H