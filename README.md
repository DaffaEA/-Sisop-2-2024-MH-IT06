# Sisop-1-2024-MH-IT06
# Laporan Resmi Praktikum Sistem Operasi Modul 1

Kelompok IT-06:

-Daffa Rajendra Priyatama 5027231009

-Nicholas Arya Krisnugroho Rerangin 5027231058

-Johanes Edward Nathanael 5027231067

# Pengantar 

# Soal 1

## Isi Soal
Gavriel adalah seorang cyber security enthusiast. Suatu hari, ia dikontrak oleh sebuah perusahaan ternama untuk membuat sebuah program yang cukup rumit dengan bayaran jutaan rupiah. Karena tergoda dengan nominal bayaran tersebut, Gavriel langsung menerima tawaran tersebut. Setelah mencoba membuat dan mengembangkan program tersebut selama seminggu, laptop yang digunakan Gavriel mengalami overheat dan mati total sehingga harus dilarikan ke tukang servis terdekat. Karena keterbatasan waktu dalam pembuatan program dan tidak ingin mengecewakan perusahaan, Gavriel meminta bantuan kalian untuk membuat program tersebut dengan ketentuan sebagai berikut:

a. Program dapat menerima input path berupa ‘argv’ untuk mengatur folder dimana file akan dieksekusi

b. Program tersebut berfungsi untuk mereplace string dengan ketentuan sebagai berikut:

  i. String m4LwAr3 direplace dengan string [MALWARE]
  
  ii. String 5pYw4R3 direplace dengan string [SPYWARE]
  
  iii. String R4nS0mWaR3 direplace dengan string [RANSOMWARE]
  
c. Program harus berjalan secara daemon, dan tidak diperbolehkan menggunakan command system() dalam pembuatan program

d. Program akan secara terus menerus berjalan di background dengan jeda 15 detik

e. Catat setiap penghapusan string yang dilakukan oleh program pada sebuah file bernama virus.log dengan format: [dd-mm-YYYY][HH:MM:SS] Suspicious string at <file_name> successfully replaced!

## Penyelesaian

### virus.c
#### Menerima Input Path untuk mengatur folder dimana file akan dieksekusi
```c

    char path[MAX_PATH_LENGTH];

    printf("Enter path to list files: ");
    scanf("%s", path);
```
- scanf menerima input dari user

#### Membuat dan membuka file virus.log
```c
 FILE *file = fopen("virus.log", "w");
    if (file == NULL) {
        perror("fopen failed");
        return 1;
    }
```
- Membuka virus.log di directory executable dan memberi pesan bila gagal

#### Inisialisasi Daemon
```c
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
```
- Menginisialisasi fungsi agar berjalan secara daemon

#### Menjalankan fungsi listFilesRecursively 
```c
while(1){
    listFilesRecursively(path);
}
```
- Menjalankan fungsi listFilesRecursively secara berulang tanpa henti

### listfilesRecursively
#### Membuka directory 
```bash
  char path[MAX_PATH_LENGTH];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if (!dir) {
        perror("opendir failed");
        return;
    }
```
- Membuka directory berdasar input pengguna

#### Membuka file
```c
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
```
- Menyalin Path file yang telah dibaca
- Melakukan Rekursi bila isi dari directory path juga sebuah directory
- Membuka file yang ditemukan

#### Membuka isi File
```c
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

```
- Membuka isi file sebagai buffer
- Menyimpan string yang ingin diganti dan penggantinya dengan array

#### Mengganti isi file dan menulis log 
```c
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
```
- Loop sebanyak jumlah jenis string yang ingin diganti
- Mengecek apabila ada string yang ingin diganti dengan strstr
- bila ada, mencari letak string yang ingin diganti dalam buffer dengan fseek
- Mengganti string dengan dengan string replacement
- Mengambil data waktu dari sistem
- Membuat string yang ingin dimasukkan ke dalam logfile
- Membuka logfile dan menulis string log pada logfile
- program berhenti selama 15 detik
- Menutup file dan Directory setelah semua selesai

