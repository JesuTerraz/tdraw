#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX 10

// structure for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

int
main(int argc, char **argv)
{
    key_t key;
    int msgid;

    if (argc < 2)
    {
        fprintf(stdout, "Invalid usage:\n");
        fprintf(stdout, "\tipc <message>\n");
        return (-1);
    }

    // ftok to generate unique key
    key = ftok("progfile", 65);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.mesg_type = 1;

    // Write Data
    strncpy(message.mesg_text, argv[1], 100);

    // msgsnd to send message
    msgsnd(msgid, &message, sizeof(message), 0);

    // display the message
    fprintf(stdout, "INFO: data sent, %s\n", message.mesg_text);

    return 0;
}