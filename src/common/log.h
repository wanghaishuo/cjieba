#ifndef LOG_H
#define LOG_H

#include "base.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_END,
} LogLevelT;

// 初始化日志系统，fileName为NULL时输出到stderr
ErrorT LogInit(const char *fileName, LogLevelT level);
// 关闭日志系统
void LogClose(void);
// 记录日志消息
void logMessage(LogLevelT level, ErrorT err, const char *fmt, ...);

// 根据是否启用调试模式定义宏
#ifndef NDEBUG
#define LOG_DEBUG(err, fmt, ...) logMessage(LOG_DEBUG, err, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(err, fmt, ...)
#endif

#define LOG_INFO(err, fmt, ...) logMessage(LOG_INFO, err, fmt, ##__VA_ARGS__)
#define LOG_WARNING(err, fmt, ...) logMessage(LOG_WARNING, err, fmt, ##__VA_ARGS__)
#define LOG_ERROR(err, fmt, ...) logMessage(LOG_ERROR, err, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // LOG_H