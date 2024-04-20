#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

void download() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char *args[] = {"wget", "-O", "library.zip", "https://drive.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", NULL};
        
        if (execvp("wget", args) == -1) {
            perror("Error executing wget");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        }
    }
}

void unzip() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char *args[] = {"unzip", "-o", "library.zip", NULL};
        
        if (execvp("unzip", args) == -1) {
            perror("Error executing unzip");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        }
    }
}

void decrypt_filenames(const char *dir_path) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(dir_path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                char filename[100];
                strcpy(filename, ent->d_name);

                for (int i = 0; filename[i] != '\0'; ++i) {
                    char c = filename[i];
                    if (c >= 'A' && c <= 'Z')
                        filename[i] = ((c - 'A' + 7) % 26) + 'A';
                    else if (c >= 'a' && c <= 'z')
                        filename[i] = ((c - 'a' + 7) % 26) + 'a';
                }

                char old_path[256];
                char new_path[256];
                sprintf(old_path, "%s/%s", dir_path, ent->d_name);
                sprintf(new_path, "%s/%s", dir_path, filename);

                if (rename(old_path, new_path) != 0) {
                    perror("Error renaming file");
                }
            }
        }
        closedir(dir);
    } 
}

void delete_files(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (strstr(entry->d_name, "d3Let3") != NULL) {
                char filepath[256];
                sprintf(filepath, "%s/%s", dir_path, entry->d_name);
                if (remove(filepath) != 0) {
                    perror("Error deleting file");
                } else {
                    printf("File deleted: %s\n", entry->d_name);
                }
            }
        }
    }

    closedir(dir);
}

void rename_files(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "r3N4mE") != NULL) {
            char old_path[1024];
            char new_path[1024];
            snprintf(old_path, sizeof(old_path), "%s/%s", dir_path, entry->d_name);

            char *new_filename;
            if (strcmp(entry->d_name + strlen(entry->d_name) - 3, ".ts") == 0) {
                new_filename = "helper.ts";
            } else if (strcmp(entry->d_name + strlen(entry->d_name) - 3, ".py") == 0) {
                new_filename = "calculator.py";
            } else if (strcmp(entry->d_name + strlen(entry->d_name) - 3, ".go") == 0) {
                new_filename = "server.go";
            } else {
                new_filename = "renamed.file";
            }

            snprintf(new_path, sizeof(new_path), "%s/%s", dir_path, new_filename);
            if (rename(old_path, new_path) != 0) {
                perror("Error renaming file");
            }
        }
    }
    closedir(dir);
}

int main() {
    download();
    unzip();
    decrypt_filenames("library");
    rename_files("library");
    delete_files("library");

    return 0;
}
