#include "library.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>
#define MAX_SIZE 1024

Process createProcess(char *path, char *args[])
{
    int i = 0, size = 0;
    char **realArgs; // We use this to add the name of the program on top of the arguments
    Process process;

    // Check if the provided path is NULL
    if (path == NULL)
        return NULL;

    // Allocate memory for the Process structure
    process = malloc(sizeof(*process));
    if (process == NULL)
        return NULL;

    // Allocate memory for the command and copy the path into it
    process->command = malloc(strlen(path) + 1);
    strncpy(process->command, path, strlen(path) + 1);
    process->pid = 0; // The process doesn't have a PID because it's not started

    // Calculate the size of the arguments array
    while (args[size] != NULL)
        size++;

    // Allocate memory for realArgs (size + 2 for the program name and NULL terminator)
    realArgs = calloc(size + 2, sizeof(char *));
    if (realArgs == NULL) {
        free(process->command);
        free(process);
        return NULL;
    }

    // Populate realArgs with the provided arguments
    if (args != NULL) {
        for (i = 0; args[i] != NULL && i < size; i++) {
            realArgs[i + 1] = strdup(args[i]);
        }
    }

    // Set the first argument to the command and terminate the array with NULL
    realArgs[0] = process->command; // Args need to start with the process name
    realArgs[i + 1] = NULL;         // Args need to finish with NULL
    process->args = realArgs;
    process->inputStream = STDIN_FILENO;
    process->outputStream = STDOUT_FILENO;

    return process;
}

int startProcess(Process process) {
    if (process == NULL) {
        fprintf(stderr, "Error: NULL process provided\n");
        return -1;
    }

    if (process->pid != 0 && process->pid != -1) {
        fprintf(stderr, "Error: Process already started, pid: %d\n", process->pid);
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: Failed to create a Process\n");
        return -1;
    }

    else if (pid == 0) { // Child process
        if (dup2(process->inputStream, STDIN_FILENO) == -1 || dup2(process->outputStream, STDOUT_FILENO) == -1) {
            fprintf(stderr, "failed to dup\n errno: %s",  strerror(errno));
            exit(1);
        }
        //fprintf(stdout, "Ready to start\n, cmd: %s\n, args[0]: %s\n", process->command, process->args[0]);
        if (execvp(process->command, process->args) == -1) {
            fprintf(stderr, "failed to start the process, command: %s, errno: %s\n", process->command, strerror(errno));
            exit(1);
        }

    } else { // Parent process
        process->pid = pid;
        return process->pid;
    }
    return -1;
}

int getInputStream(Process pid){
    return pid->inputStream;
}

int getOutputStream(Process pid){
    return pid->outputStream;
}

void setInputStream(Process pid, int newFd){
    pid->inputStream = newFd;
}

void setOutputStream(Process pid, int newFd){
    pid->outputStream = newFd;
}
size_t sendToProcess(Process target, void *message, size_t nBytes){
    return write(getInputStream(target), message, nBytes);
}

void deallocateProcess(Process process)
{
    int index = 0;

    if (process != NULL && process->pid != -1)
    {
        /**
         * After this, we can't free process->command because.
         * it is included in process->args[0].
         */
        while (process->args[index] != NULL)
        {
            free(process->args[index]);
            index++;
        }
        free(process->args);

        process->pid = -1;
        free(process);
    }
}

int signalProcess(Process target, int signal){
    return kill(target->pid, signal);
}


int killProcess(Process target){
    if(target != NULL){
        int exitStatus = kill(target->pid, 9);
        deallocateProcess(target);
        return exitStatus;
    }
    return -1;
}

int waitForTermination(Process target){
    int status = 0;
    waitpid(target->pid, &status, WEXITSTATUS(status));
    deallocateProcess(target);
    return WEXITSTATUS(status);
}


