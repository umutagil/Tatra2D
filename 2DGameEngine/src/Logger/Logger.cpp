#include "Logger.h"
#include <iostream>
#include <chrono>

#include "date.h"

constexpr std::string_view RED = "\033[31m"			/* Red */;
constexpr std::string_view GREEN = "\033[32m"		/* Green */;
constexpr std::string_view RESET = "\033[0m";

std::vector<LogEntry> Logger::messages;

std::string GetCurrentDateTimeString()
{
	return date::format("%d/%b/%Y %X", std::chrono::system_clock::now());
}

void Logger::Log(const std::string &message)
{
	LogEntry logEntry;
	logEntry.type = LogType::LOG_info;
	logEntry.message = "LOG: [" + GetCurrentDateTimeString() + " ]: " + message;
	messages.push_back(logEntry);

	std::cout << GREEN << logEntry.message << RESET << '\n';
}

void Logger::Err(const std::string &message)
{
	LogEntry logEntry;
	logEntry.type = LogType::LOG_info;
	logEntry.message = "LOG: [" + GetCurrentDateTimeString() + " ]: " + message;
	messages.push_back(logEntry);

	std::cerr << RED << logEntry.message << RESET << '\n';
}
