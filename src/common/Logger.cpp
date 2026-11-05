#include "common/Logger.hpp"

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <mutex>

namespace newton::common
{
	Logger::Logger(LogLevel level)
		: m_level(level)
	{}

	/**
	 * @brief Returns the shared logger, created on first use with the WARN level.
	 */
	Logger &Logger::getInstance()
	{
		static Logger instance;
		return instance;
	}

	/**
	 * @brief Creates a logger independent from the shared one.
	 *
	 * @param level Initial level.
	 */
	std::unique_ptr<Logger> Logger::createInstance(LogLevel level)
	{
		return std::unique_ptr<Logger>(new Logger(level));
	}

	void Logger::setLogLevel(LogLevel level)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_level = level;
	}

	/**
	 * @brief Logs a formatted message to stdout.
	 * @param level Message severity.
	 * @param msg printf-style format string.
	 * @return Number of characters written, 0 if the message was filtered out, negative on error.
	 */
	int Logger::printf(LogLevel level, const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

		int result = vfprintf(level, stdout, msg, args);

		va_end(args);

		return result;
	}

	/**
	 * @brief Logs a formatted message to the given stream.
	 * @param level Message severity.
	 * @param stream Output stream, e.g. stderr or an open file.
	 * @param msg printf-style format string.
	 * @return Number of characters written, 0 if the message was filtered out, negative on error.
	 */
	int Logger::fprintf(LogLevel level, FILE *stream, const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

		int result = vfprintf(level, stream, msg, args);

		va_end(args);

		return result;
	}

	/**
	 * @brief Filters by level and writes the prefix and the message under one lock.
	 */
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
