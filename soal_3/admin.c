#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define LOG_FILE_EXTENSION ".log"
#define MAX_COMMAND_LENGTH 100
#define MAX_PROCESS_NAME_LENGTH 100
#define MAX_LINE_LENGTH 256
#define MAX_TIMESTAMP_LENGTH 50
#define MAX_LOGFILE_PATH_LENGTH 100

void monitor_processes(const char *user) {
    // Membuat nama file log
    char logfile[MAX_LOGFILE_PATH_LENGTH];
    snprintf(logfile, sizeof(logfile), "%s%s", user, LOG_FILE_EXTENSION);

    // Membuka file log untuk ditulis
    FILE *fp = fopen(logfile, "a");
    if (fp == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    printf("Monitoring for user '%s' started. Log file: %s\n", user, logfile);

    while (1) {
        // Mendapatkan timestamp
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char timestamp[MAX_TIMESTAMP_LENGTH];
        strftime(timestamp, sizeof(timestamp), "[%d:%m:%Y]-[%H:%M:%S]", timeinfo);

        // Menuliskan timestamp ke konsol dan file log
        fprintf(fp, "%s\n", timestamp);
        printf("%s\n", timestamp);

        // Menjalankan command ps untuk mendapatkan proses pengguna
        char command[MAX_COMMAND_LENGTH];
        snprintf(command, sizeof(command), "ps -u %s", user);
        FILE *p = popen(command, "r");
        if (p == NULL) {
            fprintf(stderr, "Error executing command: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Membaca output dari command ps
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), p) != NULL) {
            int pid;
            char process_name[MAX_PROCESS_NAME_LENGTH];
            if (sscanf(line, "%d %[^\n]", &pid, process_name) == 2) {
                // Menuliskan informasi proses ke konsol dan file log
                fprintf(fp, "%d-process_%s_JALAN\n", pid, process_name);
                printf("%d-process_%s_JALAN\n", pid, process_name);
            }
        }
        pclose(p);
        sleep(60);  // Monitor every minute
    }

    fclose(fp);
}

void stop_monitoring(const char *user) {
    printf("Monitoring for user '%s' has been stopped.\n", user);
}

int main(int argc, char *argv[]) {
    if (argc != 3 || (strcmp(argv[1], "-m") != 0 && strcmp(argv[1], "-s") != 0)) {
        printf("Usage: ./admin <-m/-s> <user>\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-m") == 0) {
        // Start monitoring
        monitor_processes(argv[2]);
    } else {
        // Stop monitoring
        stop_monitoring(argv[2]);
    }

    return EXIT_SUCCESS;
}
