#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#define MAX_PATH_LENGTH 1000
#define MAX_BUFFER_LENGTH 1000

void listFilesRecursively(char *basePath);

int main() {
    pid_t pid, sid;

    char path[MAX_PATH_LENGTH];

    printf("Enter path to list files: ");
    scanf("%s", path);

    FILE *file = fopen("virus.log", "w");
    if (file == NULL) {
        perror("fopen failed");
        return 1;
    }

    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // Parent process exits
        fclose(file);
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    listFilesRecursively(path);

    return 0;
}

void listFilesRecursively(char *basePath) {
    char path[MAX_PATH_LENGTH];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if (!dir) {
        perror("opendir failed");
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            if (dp->d_type == DT_DIR) {
                listFilesRecursively(path);
            } else if (dp->d_type == DT_REG) {
                FILE *file = fopen(path, "r+");
                if (file == NULL) {
                    perror("fopen failed");
                    continue;
                }

                char buffer[MAX_BUFFER_LENGTH];
                size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
                if (bytesRead == 0) {
                    perror("fread failed");
                    fclose(file);
                    continue;
                }

                const char *malwareStrings[] = {"m4LwAr3", "5pYw4R3", "R4nS0mWaR3"};
                const char *replacements[] = {"[MALWARE]", "[SPYWARE]", "[RANSOMWARE]"};
                int count = 0;

                for (int i = 0; i < 3; i++) {
                    char *pos = strstr(buffer, malwareStrings[i]);
                    if (pos != NULL) {
                        fseek(file, pos - buffer, SEEK_SET);
                        fwrite(replacements[i], 1, strlen(replacements[i]), file);

                        time_t t = time(NULL);
                        struct tm *tm_info = localtime(&t);

                        // Format the date and time
                        char datetime[20];
                        strftime(datetime, sizeof(datetime), "%d/%m/%y %H:%M:%S", tm_info);

                        // Format the log message
                        char message[MAX_BUFFER_LENGTH];
                        snprintf(message, sizeof(message), "%s Suspicious string in %s successfully replaced\n", datetime, dp->d_name);

                        // Open the log file in append mode
                        FILE *logfile = fopen("virus.log", "a");
                    
                        // Write the message to the log file
                        fprintf(logfile, "%s", message);

                        // Close the log file
                        fclose(logfile);
                        sleep(15);
                
                    }
                }

                fclose(file);
            
            }
        }
    }

    closedir(dir);
}
