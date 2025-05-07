#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "treasure.h"

volatile sig_atomic_t monitor_running = 0;
volatile sig_atomic_t monitor_terminated = 0;
int pipe_fd[2]; // Pipe for communication

void handle_signal(int sig) {
    if (sig == SIGTERM) {
        printf("Monitor: Termination signal received. Exiting...\n");
        monitor_terminated = 1;
    }
}

void monitor_process() {
    close(pipe_fd[1]); // Close write end of the pipe in the monitor process
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGTERM, &sa, NULL);

    printf("Monitor process started (PID: %d).\n", getpid());

    char buffer[256];
    while (!monitor_terminated) {
        ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate the string
            char command[256], hunt_id[256];
            int treasure_id;

            if (sscanf(buffer, "%s %s %d", command, hunt_id, &treasure_id) >= 1) {
                if (strcmp(command, "list_hunts") == 0) {
                    printf("Monitor: Listing hunts...\n");
                    list(hunt_id);
                } else if (strcmp(command, "list_treasures") == 0) {
                    printf("Monitor: Listing treasures in hunt %s...\n", hunt_id);
                    list(hunt_id);
                } else if (strcmp(command, "view_treasure") == 0) {
                    printf("Monitor: Viewing treasure %d in hunt %s...\n", treasure_id, hunt_id);
                    view(hunt_id, treasure_id);
                }
            }
        }
    }

    close(pipe_fd[0]); // Close read end of the pipe
    usleep(500000);    // Simulate delay before exiting
    printf("Monitor process exiting.\n");
    exit(0);
}

int main() {
    pid_t monitor_pid = -1;
    char command[256];

    if (pipe(pipe_fd) == -1) {
        perror("Failed to create pipe");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("treasure_hub> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("Error reading command");
            continue;
        }

        command[strcspn(command, "\n")] = '\0'; // Remove newline character

        if (strcmp(command, "start_monitor") == 0) {
            if (monitor_running) {
                printf("Monitor is already running.\n");
                continue;
            }

            monitor_pid = fork();
            if (monitor_pid == -1) {
                perror("Failed to start monitor");
                continue;
            }

            if (monitor_pid == 0) {
                monitor_process(); // Child process becomes the monitor
            } else {
                monitor_running = 1;
                close(pipe_fd[0]); // Close read end of the pipe in the parent process
                printf("Monitor started (PID: %d).\n", monitor_pid);
            }

        } else if (strncmp(command, "list_hunts", 10) == 0) {
            if (!monitor_running) {
                printf("Error: Monitor is not running.\n");
                continue;
            }
            write(pipe_fd[1], "list_hunts", strlen("list_hunts"));

        } else if (strncmp(command, "list_treasures", 14) == 0) {
            if (!monitor_running) {
                printf("Error: Monitor is not running.\n");
                continue;
            }
            char hunt_id[256];
            sscanf(command, "list_treasures %s", hunt_id);
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "list_treasures %s", hunt_id);
            write(pipe_fd[1], buffer, strlen(buffer));

        } else if (strncmp(command, "view_treasure", 13) == 0) {
            if (!monitor_running) {
                printf("Error: Monitor is not running.\n");
                continue;
            }
            char hunt_id[256];
            int treasure_id;
            sscanf(command, "view_treasure %s %d", hunt_id, &treasure_id);
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "view_treasure %s %d", hunt_id, treasure_id);
            write(pipe_fd[1], buffer, strlen(buffer));

        } else if (strcmp(command, "stop_monitor") == 0) {
            if (!monitor_running) {
                printf("Error: Monitor is not running.\n");
                continue;
            }
            kill(monitor_pid, SIGTERM);
            printf("Waiting for monitor to terminate...\n");
            waitpid(monitor_pid, NULL, 0);
            monitor_running = 0;
            close(pipe_fd[1]); // Close write end of the pipe
            printf("Monitor terminated.\n");

        } else if (strcmp(command, "exit") == 0) {
            if (monitor_running) {
                printf("Error: Monitor is still running. Stop it first.\n");
                continue;
            }
            printf("Exiting treasure_hub.\n");
            break;

        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    return 0;
}