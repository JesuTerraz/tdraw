#include "logger.h"

#include <stdlib.h>
#include <string.h>

static char *logpath = "./logs/";

Logger *
init_logger(char *module)
{
    Logger *log = malloc(sizeof(Logger));
    int pathlen = strlen(logpath) + strlen(module) + 5;
    log->logpath = malloc(pathlen);

    sprintf(log->logpath, "%s/%s.log", logpath, module);

    FILE *logfile = fopen(log->logpath, "w");
    if (!logfile) {
        free(log);
        return (NULL);
    }

    log->file = logfile;
    log->level = LOG_DEBUG;
    return (log);
}

int
close_logger(Logger *logger)
{
    if (!logger)
        return (-1);

    if (!logger->file || !logger->logpath)
        return (-1);

    if (fclose(logger->file))
        return (-1);

    free(logger->logpath);
    free(logger);

    return (0);
}

int
set_level(Logger *logger, LoggerLevel level)
{
    if (!logger)
        return (-1);

    logger->level = level;
    return (0);
}

void
flog(Logger *logger, char *buf, int buflen, LoggerLevel level)
{
    if (!logger || !buf)
        return;

    if (logger->level > level)
        return;

    if (!logger->file)
        return;

    fwrite(buf, buflen - 1, 1, logger->file);
    fflush(logger->file);
}

void
debug(Logger *logger, char *buf)
{
    int DEBUG_LEN = 9;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + DEBUG_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "DEBUG: %s\n", buf);

    flog(logger, message, message_len, LOG_DEBUG);

    free(message);
    return;
}

void
info(Logger *logger, char *buf)
{
    int INFO_LEN = 8;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + INFO_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "INFO: %s\n", buf);

    flog(logger, message, message_len, LOG_INFO);

    free(message);
    return;
}

void
warn(Logger *logger, char *buf)
{
    int WARN_LEN = 8;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + WARN_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "WARN: %s\n", buf);

    flog(logger, message, message_len, LOG_WARN);

    free(message);
    return;
}

void
error(Logger *logger, char *buf)
{
    int ERROR_LEN = 9;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + ERROR_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "ERROR: %s\n", buf);

    flog(logger, message, message_len, LOG_WARN);

    free(message);
    return;
}

