#pragma once
#include <format>
#include <iostream>
#include <string>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
};


void Log(const char* module, LogLevel level, std::string_view text)
{
    const char* levelStr = "";
    switch (level)
    {
    case LogLevel::INFO:    levelStr = "INFO"; break;
    case LogLevel::WARNING: levelStr = "WARN"; break;
    case LogLevel::ERROR:   levelStr = "ERROR"; break;
    }

    std::cout << "[" << module << "] [" << levelStr << "] " << text << std::endl;
}


template<typename... Args>
void Log(const char* module, LogLevel level, std::string_view format, Args&&... args)
{
    std::string text = std::vformat(format, std::make_format_args(args...));
    Log(module, level, text);
}

