#include "logger.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include <stdarg.h>

#define PATH_MAX    100
#define TTY_MAX     100

char cwd[PATH_MAX];
char tty[TTY_MAX];

struct mesg_buffer {
    long mesg_type;
    char mesg_text[TTY_MAX];
} message;

static Logger *logger;

static int
retrieve_tty()
{
    key_t key;
    int msgid;

    // ftok to generate unique key
    key = ftok("progfile", 65);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Block until at least one message arrives
    if (msgrcv(msgid, &message, sizeof(message), 1, 0) < 0) {
        return (-1);
    }

    fprintf(stdout, "Message recieved: %s\n", message.mesg_text);

    // to destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    logger->file = fopen(message.mesg_text, "w");
    if (!logger->file)
    {
        return (-1);
    }

    return (0);
}

int
init_logger()
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        perror("ERROR: fork returned error.");
        return (-1);
    }

    if (!pid)
    {
        if (!getcwd(cwd, sizeof(cwd)))
        {
            perror("ERROR: getcwd returned error.");
            exit(1);
        }

        char cmd[PATH_MAX + 256];
        snprintf(cmd, sizeof(cmd),
            "tell application \"Terminal\" to do script \"TTY=$(tty) && cd '%s' && ./ipc $TTY\"", cwd);

        char *const argv[] = { 
            "osascript",
            "-e", cmd, 
            "-e", "tell application \"Terminal\" to activate",
            NULL };

        execv("/usr/bin/osascript", argv);

        perror("Unable to start terminal, exiting...");
        exit(1);
    }

    waitpid(pid, NULL, 0);

    logger = malloc(sizeof(Logger));
    if (!logger)
    {
        return (-1);
    }

    if (retrieve_tty())
    {
        return (-1);
    }

    logger->level = LOG_DEBUG;
    return (0);
}

void
set_level(LoggerLevel level)
{
    logger->level = level;
    return;
}

void
flog(LoggerLevel level, char *format, va_list args)
{
    if (!format)
        return;

    if (!logger)
        return;

    if (logger->level > level)
        return;

    if (!logger->file)
        return;

    vfprintf(logger->file, format, args);
    // fflush(logger->file);
}

void
debug(char *format, ...)
{
    va_list arg;

    int DEBUG_LEN = 9;

    if (!format) {
        return;
    }

    int message_len = strlen(format) + DEBUG_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "DEBUG: %s\n", format);

    va_start(arg, format);
    flog(LOG_DEBUG, message, arg);
    va_end(arg);

    free(message);
    return;
}

void
info(char *format, ...)
{
    va_list arg;

    int INFO_LEN = 8;

    if (!format) {
        return;
    }

    int message_len = strlen(format) + INFO_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "INFO: %s\n", format);

    va_start(arg, format);
    flog(LOG_INFO, message, arg);
    va_end(arg);

    free(message);
    return;
}

void
warn(char *format, ...)
{
    va_list arg;

    int WARN_LEN = 8;
    if (!format) {
        return;
    }

    int message_len = strlen(format) + WARN_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "WARN: %s\n", format);

    va_start(arg, format);
    flog(LOG_WARN, message, arg);
    va_end(arg);

    free(message);
    return;
}

void
error(char *format, ...)
{
    va_list arg;

    int ERROR_LEN = 9;

    if (!format) {
        return;
    }

    int message_len = strlen(format) + ERROR_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "ERROR: %s\n", format);

    va_start(arg, format);
    flog(LOG_ERROR, message, arg);
    va_end(arg);

    free(message);
    return;
}

