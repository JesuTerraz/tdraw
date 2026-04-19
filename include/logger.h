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

void debug(char *buf);
void info(char *buf);
void warn(char *buf);
void error(char *buf);

#endif