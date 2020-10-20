#include "log.hpp"

void sakura::logging::detail::log(Verbosity verbosity, const char* msg)
{
	std::cerr << msg << "\n";
}