### Hasil Output
#### Sebelum
![Screenshot 2024-04-26 005333](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/9a9476d3-a026-487b-8101-a2b0944c8ad3)

![Screenshot 2024-04-26 005347](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/741a7961-4067-4bef-87ae-86a787d04519)

#### Sesudah 
![Screenshot 2024-04-26 005845](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/b7c1443a-9edb-418b-bf1d-4adf18abbd9d)

![Screenshot 2024-04-26 005953](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/3c95b57b-4f52-43e9-8e5e-a874f22f220f)

![Screenshot 2024-04-26 010010](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/d96cb444-3505-478b-999e-d6d54c66931e)

### Revisi 
```c
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <path>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];
```
- Mengubah sistem input agar menggunakan argv sesuai soal

# Soal 2 

## Isi Soal
Paul adalah seorang mahasiswa semester 4 yang diterima magang di perusahaan XYZ. Pada hari pertama magang, ia diberi tugas oleh atasannya untuk membuat program manajemen file sederhana. Karena kurang terbiasa dengan bahasa C dan environment Linux, ia meminta bantuan kalian untuk mengembangkan program tersebut.

a. Atasannya meminta agar program tersebut dapat berjalan secara daemon dan dapat mengunduh serta melakukan unzip terhadap file berikut. Atasannya juga meminta program ini dibuat tanpa menggunakan command system()

b. Setelah ditelusuri, ternyata hanya 6 file teratas yang nama filenya tidak dienkripsi. Oleh karena itu, bantulah Paul untuk melakukan dekripsi terhadap nama file ke-7 hingga terakhir menggunakan algoritma ROT19

c. Setelah dekripsi selesai, akan terlihat bahwa setiap file memuat salah satu dari kode berikut: r3N4mE, d3Let3, dan m0V3. Untuk setiap file dengan nama yang memuat kode d3Let3, hapus file tersebut. Sementara itu, untuk setiap file dengan nama yang memuat kode r3N4mE, lakukan hal berikut:

i. Jika ekstensi file adalah “.ts”, rename filenya menjadi “helper.ts”

ii. Jika ekstensi file adalah “.py”, rename filenya menjadi “calculator.py”

iii. Jika ekstensi file adalah “.go”, rename filenya menjadi “server.go”

iv. Jika file tidak memuat salah satu dari ekstensi diatas, rename filenya menjadi “renamed.file”

d. Atasan Paul juga meminta agar program ini dapat membackup dan merestore file. Oleh karena itu, bantulah Paul untuk membuat program ini menjadi 3 mode, yaitu:

  i. default: program berjalan seperti biasa untuk me-rename dan menghapus file. Mode ini dieksekusi ketika program dijalankan tanpa argumen tambahan, yaitu dengan command ./management saja

  ii. backup: program memindahkan file dengan kode m0V3 ke sebuah folder bernama “backup”

  iii. restore: program mengembalikan file dengan kode m0V3 ke folder sebelum file tersebut dipindahkan

  iv. Contoh penggunaan: ./management -m backup

e. Terkadang, Paul perlu mengganti mode dari program ini tanpa menghentikannya terlebih dahulu. Oleh karena itu, bantulan Paul untuk mengintegrasikan kemampuan untuk mengganti mode ini dengan mengirim sinyal ke daemon, dengan ketentuan:

  i. SIGRTMIN untuk mode default

  ii. SIGUSR1 untuk mode backup

  iii. SIGUSR2 untuk mode restore
  
  iv. Contoh penggunaan: kill -SIGUSR2 <pid_program>

f. Program yang telah dibuat ini tidak mungkin akan dijalankan secara terus-menerus karena akan membebani sistem. Maka dari itu, bantulah Paul untuk membuat program ini dapat dimatikan dengan aman dan efisien

