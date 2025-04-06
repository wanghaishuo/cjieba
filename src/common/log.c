#include "log.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE *g_logFile = NULL;
static LogLevelT g_currentLevel = LOG_INFO;
static pthread_mutex_t g_logMutex = PTHREAD_MUTEX_INITIALIZER;

#include <errno.h>
#include <stdio.h>
#include <time.h>

uint64_t GetTimeStamp() {
    struct timespec ts;

    // 使用单调递增时钟
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        perror("clock_gettime failed");
        return 0;
    }

    // 计算纳秒总数（适合作为顺序标识）
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

ErrorT LogInit(const char *fileName, LogLevelT level) {
    pthread_mutex_lock(&g_logMutex);
    if (g_logFile != NULL) {
        fclose(g_logFile);
        g_logFile = NULL;
    }
    if (fileName == NULL) {
        fileName = "./run.log";
    }

    g_logFile = fopen(fileName, "a");
    if (g_logFile == NULL) {
        pthread_mutex_unlock(&g_logMutex);
        perror("Failed to open log file");
        return JIEBA_FILE_OP_WRONG;
    }
    g_currentLevel = level;
    pthread_mutex_unlock(&g_logMutex);
    return JIEBA_OK;
}

void LogClose(void) {
    pthread_mutex_lock(&g_logMutex);
    if (g_logFile != NULL) {
        fclose(g_logFile);
        g_logFile = NULL;
    }
    pthread_mutex_unlock(&g_logMutex);
}

void logMessage(LogLevelT level, ErrorT err, const char *fmt, ...) {
    if (level < g_currentLevel || g_logFile == NULL) {
        return;
    }

    const char *level_str = "";
    switch (level) {
    case LOG_DEBUG:
        level_str = "DEBUG";
        break;
    case LOG_INFO:
        level_str = "INFO";
        break;
    case LOG_WARNING:
        level_str = "WARNING";
        break;
    case LOG_ERROR:
        level_str = "ERROR";
        break;
    default:
        level_str = "UNKNOWN";
        break;
    }

    pthread_mutex_lock(&g_logMutex);

    // 输出日志头信息
    fprintf(g_logFile, "[%lu] [%s] [%d] - ", GetTimeStamp(), level_str, err);

    // 输出用户消息
    va_list args;
    va_start(args, fmt);
    vfprintf(g_logFile, fmt, args);
    va_end(args);

    fprintf(g_logFile, "\n");
    // fflush(g_logFile); // 确保立即刷新缓冲区

    pthread_mutex_unlock(&g_logMutex);
}
