#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_APPS 50
#define MAX_NAME_LENGTH 50

typedef struct {
    char name[MAX_NAME_LENGTH];
    int num_windows;
} AppInfo;

void open_app(const char *app_name, int num_windows) {
    for (int i = 0; i < num_windows; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (execlp(app_name, app_name, NULL) == -1) {
                perror("Error opening application");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("Error forking process");
            exit(EXIT_FAILURE);
        }
    }
}

void kill_all_apps() {
    execlp("pkill", "pkill", "-f", "setup", NULL);
    perror("Error killing applications");
    exit(EXIT_FAILURE);
}

void kill_apps_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    char app_name[MAX_NAME_LENGTH];
    int num_windows;

    while (fscanf(file, "%s %d", app_name, &num_windows) == 2) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (execlp("pkill", "pkill", "-f", app_name, NULL) == -1) {
                perror("Error killing application");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("Error forking process");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

void handle_open(int argc, char *argv[]) {
    if (argc < 4 || (argc - 2) % 2 != 0) {
        printf("Usage: ./setup -o <app1> <num1> <app2> <num2> ... <appN> <numN>\n");
        exit(EXIT_FAILURE);
    }

    int num_apps = (argc - 2) / 2;
    AppInfo apps[MAX_APPS];

    for (int i = 0; i < num_apps; ++i) {
        strcpy(apps[i].name, argv[2 + i * 2]);
        apps[i].num_windows = atoi(argv[3 + i * 2]);
    }

    for (int i = 0; i < num_apps; ++i) {
        open_app(apps[i].name, apps[i].num_windows);
    }
}

void handle_from_file(const char *filename) {
    kill_all_apps(); 

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    char app_name[MAX_NAME_LENGTH];
    int num_windows;

    while (fscanf(file, "%s %d", app_name, &num_windows) == 2) {
        open_app(app_name, num_windows);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./setup [-o <app1> <num1> <app2> <num2> ... <appN> <numN>] | [-f <filename>] | [-k | -k <filename>]\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-o") == 0) {
        handle_open(argc, argv);
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Usage: ./setup -f <filename>\n");
            return EXIT_FAILURE;
        }
        handle_from_file(argv[2]);
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 2) {
            kill_all_apps();
        } else if (argc == 3) {
            handle_from_file(argv[2]);
        } else {
            printf("Usage: ./setup -k | -k <filename>\n");
            return EXIT_FAILURE;
        }
    } else {
        printf("Invalid option\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

