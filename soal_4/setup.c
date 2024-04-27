#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_APPS 10
#define MAX_APP_NAME 50
#define FILENAME "running_apps.txt"

pid_t running_apps[MAX_APPS];
int num_running_apps = 0;

void addRunningApp(pid_t pid) {
    if (num_running_apps < MAX_APPS) {
        running_apps[num_running_apps++] = pid;
        printf("Jumlah aplikasi berjalan: %d\n", num_running_apps);
    }
}

void loadRunningApps() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        fscanf(file, "%d", &num_running_apps);
        for (int i = 0; i < num_running_apps; i++) {
            fscanf(file, "%d", &running_apps[i]);
        }
        fclose(file);
    }
}

void saveRunningApps() {
    FILE *file = fopen(FILENAME, "w");
    if (file != NULL) {
        fprintf(file, "%d\n", num_running_apps);
        for (int i = 0; i < num_running_apps; i++) {
            fprintf(file, "%d\n", running_apps[i]);
        }
        fclose(file);
    }
}

void openApps(char *apps[], int num_apps[]) {
    int i, j;
    pid_t pid;

    for (i = 0; i < MAX_APPS && apps[i] != NULL; i++) {
        for (j = 0; j < num_apps[i]; j++) {
            pid = fork();

            if (pid == 0) {
                execlp(apps[i], apps[i], NULL);
                exit(0);
            } else {
                addRunningApp(pid);
            }
        }
    }
}

void closeApp(char *app) {
    if (fork() == 0) {
        execlp("pkill", "pkill", app, NULL);
        exit(0);
    }
    sleep(1);
}

void closeAppsFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char app[256];
        int num;
        while (fscanf(file, "%s %d", app, &num) == 2) {
            closeApp(app);
        }
        fclose(file);
    }
}

void readConfigFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    char line[MAX_APP_NAME];
    char *app;
    int num;

    while (fgets(line, sizeof(line), file)) {
        app = strtok(line, " ");
        num = atoi(strtok(NULL, " \n"));

        for (int i = 0; i < num; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                execlp(app, app, NULL);
                exit(0);
            } else {
                addRunningApp(pid);
            }
        }
    }

    fclose(file);
}

void closeApps() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        int num_apps;
        fscanf(file, "%d", &num_apps);
        for (int i = 0; i < num_apps; i++) {
            int pid;
            fscanf(file, "%d", &pid);
            if (kill(pid, SIGTERM) == -1) {
                perror("Error killing process");
            }
        }
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    loadRunningApps();

    if (argc < 2) {
        printf("Penggunaan: %s -o <app1> <num1> <app2> <num2> ... <appN> <numN> or %s -f <filename> or %s -k\n", argv[0], argv[0], argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "-o") == 0) {
        char *apps[MAX_APPS];
        int num_apps[MAX_APPS];
        int numArgs = argc - 3;

        int i, j, appIndex = 0;
        for (i = 2; i < argc; i += 2) {
            apps[appIndex] = argv[i];
            num_apps[appIndex] = atoi(argv[i + 1]);
            appIndex++;
        }
        apps[appIndex] = NULL;

        openApps(apps, num_apps);
        saveRunningApps();
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Penggunaan: %s -f <filename>\n", argv[0]);
            exit(1);
        }
        readConfigFile(argv[2]);
        saveRunningApps();
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 3) {
            closeAppsFromFile(argv[2]);
        } else {
            closeApps();
            remove(FILENAME);
        }
    } else {
        printf("Opsi tidak valid\n");
        exit(1);
    }

    return 0;
}
