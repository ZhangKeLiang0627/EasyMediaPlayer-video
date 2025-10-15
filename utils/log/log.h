#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define log_debug(...) spdlog_debug(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_info(...) spdlog_info(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_warn(...) spdlog_warn(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_error(...) spdlog_error(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

    void spdlog_debug(const char *file, int line, const char *func, const char *fmt, ...);
    void spdlog_info(const char *file, int line, const char *func, const char *fmt, ...);
    void spdlog_warn(const char *file, int line, const char *func, const char *fmt, ...);
    void spdlog_error(const char *file, int line, const char *func, const char *fmt, ...);
    void logger_init(const char *log_dir, const char *log_file, int daily, size_t max_size, int max_files);
    void logger_shutdown();

#ifdef __cplusplus
}
#endif

#endif