g. Terakhir, program ini harus berjalan setiap detik dan mampu mencatat setiap peristiwa yang terjadi ke dalam file .log yang bernama “history.log” dengan ketentuan:

  i. Format: [nama_user][HH:MM:SS] - <nama_file> - <action>

  ii. nama_user adalah username yang melakukan action terhadap file

  iii. Format action untuk setiap kode:
  
      1. kode r3N4mE: Successfully renamed.
      2. kode d3Let3: Successfully deleted.
      3. mode backup: Successfully moved to backup.
      4. mode restore: Successfully restored from backup.
      
   iv. Contoh pesan log:
  
      1. [paul][00:00:00] - r3N4mE.ts - Successfully renamed.
      2. [paul][00:00:00] - m0V3.xk1 - Successfully restored from backup.

h. Berikut adalah struktur folder untuk pengerjaan nomor 2:

    soal_2/
    ├── history.log
    ├── management.c
    └── library/
        └── backup/
        
## Penyelesaian
### management.c
#### Main Function
```c
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
```
- Menolak argumen yang tidak sesuai format
- Menjalankan Fungsi Download, Unzip, Menghapus library.zip, decrypt_filenames, delete_files, rename_files secara berurutan
- Mengecek argumen dan apabila argumen backup maka menjalankan fungsi backup
- Apabila argumen restore maka menjalankan fungsi restore
- exit apabila argumen tidak valid

#### Download dan Unzip
```c
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
```
- Membuat child process
- Menjalakan wget menggunakan execvp pada child process
- Menunggu hingga child process selesai

```c
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
```
- Membuat child process
- Menjalakan unzip menggunakan execvp pada child process
- Menunggu hingga child process selesai

#### Decrypt_filenames
```c
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
```
- Membuka directory
- Apabila isi dari directory adalah file maka menyalin nama dari file tersebut
- Dekripsi nama yang disalin dengan menggunakan algoritma ROT

```c
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
```
- Membuat nama path file baru berdasar hasil dekripsi
- Mengganti path lama dengan path baru dengan fungsi rename

#### delete_files
```c
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
```
- Membuka Directory
- Apabila isi directory adalah file maka membaca nama dari file tersebut
- Mengecek apabila nama mengandung string "d3Let3"
- Apabila ada maka menghapus file tersebut dengan fungsi remove

```c
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
```
- Membuka logfile
- Mengambil data waktu dari sistem
- Membuat string log berdasar ketentuan
- Memasukkan string log kepada logfile
- Menutup logfile
- Menutup Directory

#### rename_files
```c
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
```
- Membuka Directory
- Apabila isi directory adalah file maka membaca nama dari file tersebut
- Mengecek apabila nama mengandung string "r3N4mE"
- Apabila ada maka menyimpan path dari file tersebut

  ```c
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
  ```
  - Mengecek 3 huruf terakhir dari nama file tersebut
  - Apabila .ts maka menyimpan nama file baru sebagai "helper.ts"
  - Apabila .py maka menyimpan nama file baru sebagai "calculator.py"
  - Apabila .go maka menyimpan nama file baru sebagai "server.go"
  - Apabila yang lain maka menyimpan nama file baru sebagai "renamed.file"
  - Menyimpan path dengan nama file baru

```c
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
```
- Merubah path lama menjadi path baru
- Membuka logfile
- Mengambil data waktu dari sistem
- Membuat string log berdasar ketentuan
- Memasukkan string log kepada logfile
- Menutup logfile
- Menutup Directory

#### backup
```c
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
```
- Membuat directory bernama backup
- Mengecek apabila directory telah ada atau belum

```c
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
```
- Membuka directory
- Mengecek apabila nama file mengandung string "m0V3"
- Menyalin path file
- Membuat path file baru dengan tujuan menambah "/backup" diantara library dan /namafile
- memindah file dengan rename filepath 

```c
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
```
- Membuka logfile
- Mengambil data waktu dari sistem
- Membuat string log berdasar ketentuan
- Memasukkan string log kepada logfile
- Menutup logfile
- Menutup Directory

