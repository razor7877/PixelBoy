#ifndef LOGGING_H
#define LOGGING_H

void log_info(const char* log, ...);
void log_debug(const char* log, ...);
void log_warning(const char* log, ...);
void log_error(const char* log, ...);

#endif