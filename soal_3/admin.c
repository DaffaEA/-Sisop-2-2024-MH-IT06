#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void monitor_processes(const char *user) {
    char logfile[50];
    sprintf(logfile, "%s.log", user);

    while (1) {
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char timestamp[50];
        strftime(timestamp, sizeof(timestamp), "[%d:%m:%Y]-[%H:%M:%S]", timeinfo);

        FILE *log_fp = fopen(logfile, "a");
        if (log_fp == NULL) {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }
        fprintf(log_fp, "%s\n", timestamp);
        fclose(log_fp);

        pid_t pid = fork();
        if (pid == -1) {
            perror("Error forking");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            int fd = open(logfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                perror("Error opening log file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            execlp("ps", "ps", "-u", user, NULL);
            perror("Error executing command");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                perror("Child process failed");
                exit(EXIT_FAILURE);
            }
            // Print output to terminal
            printf("Output for user %s:\n", user);
            FILE *p = popen("ps -u maximumyeet", "r");
            if (p == NULL) {
                perror("Error executing command");
                exit(EXIT_FAILURE);
            }
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), p) != NULL) {
                printf("%s", buffer);
            }
            pclose(p);
        }

        sleep(60);
    }
}

void stop_monitoring(const char *user) {
    printf("Monitoring stopped for user %s.\n", user);
}

int main(int argc, char *argv[]) {
    if (argc != 3 || (strcmp(argv[1], "-m") != 0 && strcmp(argv[1], "-s") != 0)) {
        printf("Usage: ./admin <-m/-s> <user>\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-m") == 0) {
        monitor_processes(argv[2]);
    } else {
        stop_monitoring(argv[2]);
    }

    return EXIT_SUCCESS;
}
