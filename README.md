# soal-shift-sisop-modul-3-IT01-2021
### Nama Anggota Kelompok:
1. Widya Inayatul Luthfiyah (05311940000010)
2. Muhammad Rizqi Wijaya (05311940000014)
3. Gavin Bagus Kanzie N (05311940000028)
---
## Soal Nomor 1
Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. 

Di dalam proyek itu, Keverk diminta: 

a.	Pada saat client tersambung dengan server, terdapat dua pilihan pertama, yaitu register dan login. Jika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server. User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Keverk menginginkan lokasi penyimpanan id dan password pada file bernama 

akun.txt dengan format :

>id:password
>
>id2:password2


b.	Sistem memiliki sebuah database yang bernama files.tsv. Isi dari files.tsv ini adalah path file saat berada di server, publisher, dan tahun publikasi. Setiap penambahan dan penghapusan file pada folder file yang bernama  FILES pada server akan memengaruhi isi dari files.tsv. Folder FILES otomatis dibuat saat server dijalankan. 

c.	Tidak hanya itu, Keverk juga diminta membuat fitur agar client dapat menambah file baru ke dalam server. Direktori FILES memiliki struktur direktori di bawah ini : 

DIREKTORI FILES
>File1.ekstensi
>
>File2.ekstensi

Pertama client mengirimkan input ke server dengan struktur sebagai berikut :

Contoh Command Client :
>add

Output Client Console:
>Publisher:
>
>Tahun Publikasi:
>
>Filepath:

dari aplikasi client akan dimasukan data buku tersebut (perlu diingat bahwa Filepath ini merupakan path file yang akan dikirim ke server). Lalu client nanti akan melakukan pengiriman file ke aplikasi server dengan menggunakan socket. Ketika file diterima di server, maka row dari files.tsv akan bertambah sesuai dengan data terbaru yang ditambahkan.


d.	Dan client dapat mendownload file yang telah ada dalam folder FILES di server, sehingga sistem harus dapat mengirim file ke client. Server harus melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di folder client tersebut. 

Contoh Command client
>download TEMPfile.pdf

e.	Setelah itu, client juga dapat menghapus file yang tersimpan di server. Akan tetapi, Keverk takut file yang dibuang adalah file yang penting, maka file hanya akan diganti namanya menjadi ‘old-NamaFile.ekstensi’. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus.

Contoh Command Client:
>delete TEMPfile.pdf

f.	Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang bernama see. Output dari perintah tersebut keluar dengan format. 


Contoh Command Client :
>see

Contoh Format Output pada Client:
>Nama:
>
>Publisher:
>
>Tahun publishing:
>
>Ekstensi File : 
>
>Filepath : 

>Nama:
>
>Publisher:
>
>Tahun publishing:
>
>Ekstensi File : 
>
>Filepath : 

g.Aplikasi client juga dapat melakukan pencarian dengan memberikan suatu string. Hasilnya adalah semua nama file yang mengandung string tersebut. Format output seperti format output f.

Contoh Client Command:
>find TEMP

h.	Dikarenakan Keverk waspada dengan pertambahan dan penghapusan file di server, maka Keverk membuat suatu log untuk server yang bernama running.log. Contoh isi dari log ini adalah

running log
>Tambah : File1.ektensi (id:pass)
>
>Hapus : File2.ektensi (id:pass)

Note: 
-	Dilarang menggunakan system() dan execv(). Silahkan dikerjakan sepenuhnya dengan thread dan socket programming. 
-	Untuk download dan upload silahkan menggunakan file teks dengan ekstensi dan isi bebas (yang ada isinya bukan touch saja dan tidak kosong) dan requirement untuk benar adalah percobaan dengan minimum 5 data.

Struktur Direktori

>     ├── Client
>
>     |    ├── client.c
>
>     |    ├── File2.extensi
>
>     |    └── File1.extensi
>
>     |── Server
>
>             ├── akun.txt
>
>             ├── files.tsv
>
>             ├── server.c
>
>             ├── running.log
>
>             └── FILES
>
>                 ├── File2.extensi
>
>                 └── File1.ekstensi

