#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void download() {
    // Command and arguments to download a file using wget
    char *args[] = {"wget", "-O", "library.zip", "https://drive.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", NULL};
    
    // Execute the command
    if (execvp("wget", args) == -1) {
        perror("Error executing wget");
        exit(EXIT_FAILURE);
    }
}

void unzip() {
    // Command and arguments to unzip a file
    char *args[] = {"unzip", "library.zip", NULL};
    
    // Execute the command
    if (execvp("unzip", args) == -1) {
        perror("Error executing unzip");
        exit(EXIT_FAILURE);
    }
}


#include <dirent.h>
#include <string.h>

void decrypt_filenames(const char *dir_path) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(dir_path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Only process regular files
                char filename[100];
                strcpy(filename, ent->d_name); // Added missing semicolon

                // Decrypting the filename
                for (int i = 0; filename[i] != '\0'; ++i) {
                    char c = filename[i];
                    if (c >= 'A' && c <= 'Z')
                        filename[i] = ((c - 'A' + 7) % 26) + 'A'; // Changed the decryption key
                    else if (c >= 'a' && c <= 'z')
                        filename[i] = ((c - 'a' + 7) % 26) + 'a'; // Changed the decryption key
                }

                char old_path[256];
                char new_path[256];
                sprintf(old_path, "%s/%s", dir_path, ent->d_name);
                sprintf(new_path, "%s/%s", dir_path, filename);

                // Renaming the file
                if (rename(old_path, new_path) != 0) { // Check if renaming is successful
                    perror("Error renaming file");
                }
            }
        }
        closedir(dir);
    } 
}

void delete() {
    // Open the current directory
    DIR* dir = opendir("library");

    struct dirent* entry;

    // Loop through each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry is a regular file
        if (entry->d_type == DT_REG) {
            // Check if the file name contains "d3Let3"
            if (strstr(entry->d_name, "d3Let3") != NULL) {
                // Delete the file
                remove(entry->d_name);
            }
        }
    }

    // Close the directory
    closedir(dir);
}

void rename2() {
    // Open the current directory
    DIR* dir = opendir("library");
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;

    // Loop through each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Check if the file name contains "r3N4mE"
        if (strstr(entry->d_name, "r3N4mE") != NULL) {
            // Construct full path
            char old_path[1024];
            char new_path[1024];
            snprintf(old_path, sizeof(old_path), "library/%s", entry->d_name);

            // Determine file extension
            char* ext = strrchr(entry->d_name, '.');
            if (ext != NULL) {
                // Check file extension and rename accordingly
                if (strcmp(ext, ".ts") == 0) {
                    snprintf(new_path, sizeof(new_path), "library/helper.ts");
                } else if (strcmp(ext, ".py") == 0) {
                    snprintf(new_path, sizeof(new_path), "library/calculator.py");
                } else if (strcmp(ext, ".go") == 0) {
                    snprintf(new_path, sizeof(new_path), "library/server.go");
                } else {
                    snprintf(new_path, sizeof(new_path), "library/renamed.file");
                }
                // Rename the file
                if (rename(old_path, new_path) != 0) {
                    perror("Error renaming file");
                }
            }
        }
    }
    closedir(dir);
}




int main() {
    pid_t pid_download, pid_unzip, pid_decrypt;
    
    pid_download = fork();
    
    if (pid_download == -1) {
        perror("Failed to fork download process");
        exit(EXIT_FAILURE);
    } else if (pid_download == 0) {
        download();
        return 0;
    } else {
        pid_unzip = fork();
        
        if (pid_unzip == -1) {
            perror("Failed to fork unzip process");
            exit(EXIT_FAILURE);
        } else if (pid_unzip == 0) {
            unzip();
            return 0;
        } else {
            int status;
            
            // Wait for both child processes to finish
            waitpid(pid_download, &status, 0);
            waitpid(pid_unzip, &status, 0);
            
            // Fork for decryption
            pid_decrypt = fork();
            if (pid_decrypt == -1) {
                perror("Failed to fork decryption process");
                exit(EXIT_FAILURE);
            } else if (pid_decrypt == 0) {
                decrypt_filenames("library");
                return 0;
            } else {
                // Wait for decryption to finish
                waitpid(pid_decrypt, &status, 0);
                
                // After decryption finishes, perform file operations
                delete();
                rename2();
            }
        }
    }
    
    return 0;
}
