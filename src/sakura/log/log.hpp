#pragma once
#include "../../ext/loguru/loguru.hpp"

#include "type_aliases.hpp"

#if !defined(NDEBUG)
#define SAKURA_ASSERT_ENABLED
#endif

#ifdef SAKURA_ASSERT_ENABLED
#define SKR_ASSERT_M(cond, msg, ...)                                                           \
	if (cond) {                                                                                 \
	} else {                                                                                    \
		char buf[sakura::logging::MESSAGE_SIZE];                                                 \
		sprintf_s(buf, sakura::logging::MESSAGE_SIZE, msg, __VA_ARGS__);                         \
		logging::log_error(msg, __VA_ARGS__);                                                    \
		platform::report_assert(__FILE__, __LINE__, buf, platform::AssertIgnoreMode::CanIgnore); \
	}
#define SKR_ASSERT(cond)                                                                         \
	if (cond) {                                                                                   \
	} else {                                                                                      \
		logging::log_error(#cond);                                                                 \
		platform::report_assert(__FILE__, __LINE__, #cond, platform::AssertIgnoreMode::CanIgnore); \
	}
#define SKR_ASSERT_FATAL_M(cond, msg, ...)                                                        \
	if (cond) {                                                                                    \
	} else {                                                                                       \
		char buf[sakura::logging::MESSAGE_SIZE];                                                    \
		sprintf_s(buf, sakura::logging::MESSAGE_SIZE, msg, __VA_ARGS__);                            \
		logging::log_error(msg, __VA_ARGS__);                                                       \
		platform::report_assert(__FILE__, __LINE__, buf, platform::AssertIgnoreMode::CannotIgnore); \
	}
#define SKR_ASSERT_FATAL(cond)                                                                      \
	if (cond) {                                                                                      \
	} else {                                                                                         \
		logging::log_error(#cond);                                                                    \
		platform::report_assert(__FILE__, __LINE__, #cond, platform::AssertIgnoreMode::CannotIgnore); \
	}
#define SKR_ASSERT_FAST(cond) \
	if (!(cond))               \
		platform::debug_break();
#else // SAKURA_ASSERT_ENABLED
#define SKR_ASSERT_FATAL_M(cond, msg, ...)
#define SKR_ASSERT_FATAL(cond)
#define SKR_ASSERT_M(cond, msg, ...)
#define SKR_ASSERT(cond)
#define SKR_ASSERT_FAST(cond)
#endif // SAKURA_ASSERT_ENABLED

namespace sakura {
	namespace logging {
		constexpr size_t MESSAGE_SIZE = 256;

		using Verbosity = loguru::Verbosity;
		using NamedVerbosity = loguru::NamedVerbosity;
		using Message = loguru::Message;
		using LogHandler = loguru::log_handler_t; // void (*LogHandler)(void* user_data, const Message& message);
		using FileMode = loguru::FileMode;

		bool add_file(const char* path, FileMode mode, Verbosity verbosity);
		void add_callback(const char* id, LogHandler callback, void* user_data, Verbosity verbosity);
		void suggest_log_path(const char* prefix, char* buff, unsigned buff_size);
		const char* get_verbosity_name(Verbosity verbosity);

		// Normal log functions
		template<typename... Args> void log_info(const char* fmt, Args... args)
		{
			LOG_F(INFO, fmt, args...);
		}

		template<typename... Args> void log_warning(const char* fmt, Args... args)
		{
			LOG_F(WARNING, fmt, args...);
		}

		template<typename... Args> void log_error(const char* fmt, Args... args)
		{
			loguru::stacktrace();
			LOG_F(ERROR, fmt, args...);
		}

		// Debug only log functions
		template<typename... Args> void dlog_info(const char* fmt, Args... args)
		{
			DLOG_F(INFO, fmt, args...);
		}

		template<typename... Args> void dlog_warning(const char* fmt, Args... args)
		{
			DLOG_F(WARNING, fmt, args...);
		}

		template<typename... Args> void dlog_error(const char* fmt, Args... args)
		{
			loguru::stacktrace();
			DLOG_F(ERROR, fmt, args...);
		}
	} // namespace logging
} // namespace sakura