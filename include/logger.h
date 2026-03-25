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
    char *logpath;
} Logger;



Logger *init_logger(char *module);
int close_logger(Logger *logger);
int set_level(Logger *logger, LoggerLevel level);

void debug(Logger *logger, char *buf);
void info(Logger *logger, char *buf);
void warn(Logger *logger, char *buf);
void error(Logger *logger, char *buf);

#endif