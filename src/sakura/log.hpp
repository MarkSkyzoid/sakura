#pragma once

#include "platform.hpp"

#include <stdio.h>

#if !defined(NDEBUG)
#define SAKURA_ASSERT_ENABLED
#endif

#ifdef SAKURA_ASSERT_ENABLED
#define SKR_ASSERT_M(cond, msg, ...) \
if(cond) {} \
else { \
	sakura::logging::report_assert(__FILE__, __LINE__, msg, __VA_ARGS__); \
	platform::debug_break(); \
}
#define SKR_ASSERT(cond) \
if(cond) {} \
else { \
	sakura::logging::report_assert(__FILE__, __LINE__, #cond); \
	platform::debug_break(); \
}
#define SKR_ASSERT_FAST(cond) if (!(cond)) platform::debug_break();
#else // SAKURA_ASSERT_ENABLED
#define SKR_ASSERT_M(cond, msg, ...)
#define SKR_ASSERT(cond)
#define SKR_ASSERT_FAST(cond)
#endif // SAKURA_ASSERT_ENABLED

namespace sakura {
	namespace logging {
		
		namespace detail {
			template <typename ... Args>
			void log(platform::Verbosity verbosity, const char* msg, Args... args) {
				char buf[256];
				sprintf_s(buf, 256, msg, args...);
				platform::log(verbosity, buf);
			}

			void log(platform::Verbosity verbosity, const char* msg) {
				platform::log(verbosity, msg);
			}
		}

		template <typename ... Args>
		void log(const char* msg, Args... args) {
			detail::log(platform::Verbosity::Info, msg, args...);
		}

		template <typename ... Args>
		void log_warning(const char* msg, Args... args) {
			detail::log(platform::Verbosity::Warning, msg, args...);
		}

		template <typename ... Args>
		void log_critical(const char* msg, Args... args) {
			detail::log(platform::Verbosity::Critical, msg, args...);
		}

		template <typename ... Args>
		void report_assert(const char* file, i32 line, const char* msg, Args... args) {
			char buf[256];
			sprintf_s(buf, 256, msg, args...);
			detail::log(platform::Verbosity::Critical, "ASSERTION AT \n%s(%d): %s", file, line, buf);
		}
	}
}