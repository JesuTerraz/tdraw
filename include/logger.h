#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
} LoggerLevel;

typedef struct logger {
    FILE *file;
    LoggerLevel level;
} Logger;

int init_logger();

void debug(char *format, ...);
void info(char *format, ...);
void warn(char *format, ...);
void error(char *format, ...);

#endif