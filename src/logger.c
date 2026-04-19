#include "logger.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
flog(char *buf, int buflen, LoggerLevel level)
{
    if (!buf || !buflen)
        return;

    if (!logger)
        return;

    if (logger->level > level)
        return;

    if (!logger->file)
        return;

    fprintf(logger->file, "%s", buf);
}

void
debug(char *buf)
{
    int DEBUG_LEN = 9;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + DEBUG_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "DEBUG: %s\n", buf);

    flog(message, message_len, LOG_DEBUG);

    free(message);
    return;
}

void
info(char *buf)
{
    int INFO_LEN = 8;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + INFO_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "INFO: %s\n", buf);

    flog(message, message_len, LOG_INFO);

    free(message);
    return;
}

void
warn(char *buf)
{
    int WARN_LEN = 8;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + WARN_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "WARN: %s\n", buf);

    flog(message, message_len, LOG_WARN);

    free(message);
    return;
}

void
error(char *buf)
{
    int ERROR_LEN = 9;
    if (!buf) {
        return;
    }

    int message_len = strlen(buf) + ERROR_LEN;
    char *message = malloc(message_len);
    snprintf(message, message_len, "ERROR: %s\n", buf);

    flog(message, message_len, LOG_WARN);

    free(message);
    return;
}