#### restore
```c
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
```
- Membuka directory
- Mengecek apabila nama file mengandung string "m0V3"
- Menyalin path file
- Membuat path file baru dengan tujuan menghapus "/backup" diantara library dan /namafile
- Apabila ada, maka memindah file dengan rename filepath 

```c
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
```
- Membuka logfile
- Mengambil data waktu dari sistem
- Membuat string log berdasar ketentuan
- Memasukkan string log kepada logfile
- Menutup logfile
- Menutup Directory

### Hasil Output 
#### Tanpa argumen
![Screenshot 2024-04-26 025751](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/ca397024-f857-4982-919a-d59716e6d406)

![Screenshot 2024-04-26 025851](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/623ab950-5c8e-42e3-8d6a-876e450524ac)

#### Mode Backup
![Screenshot 2024-04-26 031838](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/00ebd330-9362-4f9b-8f4e-f060ccf228dd)

![Screenshot 2024-04-26 031853](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/d3561d4e-4d5d-4d63-91e0-77f0ea697dc9)

![Screenshot 2024-04-26 031907](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/3f238592-4f3f-4a4c-a83a-4f7f68ee43ea)

#### Mode Restore
![Screenshot 2024-04-26 032122](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/08f5db0b-bc87-4e64-8543-f0b4b5047918)

![Screenshot 2024-04-26 032214](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/41248c1d-cd46-49aa-ad43-aaeea7c23826)

![Screenshot 2024-04-26 032453](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/132379720/4d7c543e-e85f-410f-8442-8987f0c26397)

# Soal 3 

# Isi Soal 
Pak Heze adalah seorang admin yang baik. Beliau ingin membuat sebuah program admin yang dapat memantau para pengguna sistemnya. Bantulah Pak Heze untuk membuat program  tersebut!

a. Nama program tersebut dengan nama admin.c

b. Program tersebut memiliki fitur menampilkan seluruh proses yang dilakukan oleh seorang user dengan menggunakan command:
./admin <user>

c. Program dapat memantau proses apa saja yang dilakukan oleh user. Fitur ini membuat program berjalan secara daemon dan berjalan terus menerus. Untuk menjalankan fitur ini menggunakan command: 
./admin -m <user>
Dan untuk mematikan fitur tersebut menggunakan: 
./admin -s <user>
Program akan mencatat seluruh proses yang dijalankan oleh user di file <user>.log dengan format:
[dd:mm:yyyy]-[hh:mm:ss]_pid-process_nama-process_GAGAL/JALAN

d. Program dapat menggagalkan proses yang dijalankan user setiap detik secara terus menerus dengan menjalankan: 
./admin -c user
sehingga user tidak bisa menjalankan proses yang dia inginkan dengan baik. Fitur ini dapat dimatikan dengan command:
./admin -a user

e. Ketika proses yang dijalankan user digagalkan, program juga akan melog dan menset log tersebut sebagai GAGAL. Dan jika di log menggunakan fitur poin c, log akan ditulis dengan JALAN

## Penyelesaian

### Fungsi monitor_processes(const char *user)

```c
void monitor_processes(const char *user) {
    char logfile[50];
    sprintf(logfile, "%s.log", user);

    while (1) {
        // Mengambil waktu saat ini
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        
        // Membuat timestamp dalam format tertentu
        char timestamp[50];
        strftime(timestamp, sizeof(timestamp), "[%d:%m:%Y]-[%H:%M:%S]", timeinfo);

        // Membuka file log
        FILE *log_fp = fopen(logfile, "a");
        if (log_fp == NULL) {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }
        // Menulis timestamp ke dalam file log
        fprintf(log_fp, "%s\n", timestamp);
        fclose(log_fp);

        // Membuat proses baru untuk mengeksekusi perintah "ps"
        pid_t pid = fork();
        if (pid == -1) {
            perror("Error forking");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Proses anak: mengalihkan output ke file log
            int fd = open(logfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                perror("Error opening log file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            // Mengeksekusi perintah "ps -u user"
            execlp("ps", "ps", "-u", user, NULL);
            perror("Error executing command");
            exit(EXIT_FAILURE);
        } else {
            // Proses induk: menunggu proses anak selesai
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                perror("Child process failed");
                exit(EXIT_FAILURE);
            }
            // Menampilkan output "ps" ke terminal
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

        // Menunda pemantauan selama 60 detik sebelum iterasi berikutnya
        sleep(60);
    }
}
```
Fungsi ini bertanggung jawab untuk memantau proses yang sedang berjalan oleh pengguna tertentu. Setiap menit, fungsi ini mencatat timestamp ke dalam file log, mengeksekusi perintah "ps -u user" untuk mendapatkan daftar proses pengguna, dan menulis outputnya ke terminal serta ke dalam file log.

