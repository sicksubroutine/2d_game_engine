#include "logger.h"
#include <iostream>
#include <ctime>
#include <string>

std::vector<LogEntry> Logger::logs;

std::string Current_DateTime_to_String() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char time_str[50];
    strftime(time_str, sizeof(time_str), "%b-%d-%Y %I:%M:%S %p", ltm);
    std::string time_str_formatted(time_str);
    return time_str_formatted;
}

void Logger::Log(const std::string& message) {
    LogEntry log_entry;
    log_entry.type = LOG_INFO;
    log_entry.message = "LOG: [" + Current_DateTime_to_String() + "] - " + message;
    std::cout << "\033[1;32m" << log_entry.message << "\033[0m" << std::endl;
    logs.push_back(log_entry);
}

void Logger::Warn(const std::string& message) {
    LogEntry log_entry;
    log_entry.type = LOG_WARNING;
    log_entry.message = "WARN: [" + Current_DateTime_to_String() + "] - " + message;
    std::cout << "\033[1;33m" << log_entry.message << "\033[0m" << std::endl;
    logs.push_back(log_entry);
}

void Logger::Err(const std::string& message) {
    LogEntry log_entry;
    log_entry.type = LOG_ERROR;
    log_entry.message = "ERR: [" + Current_DateTime_to_String() + "] - " + message;
    std::cout << "\033[1;31m" << log_entry.message << "\033[0m" << std::endl;
    logs.push_back(log_entry);
}