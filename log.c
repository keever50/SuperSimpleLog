/**
 * @file log.c
 * @author Kevin Witteveen (kevinwit1999@gmail.com)
 * @brief A super simple logging output
 * @version 1
 * @date 2024-12-04
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

#include "log.h"

enum log_level _log_current_level = LOG_DEFAULT_LEVEL;

#ifndef LOG_NO_FREERTOS
#include <semphr.h>
SemaphoreHandle_t _log_semaphore;
char _log_semaphore_created=0;
#define _LOG_SM_TAKE xSemaphoreTake(_log_semaphore, portMAX_DELAY)
#define _LOG_SM_GIVE xSemaphoreGive(_log_semaphore)
#else 
#define _LOG_SM_TAKE
#define _LOG_SM_GIVE
#endif


void _log_debug(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[32m[DEBUG]\033[0m %s:%d(%s): %s\r\n", file, line, func, msg);
  log_out(location_msg);
}

void _log_info(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "[INFO] %s: %s\r\n", func, msg);
  log_out(location_msg);
}

void _log_notice(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[34m[NOTICE]\033[0m %s: %s\r\n", func, msg);
  log_out(location_msg);
}

void _log_warning(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[33m[WARNING]\033[0m %s: %s\r\n", func, msg);
  log_out(location_msg);
}

void _log_error(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[31m[ERROR]\033[0m %s:%d(%s): %s\r\n", file, line, func, msg);
  log_out(location_msg);
}

void _log_critical(const char* msg, const char *file, const char *func, int line)
{
  char location_msg[0xFF];
  snprintf(location_msg, sizeof(location_msg), "\033[31m\033[5m[CRITICAL]\033[0m %s:%d(%s): %s\r\n", file, line, func, msg);
  log_out(location_msg);
}

void log_setlevel(enum log_level lvl)
{
  _log_current_level = lvl;
}

void _log(enum log_level lvl, const char *msg, const char *file, const char *func, int line, ...)
{
  if(lvl<_log_current_level)
    return;

  #ifndef LOG_NO_FREERTOS
  if(!_log_semaphore_created)
  {
    _log_semaphore = xSemaphoreCreateMutex();
    _log_semaphore_created=1;
  }
  #endif

  _LOG_SM_TAKE;

  char msgbuff[LOG_MAX_MSG_SIZE];

  va_list vargs;
  va_start(vargs, line);
  vsnprintf(msgbuff, LOG_MAX_MSG_SIZE, msg, vargs);
  va_end(vargs);

  switch(lvl)
  {
    case LOG_DEBUG:
      _log_debug(msgbuff, file, func, line);
      break;
    case LOG_INFO:
      _log_info(msgbuff, file, func, line);
      break;
    case LOG_NOTICE:
      _log_notice(msgbuff, file, func, line);
      break;
    case LOG_WARNING:
      _log_warning(msgbuff, file, func, line);
      break;
    case LOG_ERROR:
      _log_error(msgbuff, file, func, line);
      break;
    case LOG_CRITICAL:
      _log_critical(msgbuff, file, func, line);
      break;
  }

  _LOG_SM_GIVE;

  return;
}