### Fungsi stop_monitoring(const char *user)

```c
void stop_monitoring(const char *user) {
    printf("Monitoring stopped for user %s.\n", user);
}
```
Fungsi ini bertanggung jawab untuk memberhentikan pemantauan proses untuk pengguna tertentu. Ini hanya mencetak pesan bahwa pemantauan telah dihentikan untuk pengguna tersebut.

### Fungsi 'main(int argc, char *argv[])'

```c
int main(int argc, char *argv[]) {
    // Memeriksa jumlah dan isi argumen
    if (argc != 3 || (strcmp(argv[1], "-m") != 0 && strcmp(argv[1], "-s") != 0)) {
        printf("Usage: ./admin <-m/-s> <user>\n");
        return EXIT_FAILURE;
    }

    // Memilih mode operasi berdasarkan argumen pertama
    if (strcmp(argv[1], "-m") == 0) {
        // Mode pemantauan: panggil fungsi monitor_processes dengan nama pengguna
        monitor_processes(argv[2]);
    } else {
        // Mode berhenti: panggil fungsi stop_monitoring dengan nama pengguna
        stop_monitoring(argv[2]);
    }

    return EXIT_SUCCESS;
}
```
Fungsi utama ini menerima argumen dari baris perintah dan memeriksa apakah argumen yang diberikan sesuai dengan format yang diharapkan. Jika tidak, program menampilkan pesan penggunaan dan keluar dengan status gagal. Jika formatnya benar, fungsi ini memilih mode operasi berdasarkan argumen pertama dan memanggil fungsi yang sesuai.

### Hasil Output

Menjalankan program dengan command ./admin -m <user> dan menghentikan program dengan command ./admin -s <user> sudah berhasil

![Screenshot from 2024-04-27 17-04-42](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/144967723/dc5e2174-ce1f-492e-98cc-cdadeae29de6)

![Screenshot from 2024-04-27 17-04-51](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/144967723/c28047f7-64f7-44de-a9fe-a152c06c1d2c)

![Screenshot from 2024-04-27 17-05-08](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/144967723/eca4a6ff-f033-4f45-ad17-591cda4f506f)

![Screenshot from 2024-04-27 17-05-14](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/144967723/ebd1bd6d-b610-44db-a10f-8003f1844e87)

File .log sudah terbuat di folder

![Screenshot from 2024-04-27 17-05-28](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/144967723/bccd8fd1-eada-4384-9a39-e7f33217ca29)

Isi file .log

![Screenshot from 2024-04-27 17-06-01](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/144967723/ae4e7ba0-0659-44ee-bdce-b67089f645ad)

![Screenshot from 2024-04-27 17-05-52](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/144967723/4ad4a7ad-9b84-4bc1-b22c-3c490eb0d71c)


# Soal 4 

## Isi Soal
Salomo memiliki passion yang sangat dalam di bidang sistem operasi. Saat ini, dia ingin mengotomasi kegiatan-kegiatan yang ia lakukan agar dapat bekerja secara efisien. Bantulah Salomo untuk membuat program yang dapat mengotomasi kegiatan dia!
(NB: Soal di uji coba dan berhasil di sistem yang menggunakan MacOS dan linux yang menggunakan dual boot dan VM. Untuk teman-teman yang menggunakan WSL bisa mengerjakan soal yang lain ya)

