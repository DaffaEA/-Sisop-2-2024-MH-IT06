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
