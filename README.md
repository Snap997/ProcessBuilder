# ProcessBuilder
A simple C library that provides a simple and flexible interface for managing processes in a Unix-like environment. It offers functionalities to create, start, and manipulate processes, making it easier for developers to incorporate process handling capabilities into their applications.

### Basic example:
```c
int main(){
    char *args[SIZE] = {"/bin/", "-al"};
    Process process = createProcess("ls", args);
    startProcess(process);
    waitForTermination(process);
}
```
if you want to redirect the output in a file you simply do:
```c
int main(){
    char *args[SIZE] = {"/bin/", "-al"};
    Process process = createProcess("ls", args);
    setOutputStream(process, open("out.txt", O_WRONLY, 0666));
    startProcess(process);
    waitForTermination(process);
}
```
