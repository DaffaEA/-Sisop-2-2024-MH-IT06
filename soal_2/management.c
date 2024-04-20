#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

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
                    // Log the deletion action
                    FILE *log_file = fopen("history.log", "a");
                    if (log_file != NULL) {
                        time_t raw_time;
                        struct tm *time_info;
                        time(&raw_time);
                        time_info = localtime(&raw_time);
                        fprintf(log_file, "[shittim][%02d:%02d:%02d] - %s - Succesfully deleted\n",
                                time_info->tm_hour, time_info->tm_min, time_info->tm_sec, entry->d_name);
                        fclose(log_file);
                    }
                }
            }
        }
    }

    closedir(dir);
}

void backup(const char *dir_path) {
    // Membuat direktori "backup" jika belum ada
    char backup[256];
    sprintf(backup, "%s/backup", dir_path);
    if (mkdir(backup, 0777) == -1) {
        perror("Error creating backup directory");
        exit(EXIT_FAILURE);
    }

    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (strstr(entry->d_name, "m0V3") != NULL) {
                char filepath[256];
                sprintf(filepath, "%s/%s", dir_path, entry->d_name);

                char backup_filepath[256];
                sprintf(backup_filepath, "%s/%s/%s", dir_path, "backup", entry->d_name);

                if (rename(filepath, backup_filepath) != 0) {
                    perror("Error moving file to backup");
                } else {
                    printf("File moved to backup: %s\n", entry->d_name);
                    // Log the backup action
                    FILE *log_file = fopen("history.log", "a");
                    if (log_file != NULL) {
                        time_t raw_time;
                        struct tm *time_info;
                        time(&raw_time);
                        time_info = localtime(&raw_time);
                        fprintf(log_file, "[shittim][%02d:%02d:%02d] - %s - Succesfully moved to backup\n",
                                time_info->tm_hour, time_info->tm_min, time_info->tm_sec, entry->d_name);
                        fclose(log_file);
                    }
                }
            }
        }
    }

    closedir(dir);
}

void restore(const char *dir_path) {
    char backup_dir[256];
    sprintf(backup_dir, "%s/backup", dir_path);

    DIR *backup = opendir(backup_dir);
    if (backup == NULL) {
        perror("Error opening backup directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(backup)) != NULL) {
        if (entry->d_type == DT_REG) {
            char backup_filepath[256];
            sprintf(backup_filepath, "%s/%s", backup_dir, entry->d_name);

            char filepath[256];
            sprintf(filepath, "%s/%s", dir_path, entry->d_name);

            if (rename(backup_filepath, filepath) != 0) {
                perror("Error restoring file from backup");
            } else {
                printf("File restored from backup: %s\n", entry->d_name);
                // Log the restoration action
                FILE *log_file = fopen("history.log", "a");
                if (log_file != NULL) {
                    time_t raw_time;
                    struct tm *time_info;
                    time(&raw_time);
                    time_info = localtime(&raw_time);
                    fprintf(log_file, "[shittim][%02d:%02d:%02d] - %s - Succesfully restored from backup\n",
                            time_info->tm_hour, time_info->tm_min, time_info->tm_sec, entry->d_name);
                    fclose(log_file);
                }
            }
        }
    }

    closedir(backup);
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
            } else {
                // Log the renaming action
                FILE *log_file = fopen("history.log", "a");
                if (log_file != NULL) {
                    time_t raw_time;
                    struct tm *time_info;
                    time(&raw_time);
                    time_info = localtime(&raw_time);
                    fprintf(log_file, "[shittim][%02d:%02d:%02d] - %s - Succesfully renamed\n",
                            time_info->tm_hour, time_info->tm_min, time_info->tm_sec, entry->d_name);
                    fclose(log_file);
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc > 2 && !(argc == 3 && strcmp(argv[1], "-m") == 0)) {
        fprintf(stderr, "Invalid option. Use -m <backup|restore>\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 2 && strcmp(argv[1], "-m") != 0) {
        fprintf(stderr, "Invalid option. Use -m <backup|restore>\n");
        exit(EXIT_FAILURE);
    }

    download();
    unzip();
    remove("library.zip");
    decrypt_filenames("library");
    delete_files("library");
    rename_files("library");

    if (argc == 3) {
        if (strcmp(argv[1], "-m") == 0) {
            if (strcmp(argv[2], "backup") == 0) {
                backup("library");
            } else if (strcmp(argv[2], "restore") == 0) {
                restore("library");
            } else {
                fprintf(stderr, "Invalid operation. Use backup or restore.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Invalid option. Use -m <backup|restore>\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
