#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <string>
#include <memory>

#include "spdlog/spdlog.h"

class Logger
{
public:
    static void init(const std::string logDir, const std::string logName, bool isLograteDay, size_t maxSize, int logFileNum);

    static std::shared_ptr<spdlog::logger> get();

    static void shutdown();

protected:
private:
    Logger() = default;
    static std::shared_ptr<spdlog::logger> g_logger;
};