#ifndef PROCESSBUILDER_LIBRARY_H
#define PROCESSBUILDER_LIBRARY_H

#include <sys/types.h>

typedef struct process {
    char *command;      // The command to be executed
    char **args;        // Arguments for the command (NULL-terminated)
    pid_t pid;          // Process ID
    int inputStream;    // Input stream file descriptor
    int outputStream;   // Output stream file descriptor
} *Process;

/**
 * Create the Process struct and use the function startProcess to start it.
 * @param path The relative path of the process.
 * @param args Arguments of the program (NULL-terminated, doesn't need to start with processName).
 * @return A Process struct pointer in case of success, or NULL in case of an error.
 */
Process createProcess(char *path, char **args);

/**
 * Start a process.
 * @param process The Process struct representing the process to start.
 * @return The process ID (pid) of the started process, or -1 in case of an error.
 */
int startProcess(Process process);

/**
 * Get the input stream file descriptor of a process.
 * @param process The Process struct representing the process.
 * @return The input stream file descriptor.
 */
int getInputStream(Process process);

/**
 * Get the output stream file descriptor of a process.
 * @param process The Process struct representing the process.
 * @return The output stream file descriptor.
 */
int getOutputStream(Process process);

/**
 * Set a new input stream for a process that's not already started.
 * @param process The Process struct representing the process.
 * @param newFd The file descriptor of the new input stream.
 */
void setInputStream(Process process, int newFd);

/**
 * Set a new output stream for a process that's not already started.
 * @param process The Process struct representing the process.
 * @param newFd The file descriptor of the new output stream.
 */
void setOutputStream(Process process, int newFd);

/**
 * Write data to the input stream of the target process.
 * @param target The target process to write to.
 * @param message The message to write.
 * @param nBytes The size in bytes of the message.
 * @return The number of bytes written, or -1 in case of an error.
 */
size_t sendToProcess(Process target, void *message, size_t nBytes);

/**
 * Kill the target process with a signal.
 * @param target The target process to kill.
 * @param signal The signal to use for killing.
 * @return The killing exit status.
 */
int killProcess(Process target, short signal);

/**
 * Await the termination of the process.
 * @param process The Process struct representing the process.
 * @return The exit status of the process.
 */
int waitForTermination(Process process);

/* coming next...

Process cloneProcess(Process father);

int waitTermination(Process target);

char getStatus(Process pid);

int pidExist(Process pid);

int setCwd(Process pid, char *newCwd);

char *getCwd(Process pid);

int getErrorPipe(Process pid);

int setErrorPipe(Process pid, int newFd);

void inheritIO(Process p);

char *getName(Process pid);

int changePriority(Process pid, short newPriority);

size_t getNumThreads(Process pid);
 */

#endif //PROCESSBUILDER_LIBRARY_H