### Penjelasan Program :
#### Client
Source Code [client soal 1 ](https://github.com/rizwijaya/soal-shift-sisop-modul-3-IT01-2021/blob/main/soal1/Client/client.c)

#### Library
Berikut adalah library yang digunakan untuk menyelesaikan soal ini:

```#include <stdio.h>``` = untuk standard input-output

```#include <sys/socket.h>``` = untuk menjalankan socket

```#include <stdlib.h>``` = untuk fungsi umum

```#include <netinet/in.h>``` = untuk alamat domain internet

```#include <string.h>``` = untuk melakukan manipulasi string, misalnya ```strcmp()```

```#include <errno.h>``` = untuk memberikan tambahan error pada sistem

```#include <unistd.h>``` = untuk melakukan system call ```fork()```

```#include <arpa/inet.h>``` = untuk operasi internet

```#include <pthread.h>``` = untuk bisa menjalankan file c

```#include <drent.h>``` = untuk operasi akses direktori

```#include <ctype.h>``` = untuk mendeklarasikan serangkaian fungsi dan mengklasifikasikan karakter

```#include <string.h>``` = untuk melakukan manipulasi string, misalnya ```strcmp()```

```
#define MAX_LENGTH 1024
#define PORT 8080
#define CLF "./"

pthread_t input, cetak;

struct user
{
  int is_auth;
  int socket;
  char file[1000];
  char input[1000];
  char mode[1000];
} user_data;
```
- Mendefinisikan port server
- Membuat struct untuk user 

```

int fileExist(char *fname)
{
  int found = 0;
  DIR *di;
  struct dirent *dir;
  di = opendir(CLF);
  while ((dir = readdir(di)) != NULL)
  {
    if (strcmp(dir->d_name, fname) == 0)
    {
      found = 1;
      break;
    }
  }
  closedir(di);
  return found;
}
```
- Fungsi untuk mengecek apakah terdapat file
- ```DIR *di``` untuk membuka path ```struct dirent *dir``` untuk entri direktori while dimana kita dapat membaca sesuatu dari direktori. Jika d_name adalah fname lalu break. apabila tidak ada masalah tutup direktori mengembalikan found
 
 
```

void *user_cetak(void *arg)
{ 
  if (strcmp(user_data.mode, "recvstrings") == 0)
  {
    int sock = *(int *)arg;
    char buffer[1024] = {0};
    while (1)
    {
      memset(buffer, 0, 1024);
      if (recv(sock, buffer, 1024, 0) > 1)
      {
        char buffer2[1024];
        strcpy(buffer2, buffer);
        char *token = strtok(buffer2, "\n");
        printf("%s", buffer);
      }
    }
  }
}
```

- ```void *user_cetak``` untuk cetak input user. Variabel sock untuk menyimpan nilai deskriptor file socket server.

```
int send_file(int socket, char *fname)
{
  char buffer[MAX_LENGTH] = {0};
  char fpath[MAX_LENGTH];
  strcpy(fpath, CLF);
  strcat(fpath, fname);
  FILE *file = fopen(fpath, "r");
  if (file == NULL)
  {
    printf("File %s tidak ada.\n", fname);
    return -1;
  }
  bzero(buffer, MAX_LENGTH);
  int file_size;
  while ((file_size = fread(buffer, sizeof(char), MAX_LENGTH, file)) > 0)
  {
    if (send(socket, buffer, file_size, 0) < 0)
    {
      fprintf(stderr, "Gagal mengirimkan file %s. (errno = %d)\n", fname, errno);
      break;
    }
    bzero(buffer, MAX_LENGTH);
  }
  fclose(file);
  return 0;
}
```

- ```int send_file``` fungsi untuk mengirimkan file. file akan dibuka jika file == NULL akan menampilkan file tidak ada. Lalu akan dilakukan pengecekan pengiriman jika terjadi kesalahan akan menampilkan gagal mengirimkan file.

```
int receive_file(int socket, char *fname)
{
  pthread_cancel(cetak);
  char buffer[MAX_LENGTH] = {0};
  char fpath[MAX_LENGTH];
  strcpy(fpath, CLF);
  strcat(fpath, fname);
  FILE *file_masuk = fopen(fpath, "wb");
  if (file_masuk == NULL)
  {
    printf("File %s, Tidak dapat dibuat di client.\n", fname);
  }
  else
  {
    bzero(buffer, MAX_LENGTH);
    int file_size = 0;
    while ((file_size = recv(socket, buffer, MAX_LENGTH, 0)) > 0)
    {
      int write_size = fwrite(buffer, sizeof(char), file_size, file_masuk);
      if (write_size < file_size)
      {
        error("Gagal menulis file.");
      }
      bzero(buffer, MAX_LENGTH);
      if (file_size == 0 || file_size != MAX_LENGTH)
      {
        break;
      }
    }
    if (file_size < 0)
    {
      if (errno == EAGAIN)
      {
        printf("Waktu habis.\n");
      }
      else
      {
        fprintf(stderr, "gagal = %d\n", errno);
        exit(1);
      }
    }
    printf("Mendownload file dari server!\n");
  }
  fclose(file_masuk);
  printf("File %s berhasil didownloads!\n", user_data.file);
  strcpy(user_data.mode, "recvstrings"); //Set mode ke input strings
  pthread_create(&cetak, NULL, &user_cetak, (void *)&user_data.socket);
}
```

- ```int receive_file``` fungsi untuk menerima file 
- file masuk akan dibuka dengan perintah write byte lalu jika file tidak ada maka akan menampilakan File, Tidak dapat dibuat di client. Dimana ukuran file sudah didefinisikan jika tidak sesuai maka akan menampilkan gagal menulis file dan dilakukan pengecekan jika ukuran file sudah sesuai maka file akan didownload dari server apabila sudah didownload akan menampilkan file berhasil didownloads dan file akan ditutup.

```
void *user_input(void *arg)
{
  while (strcmp(user_data.mode, "recvstrings") == 0)
  {
    char buffer[1024] = {0};
    bzero(buffer, MAX_LENGTH);
    fgets(buffer, MAX_LENGTH, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    send(user_data.socket, buffer, MAX_LENGTH, 0);

    char cmd_line[MAX_LENGTH];
    strcpy(cmd_line, buffer);
    char *cmd = strtok(cmd_line, " "); // Split input

    for (int i = 0; cmd[i]; i++)
    { 
      cmd[i] = tolower(cmd[i]);
    }

    if (strcmp("add", cmd) == 0)
    { // Jika terdapat argument add
      strcpy(user_data.mode, "recvimage");
      char *fname;
      cmd = strtok(NULL, " "); //Split input
      fname = cmd;
      strcpy(user_data.file, fname);
      if (!fileExist(fname))
      { //Jika tidak ada filenya
        printf("File %s tidak ada.\n", fname);
        continue;
      }
      // send command to server
      if (send_file(user_data.socket, fname) == 0)
      {
        printf("File berhasil dikirimkan!!\n");
        strcpy(user_data.mode, "recvstrings");
      }
      else
      {
        printf("File gagal dikirimkan\n");
        strcpy(user_data.mode, "recvstrings");
      }
    }
    else if (strcmp("download", cmd) == 0)
    { //Jika terdapat argument download
      strcpy(user_data.mode, "downimage");
      char *fname;
      cmd = strtok(NULL, " ");
      fname = cmd;
      strcpy(user_data.file, fname);
      download(user_data.socket, fname);
    }
  }
  if (strcmp(user_data.mode, "recvimage") == 0)
  {
    // send command to server
    if (send_file(user_data.socket, user_data.file) == 0)
    {
      printf("File berhasil dikirimkan!!\n");
      strcpy(user_data.mode, "recvstrings");
    }
    else
    {
      printf("File gagal dikirimkan\n");
      strcpy(user_data.mode, "recvstrings");
    }
  }
}

```

- ```void *user_input``` fungsi untuk inputan user dimana Client akan menginput pilihan login dan register. lalu client akan menginputkan id dan passwordnya. Dari data tersebut nantinya akan dikirim ke server. Jika sudah register maka client dapat login. Lalu client dapat menginputkan command add untuk menambahkan file jika file tidak ada akan menampilkan file tidak ada. ```send(user_data.socket, buffer, MAX_LENGTH, 0);``` untuk mengirimkan input ke server. ```cmd[i] = tolower(cmd[i]);``` untuk merubah input ke lowecase.

```
int main(int argc, char const *argv[])
{
  struct sockaddr_in address;
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }
  else
  {
    user_data.socket = sock;
    printf("Terhubung dengan server beralamat %d\n", sock);
  }
  strcpy(user_data.mode, "recvstrings"); //Set mode ke strings

  pthread_create(&cetak, NULL, &user_cetak, (void *)&sock);
  pthread_create(&input, NULL, &user_input, (void *)&sock);
  while (1)
  { //Agar terus terhubung dengan server
    if (pthread_join(input, NULL) == 0)
    {
      pthread_create(&input, NULL, &user_input, (void *)&sock);
    }
  }
  if (strcmp(user_data.mode, "recvstrings") == 0)
  {
    pthread_join(cetak, NULL);
  }
  else
  {
    pthread_exit(&cetak);
  }
}

```

- Pada fungsi main, dalam fungsi main menggunakan parameter. parameter pertama bertipe data int (argc). Parameter argc berfungsi untuk menunjukkan banyaknya parameter yang digunakan dalam eksekusi program dan yang kedua bertipe data char consts *argv[]). Parameter argv yang akan menyimpan parameter - parameter yang digunakan dalam eksekusi program.
- Variabel ```serv_addr``` memiliki beberapa field. Field ```sin_family``` harus diset menjadi konstanta simbolic ```AF_INET```. Pada field ```sin_port``` harus diisikan dengan port. Agar portabilitas program terjamin maka digunakan fungsi ```htons()``` untuk mengubah format biner komputer host ke standar network dalam jenis bilangan short. Fungsi ini akan menyesuaikan byte data sehingga tidak terjadi kesalahan membaca antara host maupun jaringan.
- Pada fungsi ```inet_pton()``` mengubah alamat IP yang dapat dibaca menjadi format IPv6 32bit atau 128bit IPv6 yang dikemas. Disini akan melakukan pengecekan error jika terjadi kesalahan pada alamat maka akan menampilkan Invalid address/ Address not supported. 
- Pada fungsi ```connect()``` akan menghubungkan proses client terhubung dengan server. fungsi ini akan mngembalikan nilai berupa file deskriptor file baru. Argumen kedua adalah penunjuk alamat client dari koneksi. Dan argumen ketiga adalah ukuran dari server_addr. Disini akan melakukan pengecekan error jika terjadi kesalahan pada alamat maka akan menampilkan Connection Failed.

