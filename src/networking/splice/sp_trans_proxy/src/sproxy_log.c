/*Sytem Headers*/
#include<syslog.h>
#include<stdarg.h>
#include<stdio.h>
/*Local Headers*/
#include<sproxy_log.h>

int open_system_logger(char *log_ident, int log_level)
{
    int log_options = 0, log_facility = 0;
    
    log_options = LOG_PID | LOG_NDELAY;
    log_facility = LOG_LOCAL1;
    openlog(log_ident, log_options, log_facility);
    setlogmask(LOG_UPTO(log_level));
    return 0;
}

int write_log(pthread_t thread_id, char *file_name, int line_number, const char *function_name, int log_priority, char *log_format, ...)
{
  va_list log_ap;
  char format[2048] = {0,};
  va_start(log_ap, log_format);
  snprintf(format, sizeof(format), "%lu %s:%d %s > %s", thread_id, file_name, line_number, function_name, log_format);
  vsyslog(log_priority, format, log_ap);
  va_end(log_ap);   
  return 0; 
}

int close_system_logger()
{
    closelog();
    return 0;
}
