#pragma once
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <deque>


enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
};

class LogManager
{
public:
    static LogManager& Get()
    {
        static LogManager instance;
        return  instance;
    }
    ~LogManager() = default;
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    bool GetALog(std::string& logOut)
    {
        if(logBuffer.empty())
            return false;

        logOut = logBuffer.front();
        logBuffer.pop_front();
        return true;
    }
    void addLog(const std::string& module, LogLevel level, const std::string& text)
    {
        const char* levelStr = "";
        switch (level)
        {
        case LogLevel::INFO:    levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARN"; break;
        case LogLevel::ERROR:   levelStr = "ERROR"; break;
        }
        // 定宽：模块名与级别列对齐，正文从同一列开始
        constexpr int kModuleW = 14;
        constexpr int kLevelW = 5;
        std::string logStr = std::format("[{:<{}}] [{:<{}}] {}\n",
            module, kModuleW, levelStr, kLevelW, text);
        logBuffer.push_back(std::move(logStr));
    }
private:
    LogManager() = default;
    std::deque<std::string> logBuffer;
};

template<typename... Args>
void Log(const char* module, LogLevel level, std::string_view format, Args&&... args)
{
    std::string text = std::vformat(format, std::make_format_args(args...));
    LogManager::Get().addLog(module, level, text);
}

