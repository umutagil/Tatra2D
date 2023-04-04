#pragma once

#include <string>
#include <vector>

enum class LogType
{
	LOG_info,
	LOG_warning,
	LOG_error,
};

struct LogEntry
{
	LogType type;
	std::string message;
};

class Logger
{
public:
	static void Log(const std::string &message);
	static void Err(const std::string &message);

private:
	static std::vector<LogEntry> messages;

};

