#include <iostream>
#include <cstdarg>

#include "log.h"
#include "logger.h"

void logger_init(const char *log_dir, const char *log_file, int daily, size_t max_size, int max_files)
{
    Logger::init(log_dir, log_file, daily != 0, max_size, max_files);
}

void spdlog_debug(const char *file, int line, const char *func, const char *fmt, ...)
{
    auto logger = Logger::get();
    if (!logger)
        return;

    va_list args;
    va_start(args, fmt);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, args);
    logger->debug("[{}:{}:{}] {}", file, line, func, buf);
    va_end(args);
}

void spdlog_info(const char *file, int line, const char *func, const char *fmt, ...)
{

    auto logger = Logger::get();
    if (!logger)
        return;

    va_list args;
    va_start(args, fmt);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, args);
    logger->info("[{}:{}:{}] {}", file, line, func, buf);
    va_end(args);
}

void spdlog_warn(const char *file, int line, const char *func, const char *fmt, ...)
{

    auto logger = Logger::get();
    if (!logger)
        return;

    va_list args;
    va_start(args, fmt);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, args);
    logger->warn("[{}:{}:{}] {}", file, line, func, buf);
    va_end(args);
}

void spdlog_error(const char *file, int line, const char *func, const char *fmt, ...)
{

    auto logger = Logger::get();
    if (!logger)
        return;

    va_list args;
    va_start(args, fmt);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, args);
    logger->error("[{}:{}:{}] {}", file, line, func, buf);
    va_end(args);
}

void logger_shutdown()
{
    Logger::shutdown();
}