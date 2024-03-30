#include <stdarg.h>

#include "logging.h"

void log_info(const char* log, ...)
{
	va_list args;
	va_start(args, log);

	printf(log, args);

	va_end(args);
}

void log_debug(const char* log, ...)
{
	va_list args;
	va_start(args, log);

	printf(log, args);

	va_end(args);
}

void log_warning(const char* log, ...)
{
	va_list args;
	va_start(args, log);

	printf(log, args);

	va_end(args);
}

void log_error(const char* log, ...)
{
	va_list args;
	va_start(args, log);

	printf(log, args);

	va_end(args);
}