#### Server
Source Code [Server soal 1 ](https://github.com/rizwijaya/soal-shift-sisop-modul-3-IT01-2021/blob/main/soal1/Server/server.c)

#### Library
Berikut adalah library yang digunakan untuk menyelesaikan soal ini:

```#include <stdio.h>``` = untuk standard input-output

```#include <sys/socket.h>``` = untuk menjalankan socket

```#include <stdlib.h>``` = untuk fungsi umum

```#include <netinet/in.h>``` = untuk alamat domain internet

```#include <string.h>``` = untuk melakukan manipulasi string, misalnya ```strcmp()```

```#include <errno.h>``` = untuk memberikan tambahan error pada sistem

```#include <unistd.h>``` = untuk melakukan system call ```fork()```

```#include <arpa/inet.h>``` = untuk operasi internet

```#include <pthread.h>``` = untuk bisa menjalankan file c

```#include <drent.h>``` = untuk operasi akses direktori

```#include <ctype.h>``` = untuk mendeklarasikan serangkaian fungsi dan mengklasifikasikan karakter

```#include <string.h>``` = untuk melakukan manipulasi string, misalnya ```strcmp()```

```#include <time.h>``` = untuk manipulasi date and time

```
#define PORT 8080
#define MAX_LENGTH 1000
#define SVR "./FILES/"

pthread_t input, received;

void *input_main();
int error(char *err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

struct user
{
    char name[1000];
    char pwd[1000];
    char file[1000];
    char mode[1000];
    int is_auth;
    int socket;
} user_data;
```
- Mendefinisikan port server
- Membuat struct user yang terdiri atas nama ,password user, file

```
void message(char input[])
{
    char buffer[1024];
    sprintf(buffer, "\n%s\n", input);
    send(user_data.socket, buffer, 1024, 0);
```
- Fungsi untuk mengirimkan pesan ke client

```
int login(char id[], char password[])
{
    FILE *fp = fopen("akun.txt", "r"); //Buka file akun
    int is_auth = 0;
    char buffer[1024];
    while (fgets(buffer, 1024, fp) != NULL && is_auth == 0)
    {
        char file_id[1024], file_password[1024];
        //Memisahkan id dan password pada file
        char *token = strtok(buffer, ":");
        strcpy(file_id, token);
        token = strtok(NULL, "\n");
        strcpy(file_password, token);

        //Jika id dan password sesuai, maka return ke 1
        if (strcmp(id, file_id) == 0 && strcmp(password, file_password) == 0)
        {
            is_auth = 1;
        }
        else
        {
            is_auth = 0;
        }
    }
    fclose(fp);
    return is_auth;
}
```
- Fungsi untuk melakukan login
- file akun.txt akan dibuka dan dibaca disimpan dalam buffer ukr 1024 karakter. Lalu id dan password pada file akan dipisahkan. Jika id dan password sudah sesuai maka return ke 1.

```
void regist(char id[], char password[])
{
    FILE *fp = fopen("akun.txt", "a"); //Menulis id dan password di akun.txt
    fprintf(fp, "%s:%s\n", id, password);
    fclose(fp);
}
```
- ```void regist``` fungsi untuk emlakukan register
- File akun.txt akan dibuka lalu client akan menuliskan id dan password didalam file tersebut. Jika sudah maka akan mencetak id dan password

```
int addtsv(int year)
{
    FILE *fp;
    fp = fopen("files.tsv", "a+");
    if (fp == NULL)
    {
        printf("File tidak dapat dibuka");
        return 1;
    }
    fprintf(fp, "\nFILES/%s\t%s\t%d", user_data.file, user_data.name, year);
    fclose(fp);
    return 0;
}

void catatlog(char argument[])
{ 
    FILE *fp;
    fp = fopen("running.log", "a+");
    fprintf(fp, argument);
    fclose(fp);
}
```
- ```int addtsv``` fungsi untuk tambahkan file tsv
- file "files.tsv akan dibuka ```a+``` terbuka untuk membaca dan menulis. jika ```fp == NULL``` maka file tidak dapat dibuka.
- ```void catatlog```  fungsi untuk menceta log, pada running.log akan mencatat command add

```
int receive_file(int socket, char *fname)
{
    char buffer[MAX_LENGTH] = {0};
    char fpath[MAX_LENGTH];
    strcpy(fpath, SVR);
    strcat(fpath, fname);
    FILE *file_masuk = fopen(fpath, "wb");
    if (file_masuk == NULL)
    {
        printf("File %s, Tidak dapat dibuat di server.\n", fname);
    }
    else
    {
        bzero(buffer, MAX_LENGTH);
        int file_size = 0;
        while ((file_size = recv(socket, buffer, MAX_LENGTH, 0)) > 0)
        {
            int write_size = fwrite(buffer, sizeof(char), file_size, file_masuk);
            if (write_size < file_size)
            {
                error("Gagal menulis file diserver.");
            }
            bzero(buffer, MAX_LENGTH);
            if (file_size == 0 || file_size != MAX_LENGTH)
            {
                break;
            }
        }
        if (file_size < 0)
        {
            if (errno == EAGAIN)
            {
                printf("Waktu habis.\n");
            }
            else
            {
                fprintf(stderr, "gagal = %d\n", errno);
                exit(1);
            }
        }
        printf("Menerima file dari client!\n");
    }
    fclose(file_masuk);
    printf("File %s dari client berhasil diterima!\n", user_data.file);
    //Tampilkan output ke client
    char msg[1000], publisher[1000], path[1000], log[1000];
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    int year = local->tm_year + 1900;

    strcpy(publisher, user_data.name);
    strcpy(path, user_data.file);
    sprintf(msg, "Publisher: %s\nTahun Publikasi: %d\nFilepath: Client/%s", publisher, year, path);
    message(msg);
    addtsv(year); //Fungsi tambahkan ke file tsv
    sprintf(log, "Tambah : %s (%s:%s)\n", user_data.file, user_data.name, user_data.pwd);
    catatlog(log);                         //Tambahkan log
    strcpy(user_data.mode, "recvstrings"); //Set mode ke input strings
}
```

- ```receive_file``` fungsi untuk menerima file. Server akan menerima file, ukuran file, dan isi dari file tersebut. Dimana akan dilakukan pengecekan jika file gagal ditulis maka akan error ggal menulis file pada server. Jika errno == EAGAIN ```EAGAIN``` yang berarti tidak ada data yang tersedia maka akan menampilkan waktu habis. Apabila berhasil maka akan server akan menerima file dan menampilkan menerima file dari client dan file ditutup.
- output akan ditampilkan ke client berupa Nama, Publisher, Tahun publishing, Ekstensi File, Filepath.

```
int fileExist(char *fname)
{
    int found = 0;
    DIR *di;
    struct dirent *dir;
    di = opendir(SVR);
    while ((dir = readdir(di)) != NULL)
    {
        if (strcmp(dir->d_name, fname) == 0)
        {
            found = 1;
            break;
        }
    }
    closedir(di);
    return found;
}

int send_file(int socket, char *fname)
{
    char buffer[MAX_LENGTH] = {0};
    char fpath[MAX_LENGTH];
    strcpy(fpath, SVR);
    strcat(fpath, fname);
    FILE *file = fopen(fpath, "r");
    if (file == NULL)
    {
        printf("File %s tidak ada.\n", fname);
        return -1;
    }
    bzero(buffer, MAX_LENGTH);
    int file_size;
    while ((file_size = fread(buffer, sizeof(char), MAX_LENGTH, file)) > 0)
    {
        if (send(socket, buffer, file_size, 0) < 0)
        {
            fprintf(stderr, "Gagal mengirimkan file %s. (errno = %d)\n", fname, errno);
            break;
        }
        bzero(buffer, MAX_LENGTH);
    }
    fclose(file);
    return 0;
}
```
- ```int fileExist``` fungsi untuk mengecek apakah dile ada pada server.
- ```int send_file``` fungsi untuk mengirim file. File akan dibuka ```r``` yang artinya buka file teks untuk membaca. Jika ```file == NULL``` maka akan menampilkan fiel tidak ada.
- menginisiasi variabel buffer dengan fungsi bzero(). Variabel buffer akan mengambil data dari socket dengan fungsi fread. Selanjutnya akan dilakukan pengecekan fungsi send akan mengirimkan file melalui socket. Jika tidak sesuai maka gagal mengirimkan file.

```
void download(int accept_sockfd, char *fname)
{
    char buffer[MAX_LENGTH];
    if (fileExist(fname))
    {
        //Mengirimkan message keserver dan client
        printf("File %s Terdapat diserver\n", fname);
        char msg[1024];
        //Mengirimkan files ke client
        if (send_file(accept_sockfd, fname) == 0)
        {
            printf("File berhasil dikirimkan!!\n");
            strcpy(user_data.mode, "recvstrings");
        }
        else
        {
            printf("File gagal dikirimkan\n");
            strcpy(user_data.mode, "recvstrings");
        }
    }
    else
    {
        char msg[1024];
        sprintf(msg, "File %s tidak ada diserver", fname);
        message(msg);
        strcpy(user_data.mode, "recvstrings");
    }
}
```

- ```void download``` fungsi untuk mendownload files
- jika file sudah diserver maka akan mengirimkan pesan ke server dan clienr dan jika file sudah dikirimkan ke client maka akan menampilkan file berhaisl dikirimkan jika tidak maka akan menampilkan file gagal dikirimkan
- Apabila file tidak terdapat diserver maka akan menampilkan file tidak ada diserver

```
void addfile()
{
    if (fileExist(user_data.file))
    { //Jika terdapat file diserver
        message("File Sudah ada diserver\n");
        strcpy(user_data.mode, "recvstrings");
    }
    else
    { //Jika tidak ada file diserver
        message("File tidak ada diserver, bersiap menerima file!");
        strcpy(user_data.mode, "recvimage");
        receive_file(user_data.socket, user_data.file);
    }
}

void deletedb(char word[])
{ //Fungsi hapus data di file .tsv
    FILE *fp;
    char line[1024];
    char *buffer;
    char *ptr;

    buffer = (char *)malloc(1000 * sizeof(char));
    memset(buffer, 0, 1000 * sizeof(char));
    ptr = buffer;

    fp = fopen("files.tsv", "r");
    if (fp != NULL)
    {
        while (!feof(fp))
        {
            fgets(line, 1000, fp);
            if (strstr(line, word) == NULL)
            {
                //printf("%s",line);
                strcpy(ptr, line);
                ptr += strlen(line);
            }
        }
        fclose(fp);

        fp = fopen("files.tsv", "w");
        fprintf(fp, "%s", buffer);
        fclose(fp);
    }
    else
    {
        printf("File gagal dibuka");
    }
}
//Fungsi untuk menghapus file
void deletefile()
{
    char newName[1000], name[1000], db[1000];
    chdir("FILES"); //Ganti directory ke files
    sprintf(newName, "old-%s", user_data.file);
    if (rename(user_data.file, newName) == 0)
    {
        message("Berhasil menghapus files");
        printf("File %s telah dihapus.", user_data.file);
        chdir("../"); //Kembali ke folder server
        sprintf(name, "Hapus : %s (%s:%s)", user_data.file, user_data.name, user_data.pwd);
        catatlog(name); //Mencatat dilog
        sprintf(db, "FILES/%s", user_data.file);
        deletedb(db);
    }
    else
    {
        printf("File %s gagal dihapus.", user_data.file);
        message("Gagal menghapus files");
    }
}
```

- ```void addfile``` fungsi untuk menambahkan file keserver
- ```void deletedb``` fungsi untuk menghapus data di file.tsv. server akan memanggil fungsi deletedb 
- ``` voud deletfiles``` fungsi untuk menghapus file dimana client dapat menghapus file yang tersimpan di server. Jika file yang dibuang ternyata file yang penting, maka file hanya akan diganti namanya menjadi ‘old-NamaFile.ekstensi’. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus. ```catatlog(name)``` akan mencatat di log. Apabila file gagal dihapus maka akan menampilkan "Gagal menghapus files"

```
void seeallfile()
{ //Lihat isi files.tsv
    FILE *fp = fopen("files.tsv", "r");
    if (!fp)
    {
        printf("Can't open file\n");
    }
    else
    {
        char buffer[1024], buff[1024];
        char path[1024], publisher[1024];
        char getname2[1024], publisher2[1024], year[1024], ext[1024], path2[1024];
        int row = 0;
        int column = 0;

        while (fgets(buffer, 1024, fp))
        {
            column = 0;
            row++;

            if (row == 1) //Supaya column header tidak dicetak
                continue;

            char *value = strtok(buffer, "\t");
            while (value != NULL)
            {
                if (column == 0)
                {
                    sprintf(path, "%s", value);
                }
                else if (column == 1)
                {
                    sprintf(publisher, "%s", value);
                }
                else if (column == 2)
                {
                    char *getname = strtok(path, "FILES/");
                    sprintf(getname2, "Nama: %s", getname);
                    message(getname2);
                    sprintf(publisher2, "Publisher: %s", publisher);
                    message(publisher2);
                    sprintf(year, "Tahun Publishing: %s", value);
                    message(year);
                    if (!strrchr(getname, '.'))
                    {
                        message("Ekstensi File: No Extensions");
                    }
                    else
                    {
                        sprintf(ext, "Extensi File: %s", strrchr(getname, '.') + 1);
                        message(ext);
                    }
                    sprintf(path2, "Filepath: %s", path);
                    message(path2);
                }
                value = strtok(NULL, "\t");
                column++;
            }
            message(" ");
        }
        fclose(fp);
    }
}

void find(char *word)
{ //Cari kata di files.tsv
    char line[1024];
    char buffer[1024];
    FILE *fp = fopen("files.tsv", "r");
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strstr(line, word) == NULL)
        {
            message(line);
        }
    }
    fclose(fp);
}

void setfile(char cmd[])
{ //Split input dan set struct file
    char *fname;
    cmd = strtok(NULL, " "); //Split input
    fname = cmd;
    memset(user_data.file, 0, 1000);
    strcpy(user_data.file, fname);
}

void setfile(char cmd[])
{ //Split input dan set struct file
    char *fname;
    cmd = strtok(NULL, " "); //Split input
    fname = cmd;
    memset(user_data.file, 0, 1000);
    strcpy(user_data.file, fname);
}
```

- ```void seeallfile()``` fungsi untuk melihat isi files.tsv. Dimana files.tsv akan dibuka jika tidak dapat dibuka akan menampilkan "Can't open file\n".
- Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang bernama see. Dan output tersebut akan keluar dengan format yang telah ditentukan pada soal.
- ```void find``` yang nantinya client melakukan pencarian maka server akan memanggil fungsi find
- ```void setfile``` fungsi untuk split input dan set struct file dimana jika client menginput maka fungsi ini akan dipanggil server

```
void loginsukses()
{
    char msg[1024], buffer[1024];
    message("\e[1;1H\e[2J");
    printf("\nUser %s telah berhasil login.\n", user_data.name);
    message("Login berhasil!");
    while (strcmp(buffer, "exit") != 0 || strcmp(user_data.mode, "recvstrings") == 0)
    {
        message("Input Command: ");
        read(user_data.socket, buffer, 1024);
        //Memisahkan input dengan filenya
        char cmd_line[MAX_LENGTH];
        strcpy(cmd_line, buffer);
        char *cmd = strtok(cmd_line, " ");
        //Merubah input ke lowercase
        for (int i = 0; cmd[i]; i++)
        {
            cmd[i] = tolower(cmd[i]);
        }
        if (strcmp(cmd, "add") == 0)
        { //Tambahkan file jika add
            setfile(cmd);
            addfile(); //Menambahkan file diserver
        }
        else if (strcmp(cmd, "download") == 0)
        { //Fungsi download
            setfile(cmd);
            download(user_data.socket, user_data.file);
        }
        else if (strcmp(cmd, "delete") == 0)
        { //Fungsi delete
            setfile(cmd);
            deletefile();
        }
        else if (strcmp(cmd, "see") == 0)
        { //Fungsi see
            seeallfile();
        }
        else if (strcmp(cmd, "find") == 0)
        { //Fungsi find
            setfile(cmd);
            find(user_data.file);
            //printf("\nCari file %s", user_data.file);
        }
        else if (strcmp(cmd, "clear") == 0)
        {
            message("\e[1;1H\e[2J");
        }
        else
        {
            printf("\nArgument Invalid!");
            message("Argument Invalid!");
        }
    }
}
```
- ```void loginsukses``` fungsi login sukses apabila client dapat login dan akan menampilkan telah berhasil login dan pesan login berhasil
- Server akan menampilkan pesan input command. Jika client command add makan akan menambahkan file ```addfile``` file akan ditambahkan diserver. Jika client command download Server akan mencari file yang ingin client download kemudian mengirim file beserta ukuran dan isinya ke client. Server harus melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di folder client tersebut. Client juga dapat menghapus file dnegan command delete yang nantinya fungsi deletefile() akan dipanggil. Lalu Client dapat melihat semua isi files.tsv dengan command see maka fungsi seeallfile() yang telah dibuat akan dipanggil. Agar client dapat melakukan pencarian dengan command finf maka fungsi find() akan dipanggil untuk dapat dijalankan. Jika command tidak sesuai maka akan menampilkan "Argument Invalid".

```
void *input_main()
{
    char buffer[1024];
    while (1)
    {
        if (user_data.is_auth == 0)
        { //Menampilkan Menu Pilihan
            message("1. Login\n"
                    "2. Register\n"
                    "Choices: ");

            read(user_data.socket, buffer, 1024); //Dapatkan data dari pengguna
            for (int i = 0; buffer[i]; i++)
            { //Merubah input ke lowercase
                buffer[i] = tolower(buffer[i]);
            }
            //Jika user memilih atau mengetik login
            if (strcmp(buffer, "login") == 0 || strcmp(buffer, "1") == 0)
            {
                char id[1024];
                char password[1024];
                message("\e[1;1H\e[2J");
                //Dapatkan data id client
                message("Id: ");
                read(user_data.socket, id, 1024);
                //printf("Id client adalah : %s\n", id);

                //Dapatkan password client
                message("Password: ");
                read(user_data.socket, password, 1024);
                //printf("Password client adalah : %s\n", password);

                user_data.is_auth = login(id, password); //Fungsi untuk login
                if (user_data.is_auth == 0)
                { //Tampilkan pesan kesalahan di server dan client
                    message("\e[1;1H\e[2J");
                    message("Login gagal id/password salah!");
                    printf("Login gagal id/password salah!\n");
                    break;
                }
                else if (user_data.is_auth == 1)
                {
                    strcpy(user_data.name, id);
                    strcpy(user_data.pwd, password);
                    strcpy(user_data.mode, "recvstrings");
                    loginsukses();
                }
            }
            //Jika user mengetik register atau memilih 2
            if (strcmp(buffer, "register") == 0 || strcmp(buffer, "2") == 0)
            {
                char id[1024];
                char password[1024];
                message("\e[1;1H\e[2J");
                //Dapatkan data id client
                message("Id: ");
                read(user_data.socket, id, 1024);

                //Dapatkan password client
                message("Password: ");
                read(user_data.socket, password, 1024);

                regist(id, password); //Melakukan register

                char msg[1024];
                message("\e[1;1H\e[2J");
                printf("User %s telah terdaftar.", id);
                sprintf(msg, "Register user %s berhasil... (Press Anything to Continue)", id);
                message(msg);
                break;
                //read(user_data.socket, buffer, 1024);
            }
        }
        else if (user_data.is_auth == 1)
        { //Jika berhasil login maka ke process selanjutnya.
            loginsukses();
        }
    }
}
```
- pada fungsi input main ini akan menampilkan menu pilihan berupa login dan register client diminta untuk memilih dan menginputkan jika user memilih atau mengetik login maka akan menampilkan pesan ke client berupa id dan password lalu server akan mendapatkan data id dan password client. Apabila terjadi kesalahan pada client dan server maka akan menampilkan pesan "Login gagal id/password salah!". Jika berhasil maka memanggil fungsi loginsukses(). Agar client dapat login maka client register terlebih dahulu. Apabila client memilih register atau mnegetik 2 maka client akan menginputkan id dan password lalu server akan mendapatkan data dari client dan fungsi regist akan dipanggil. Apabila client berhasil untuk registrasi maka akan menampilkan user telah terdaftar. 

```
int main()
{
    int server_fd, clientsocket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server berhasil dijalankan\nMembuat folder FILES...\n");
        int check;
        char *dirname = "FILES";
        check = mkdir(dirname, 0777);

        // Mengecek apakah directory sudah dibuat apa belum
        if (!check)
        {
            printf("Folder Berhasil dibuat\n");
            printf("Menunggu sambungan dari klien.....\n");
        }
        else
        {
            printf("Folder gagal dibuat\n");
            exit(1);
        }
    }

    //Jumlah user maksimal 1, jika lebih maka akan mengantri
    if (listen(server_fd, 1) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((clientsocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Terhubung dengan klien yang beralamat: %d\n", clientsocket);
        user_data.socket = clientsocket;
    }
    user_data.is_auth = 0; //Menginisialisasi auth dengan 0

    pthread_create(&input, NULL, &input_main, 0);
    while (1)
    { //Agar terus berjalan
        if (pthread_join(input, NULL) == 0)
        {
            pthread_create(&input, NULL, &input_main, 0);
        }
    }
}
```
- Pada fungsi main, fungsi socket memiliki 3 parameter dan mengembalikan nilai integer, nilai integer inilah yang akan digunakan sebagai identifier file descriptor untuk melakukan IPC. Parameter pertama kita gunakan AF_INET, ini menunjukan bahwa kita akan membuat aplikasi yang berbasis pada IPv4. Parameter kedua kita akan menggunakan SOCK_STREAM, ini menunjukan bahwa kita akan membuat aplikasi yang menggunakan protocol TCP yaitu koneksi yang datanya ditransmisikan secara reliable atau bersifat connection oriented, koneksinya terbuat dulu baru data bisa dikirim dan diterima. Parameter ketiga adalah kita menggunakan angka 0, kita gunakan 0 agar penerapan protocol yang digunakan dapat diserahkan pengaturannya pada sistem operasi. 
- Variabel ```server_ad``` memiliki beberapa field. Field ```sin_family``` harus diset menjadi konstanta simbolic ```AF_INET```. Pada field ```sin_port``` harus diisikan dengan port. Agar portabilitas program terjamin maka digunakan fungsi ```htons()``` untuk mengubah format biner komputer host ke standar network dalam jenis bilangan short. Fungsi ini akan menyesuaikan byte data sehingga tidak terjadi kesalahan membaca antara host maupun jaringan.
- fungsi bind() digunakan untuk mengikat antara informasi alamat IP yang digunakan untuk tujuan pengiriman data dengan file descriptor dari socket yang telah dibuat. Parameter pertama yang digunakan adalah integer hasil dari pembuatan fungsi socket sebelumnya. Parameter kedua berisi informasi yang akan diikat dengan file descriptor socket, informasi tersebut dibuat dalam satu struct yaitu struct sockaddr. Parameter ketiga berisi ukuran dari struct sockaddr yang telah dibuat. Apabila server bisa dijalankan maka akan menampilkan "Server berhasil dijalankan\nMembuat folder FILES...\n"
- fungsi listen digunakan untuk membuat socket siap menerima connection dengan fungsi accept nantinya. Parameter pertama yang digunakan adalah integer hasil dari pembuatan fungsi socket sebelumnya. Parameter kedua diisi jumlah antrian yang bisa socket terima disini yang digunakan adalah 1, karena kita hanya akan membuat iterative server. 
- fungsi accept ini menerima dari antrian koneksi yang masuk pada server, membuat file descriptor baru sebagai media komunikasi yang akan digunakan pada pertukaran data nantinya dan merujuk pada socket yang telah dibuat sebelumnya. Parameter pertama yang digunakan adalah integer hasil dari pembuatan fungsi socket sebelumnya. Parameter kedua adalah adalah pointer ke struct sockaddr. Pointer ini berisi informasi mengenai peer yang terkoneksi dengan server. Parameter ketiga berisi ukuran dari pointer sockaddr yang menampung informasi client, nilai ini harus berbentuk pointer structure dari ukuran struct addr. Apabila client sudah terhubung dengan server maka akan menampilkan "Terhubung dengan klien yang beralamat:".

### Screenshot Output
