#include "Logger.h"
#include <iostream>
#include "Timer.h"

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

void Logger::Log(std::string_view message) {
    std::cout << message << std::endl;
}