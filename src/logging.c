#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>

#include "logging.h"

#ifdef _WIN32
#define COLOR_WHITE 0xF
#endif

void log_info(const char* log, ...)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
#else
	printf("\x1B[34m");
#endif

	va_list args;
	va_start(args, log);

	printf("INFO: ");
	vprintf(log, args);

	va_end(args);

#ifdef _WIN32
	SetConsoleTextAttribute(hConsole, COLOR_WHITE);
#else
	printf("\x1B[0m");
#endif
}

void log_debug(const char* log, ...)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
#else
	printf("\x1B[32m");
#endif

	va_list args;
	va_start(args, log);

	printf("DEBUG: ");
	vprintf(log, args);

	va_end(args);

#ifdef _WIN32
	SetConsoleTextAttribute(hConsole, COLOR_WHITE);
#else
	printf("\x1B[0m");
#endif
}

void log_warning(const char* log, ...)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
#else
	printf("\x1B[33m");
#endif

	va_list args;
	va_start(args, log);

	printf("WARNING: ");
	vprintf(log, args);

	va_end(args);

#ifdef _WIN32
	SetConsoleTextAttribute(hConsole, COLOR_WHITE);
#else
	printf("\x1B[0m");
#endif
}

void log_error(const char* log, ...)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
#else
	printf("\x1B[31m");
#endif

	va_list args;
	va_start(args, log);

	printf("ERROR: ");
	vprintf(log, args);

	va_end(args);

#ifdef _WIN32
	SetConsoleTextAttribute(hConsole, COLOR_WHITE);
#else
	printf("\x1B[0m");
#endif
}