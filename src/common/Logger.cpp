#include "../include/common/Logger.hpp"

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <mutex>

namespace newton::common
{
	Logger::Logger(LogLevel level)
		: m_level(level)
	{}

	Logger &Logger::getInstance()
	{
		static Logger instance;
		return instance;
	}

	std::unique_ptr<Logger> Logger::createInstance(LogLevel level)
	{
		return std::unique_ptr<Logger>(new Logger(level));
	}

	void Logger::setLogLevel(LogLevel level)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_level = level;
	}

	int Logger::printf(LogLevel level, const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

		int result = vfprintf(level, stdout, msg, args);

		va_end(args);

		return result;
	}

	int Logger::fprintf(LogLevel level, FILE *stream, const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

		int result = vfprintf(level, stream, msg, args);

		va_end(args);

		return result;
	}

	int Logger::vfprintf(LogLevel level, FILE *stream, const char *msg, va_list args)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (level == LogLevel::DISABLED || level < m_level) {
			return 0;
		}

		int result = 0;

		if ((result = ::fprintf(stream, "[%s] ", levelToString(level))) < 0)
			return result;

		int msgResult = 0;

		if ((msgResult = ::vfprintf(stream, msg, args)) < 0)
			return msgResult;

		return result + msgResult;
	}

	const char *Logger::levelToString(LogLevel level)
	{
		switch (level) {
			case LogLevel::DEBUG : return "DEBUG";
			case LogLevel::INFO : return "INFO";
			case LogLevel::WARN : return "WARN";
			case LogLevel::ERROR : return "ERROR";
			case LogLevel::DISABLED : return "DISABLED";
			default : return "UNKNOWN";
		}
	}

} // namespace newton::common
