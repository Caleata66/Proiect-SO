#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "treasure.h"
#include <dirent.h>
#include <sys/stat.h>

#define BUF_SIZE 512

volatile sig_atomic_t got_command = 0;

void sigusr1_handler(int sig) {
    got_command = 1;
}

void list_all_hunts_with_treasures() {
    DIR *d = opendir(".");
    if (!d) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Skip . and ..
            if (entry->d_name[0] == '.')
                continue;
            pid_t pid = fork();
            if (pid == 0) {
                execl("./treasure_manager", "treasure_manager", "--list", entry->d_name, (char*)NULL);
                perror("execl failed");
                exit(1);
            } else if (pid > 0) {
                waitpid(pid, NULL, 0);
            }
        }
    }
    closedir(d);
}

void monitor_process(int read_fd) {
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    struct sigaction sa_term;
    sa_term.sa_handler = SIG_DFL;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);

    char buf[BUF_SIZE];
    while (1) {
        pause(); // Wait for a signal
        if (got_command) {
            got_command = 0;
            ssize_t n = read(read_fd, buf, BUF_SIZE - 1);
            if (n <= 0) break;
            buf[n] = 0;
            if (strncmp(buf, "list_hunts", 10) == 0) {
                list_all_hunts_with_treasures();
            } else if (strncmp(buf, "list_treasures", 14) == 0) {
                char hunt[256];
                sscanf(buf, "list_treasures %255s", hunt);
                pid_t pid = fork();
                if (pid == 0) {
                    execl("./treasure_manager", "treasure_manager", "--list", hunt, (char*)NULL);
                    perror("execl failed");
                    exit(1);
                } else if (pid > 0) {
                    waitpid(pid, NULL, 0);
                }
            } else if (strncmp(buf, "view_treasure", 13) == 0) {
                char hunt[256]; int tid;
                sscanf(buf, "view_treasure %255s %d", hunt, &tid);
                pid_t pid = fork();
                if (pid == 0) {
                    execl("./treasure_manager", "treasure_manager", "--view", hunt, tid, (char*)NULL);
                    perror("execl failed");
                    exit(1);
                } else if (pid > 0) {
                    waitpid(pid, NULL, 0);
                }
            } else if (strncmp(buf, "calculate_score", 15) == 0) {
                char hunt[256]; char user_name[32];
                sscanf(buf, "calculate_score %255s %31s", hunt, user_name);
                pid_t pid = fork();
                if (pid == 0) {
                    execl("./score_calculator", "score_calculator", hunt, user_name, (char*)NULL);
                    perror("execl failed");
                    exit(1);
                } else if (pid > 0) {
                    waitpid(pid, NULL, 0);
                }
            }else if (strncmp(buf, "stop", 4) == 0) {
                printf("[monitor] Stopping monitor (delayed)...\n");
                fflush(stdout);
                usleep(500000); // simulate delay
                break;
            }
            fflush(stdout);
        }
    }
    exit(0);
}

int main(void) {
    int pipefd[2];
    pid_t monitor_pid = -1;
    int monitor_running = 0;
    char cmd[BUF_SIZE];

    while (1) {
        printf("treasure_hub> ");
        fflush(stdout);
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        cmd[strcspn(cmd, "\n")] = 0;

        if (strcmp(cmd, "start_monitor") == 0) {
            if (monitor_running) {
                printf("Monitor already running.\n");
                continue;
            }
            if (pipe(pipefd) == -1) {
                perror("pipe");
                continue;
            }
            monitor_pid = fork();
            if (monitor_pid == 0) {
                close(pipefd[1]);
                monitor_process(pipefd[0]);
                exit(0);
            } else if (monitor_pid > 0) {
                close(pipefd[0]);
                monitor_running = 1;
                printf("Monitor started (PID %d).\n", monitor_pid);
            } else {
                perror("fork");
            }
        } else if (strcmp(cmd, "list_hunts") == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write(pipefd[1], "list_hunts", strlen("list_hunts"));
            kill(monitor_pid, SIGUSR1);
        } else if (strncmp(cmd, "list_treasures", 14) == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write(pipefd[1], cmd, strlen(cmd));
            kill(monitor_pid, SIGUSR1);
        } else if (strncmp(cmd, "view_treasure", 13) == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write(pipefd[1], cmd, strlen(cmd));
            kill(monitor_pid, SIGUSR1);
        }
        else if (strncmp(cmd, "calculate_score", 15) == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write(pipefd[1], cmd, strlen(cmd));
            kill(monitor_pid, SIGUSR1);
        } else if (strcmp(cmd, "stop_monitor") == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write(pipefd[1], "stop", 4);
            kill(monitor_pid, SIGUSR1);
            close(pipefd[1]);
            printf("Waiting for monitor to terminate...\n");
            waitpid(monitor_pid, NULL, 0);
            printf("Monitor terminated.\n");
            monitor_running = 0;
        } else if (strcmp(cmd, "exit") == 0) {
            if (monitor_running) {
                printf("Error: Monitor still running. Stop it first.\n");
                continue;
            }
            break;
        } else {
            printf("Unknown command.\n");
        }
    }
    return 0;
}