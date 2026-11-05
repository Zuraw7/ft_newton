#ifndef COMMON_LOGGER_H
#define COMMON_LOGGER_H

#include <cstdio>
#include <memory>
#include <mutex>

namespace newton::common
{
	/**
	 * @brief Message severity, ordered from the most verbose.
	 */
	enum class LogLevel {
		DEBUG,
		INFO,
		WARN,
		ERROR,
		DISABLED
	};

	/**
	 * @brief Thread-safe printf-style logger with severity filtering.
	 *
	 * Each message gets a "[LEVEL] " prefix, the newline must be part of the message if needed.
	 * Use @ref getInstance() for the shared logger, or @ref createInstance() for a separate one.
	 * The default level is WARN, so DEBUG and INFO messages are hidden until @ref setLogLevel() is called.
	 */
	class Logger
	{
		public:
			~Logger() = default;

			static Logger &getInstance();
			static std::unique_ptr<Logger> createInstance(LogLevel level = LogLevel::WARN);

			void setLogLevel(LogLevel level);

			int printf(LogLevel level, const char *msg, ...) __attribute__((format(printf, 3, 4)));
			int fprintf(LogLevel level, FILE *stream, const char *msg, ...) __attribute__((format(printf, 4, 5)));

		private:
			explicit Logger(LogLevel level = LogLevel::WARN);

			Logger(const Logger &) = delete;
			Logger &operator=(const Logger &) = delete;

			int vfprintf(LogLevel level, FILE *stream, const char *msg, va_list args)
				__attribute__((format(printf, 4, 0)));
			const char *levelToString(LogLevel level);

			LogLevel m_level = LogLevel::WARN; ///< Messages below this level are skipped.
			std::mutex m_mutex;                ///< Guards @ref m_level and writes.
	};

} // namespace newton::common

#endif // COMMON_LOGGER_H
