#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_APPS 10
#define MAX_NAME_LENGTH 50
#define MAX_COMMAND_LENGTH 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int numWindows;
} AppInfo;

void openApp(const char *appName, int numWindows) {
    pid_t pid;
    for (int i = 0; i < numWindows; ++i) {
        pid = fork();
        if (pid == 0) { 
            execlp(appName, appName, NULL);
            perror("Error: exec failed");
            exit(1);
        } else if (pid < 0) { 
            perror("Error: fork failed");
            exit(1);
        }
    }
}

void closeAllApps() {
    pid_t pid;
    pid = fork();
    if (pid == 0) { 
        execlp("pkill", "pkill", "-f", "*", NULL); 
        perror("Error: exec failed");
        exit(1);
    } else if (pid < 0) { 
        perror("Error: fork failed");
        exit(1);
    }
}

void readConfig(const char *filename, AppInfo apps[], int *numApps) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    *numApps = 0;
    while (fscanf(file, "%s %d", apps[*numApps].name, &apps[*numApps].numWindows) == 2) {
        (*numApps)++;
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s -o <app1> <num1> <app2> <num2> ... OR %s -f <config_file> OR %s -k OR %s -k <config_file>\n", argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") == 0) { 
        int i = 2;
        while (i < argc) {
            openApp(argv[i], atoi(argv[i + 1])); 
            i += 2;
        }
    } else if (strcmp(argv[1], "-f") == 0) { 
        if (argc != 3) {
            printf("Usage: %s -f <config_file>\n", argv[0]);
            return 1;
        }
        AppInfo apps[MAX_APPS];
        int numApps;
        readConfig(argv[2], apps, &numApps); 
        for (int i = 0; i < numApps; ++i) {
            openApp(apps[i].name, apps[i].numWindows); 
        }
    } else if (strcmp(argv[1], "-k") == 0) { 
        if (argc == 2) {
            closeAllApps(); 
        } else if (argc == 3) {
            AppInfo apps[MAX_APPS];
            int numApps;
            readConfig(argv[2], apps, &numApps); 
        } else {
            printf("Usage: %s -k OR %s -k <config_file>\n", argv[0], argv[0]);
            return 1;
        }
    } else {
        printf("Invalid option\n");
        return 1;
    }

    return 0;
}