Salomo ingin dapat membuka berbagai macam aplikasi dengan banyak jendela aplikasi dalam satu command. Namai file program tersebut setup.c

Program dapat membuka berbagai macam aplikasi dan banyak jendela aplikasi sekaligus (bisa membuka 1 atau lebih aplikasi dengan 1 atau lebih window (kecuali aplikasi yang tidak bisa dibuka dengan banyak window seperti discord)) dengan menjalankan: 
./setup -o <app1> <num1> <app2> <num2>.....<appN> <numN>
Contoh penggunaannya adalah sebagai berikut: 
./setup -o firefox 2 wireshark 2
Program akan membuka 2 jendela aplikasi firefox dan 2 jendela aplikasi wireshark.

Program juga dapat membuka aplikasi dengan menggunakan file konfigurasi dengan menggunakan 
./setup -f file.conf 
Format file konfigurasi dibebaskan, namun pastikan dapat menjalankan fitur dari poin 2.
Contoh isi file.conf:
Firefox 2
Wireshark 3

Ketika menjalankan command contoh, program akan membuka 2 jendela aplikasi firefox dan 3 jendela aplikasi wireshark.
Program dapat mematikan semua aplikasi yg dijalankan oleh program tersebut dengan: 
./setup -k

Program juga dapat mematikan aplikasi yang dijalankan sesuai dengan file konfigurasi. 
Contohnya: 
./setup -k file.conf 
Command ini hanya mematikan aplikasi yang dijalankan dengan 
./setup -f file.conf

## Penyelesaian // setup.c

```c
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
```
Pada kode tersebut ada pengatur untuk argumen-argumen berikut:
-  Argumen `-o` yang digunakan untuk open program berdasarkan parameter yang diketikan setelahnya, contoh `./setup -o wireshark 2 firefox 2`
-  Argumen `-k` yang digunakan untuk mengekill semua program yang dijalankan menggunakan `./setup -k`
-  Argumen `-f` yang digunakan untuk open program berdasarkan file config yang kita ketikkan setelahnya, contoh `./setup -f file.conf`
-  Argumen `-k` juga dapat diimplementasikan untuk melakukan kill sesuai file conf dengan cara `./setup -k file.conf`

### Proses membuka Aplikasi 
Dimulai dengan membuka file untuk mendapatkan PID yang dijalankan // sudah ada
```c
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
```
Setelah dibuat filenya, Proses buka App memiliki fungsi untuk menambahkan PID ke file txt yang telah dibuat dan menambahkan serta melakukan read program-program ke dalam file txtnya

```c
void addRunningApp(pid_t pid) {
    if (num_running_apps < MAX_APPS) {
        running_apps[num_running_apps++] = pid;
        printf("Jumlah aplikasi berjalan: %d\n", num_running_apps);
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
```
### Proses melakukan Kill Apps
Dalam proses melakukan kill appnya saya  menggunakan pkill ketika ingin kill berdasarkan file.conf
Jika tidak spesifik terhadap file, maka saya melakukan kill berdasarkan apa yang ada pada file txtnya
```c
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
```

### Dokumentasi // Hasil
![image](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/142997842/3ca70fa8-e040-464c-b4e6-d61fbab3a356)
![image](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/142997842/461d2ebf-83c9-43b9-9a45-9cb7e017b6e2)

### Revisi 
Revisi dilakukan dalam memperbaiki function untuk membuka app melalui file.conf dan melakukan kill terhadap app yang dijalankan melalui file.conf
![image](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/142997842/6c092344-6f74-4ab1-9f9e-bba94b49ce92)
![image](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/142997842/c2eb6048-eadf-418e-ad61-076d8ad21509)
![image](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/142997842/2d0c5cf1-3f15-4bbb-b3ee-d715fa8812ff)
![image](https://github.com/DaffaEA/Sisop-2-2024-MH-IT06/assets/142997842/5ac0e407-b77a-41d1-9e55-07bb07fa8546)
Setelah di cek dengan `ps aux` terlihat bahwa sudah tidak ada program yang running

