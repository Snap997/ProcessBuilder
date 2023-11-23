#include "library.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>
#define MAX_SIZE 1024

Process createProcess(char *path, char *args[]){
    if(path == NULL){
        return NULL;
    }
    Process process = malloc(sizeof(*process));
    if(process == NULL) {
        return NULL;
    }

    char processCommand[MAX_SIZE];
    strncpy(processCommand, path, MAX_SIZE-1);
    process->command = processCommand;
    process->pid = 0; // the process doesn't have a pid because it's not started
    char *realArgs[MAX_SIZE]; // we do this to add the name of the program on top of the arguments
    realArgs[0] = process->command; // args needs to start with the process name
    int i =0;
    if(args != NULL){
        //printf("%s\n", process->command);
        for(i; args[i] != NULL && i<MAX_SIZE-1; i++){
            realArgs[i+1] = strdup(args[i]);
        }
    }
    realArgs[i+1] = NULL; //args needs to finish with NULL
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

void deallocateProcess(Process process){
    if(process != NULL && process->pid != -1) {
        //free(process->args);
        process->pid = -1;
        free(process);
    }
}

int killProcess(Process target, short signal){
    if(target != NULL){
        int exitStatus = kill(target->pid, signal);
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
