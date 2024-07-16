#pragma once
#include <string>
#include <mutex>

class Logger {
public:
	static Logger& GetInstance();

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	void Log(std::string_view message);


public:

private:
	std::mutex logMutex;
	Logger() = default;
	~Logger() = default;
};