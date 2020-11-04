#include "log.hpp"
#include "../../ext/loguru/loguru.cpp"

namespace sakura::logging {
	bool add_file(const char* path, FileMode mode, Verbosity verbosity) 
	{
		return loguru::add_file(path, mode, verbosity);
	}

	void suggest_log_path(const char* prefix, char* buff, unsigned buff_size) 
	{
		loguru::suggest_log_path(prefix, buff, buff_size);
	}

	void add_callback(const char* id, LogHandler callback, void* user_data, Verbosity verbosity) 
	{
		loguru::add_callback(id, callback, user_data, verbosity);
	}

	const char* get_verbosity_name(Verbosity verbosity)
	{
		return loguru::get_verbosity_name(verbosity);
	}
} // namespace sakura::logging