#ifndef SPROXY_LOG_H
#define SPROXY_LOG_H
#include<syslog.h>
#include<pthread.h>

#define log_msg(level, fmt, args...)   \
     write_log( pthread_self(), __FILE__, __LINE__, __FUNCTION__, level, fmt, ##args);

int open_system_logger(char *log_ident, int log_level);
int write_log(pthread_t thread_id, char *file_name, int line_number, const char *function_name, int log_priority, char *log_format, ...);
int close_system_logger();
#endif
