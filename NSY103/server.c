#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

void sigInt(int code)
{
    fprintf(stderr, ">>> SIGINT received [%d]\n", code);

    // Pour forcer les appels sur les fonctions enregistrées par atexit
    exit(EXIT_SUCCESS);
}

void sigHup(int code)
{
    printf(">>> SIGHUP received [%d]\n", code);
}

char buffer[256];

int main(void)
{
    // int now;
    // time(&now);
    struct tm *local;
    time_t now = time(NULL);
    local = localtime(&now);
    pid_t pid;
    FILE *fileLog = NULL;
    // creation fichier log
    fileLog = fopen("log.txt", "a");

    fprintf(fileLog, "process pere tué: \n");
    // fork off the parent process = child process
    // This system call gets called once (in parent process)
    // but returns twice (once in parent and second time in child).
    // Note that after the fork() system call,
    // whether the parent will run first or the child is non-deterministic.
    // It purely depends on the context switch mechanism.
    // This call returns zero in child while returns PID of child process
    // in the parent process.
    pid = fork();
    fprintf(fileLog, "process numéro %d : %s\n", pid, asctime(local));
    // test child process creation
    if (pid < 0)
    {
        fprintf(fileLog, "Echec de creation processus fils: \n");
        // failure
        exit(1);
    }

    // kill parent process
    if (pid > 0)
    {
        printf(">>> Parent process killed: \n", pid);
        fprintf(fileLog, "process pere tué: \n");
        exit(0);
    }

    // control file permission
    umask(0);

    /* Open any logs here */

    // creation new session

    int sid = setsid();

    if (sid < 0)
    {
        fprintf(fileLog, "session foirées: \n");
        exit(1);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0)
    {
        /* Log any failure here */
        exit(EXIT_FAILURE);
    }

    //daemon();
    signal(SIGINT, &sigInt);
    signal(SIGHUP, &sigHup);

    printf("Enter your command: \n");
    scanf("%s", buffer);
    fprintf(fileLog, "process pere tué:\n");
    int signalId = atoi(buffer);
    fprintf(fileLog, "convertion buffer en int: \n");
    raise(signalId);

    return EXIT_SUCCESS;
}
