#pragma once

#include <iostream>
#include <stdio.h>

#include "type_aliases.hpp"

#if !defined(NDEBUG)
#define SAKURA_ASSERT_ENABLED
#endif

#ifdef SAKURA_ASSERT_ENABLED
#define SKR_ASSERT_M(cond, msg, ...) \
if(cond) {} \
else { \
	char buf[sakura::logging::MESSAGE_SIZE]; \
	sprintf_s(buf, sakura::logging::MESSAGE_SIZE, msg, __VA_ARGS__); \
	platform::report_assert(__FILE__, __LINE__, buf, platform::AssertIgnoreMode::CanIgnore); \
}
#define SKR_ASSERT(cond) \
if(cond) {} \
else { \
	platform::report_assert(__FILE__, __LINE__, #cond, platform::AssertIgnoreMode::CanIgnore); \
}
#define SKR_ASSERT_FAST(cond) if (!(cond)) platform::debug_break();
#else // SAKURA_ASSERT_ENABLED
#define SKR_ASSERT_M(cond, msg, ...)
#define SKR_ASSERT(cond)
#define SKR_ASSERT_FAST(cond)
#endif // SAKURA_ASSERT_ENABLED

namespace sakura
{
	namespace logging 
	{
		constexpr size_t MESSAGE_SIZE = 256;

		enum class Verbosity : u8 {
			Info = 0,
			Warning,
			Critical,
			Count
		};

		namespace detail 
		{
			template <typename ... Args>
			void log(Verbosity verbosity, const char* msg, Args... args)
			{
				char buf[MESSAGE_SIZE];
				sprintf_s(buf, MESSAGE_SIZE, msg, args...);
				log(verbosity, buf);
			}

			void log(Verbosity verbosity, const char* msg);
		}

		template <typename ... Args>
		void log_info(const char* msg, Args... args)
		{
			detail::log(Verbosity::Info, msg, args...);
		}

		template <typename ... Args>
		void log_warning(const char* msg, Args... args)
		{
			detail::log(Verbosity::Warning, msg, args...);
		}

		template <typename ... Args>
		void log_critical(const char* msg, Args... args)
		{
			detail::log(Verbosity::Critical, msg, args...);
		}
	}
}