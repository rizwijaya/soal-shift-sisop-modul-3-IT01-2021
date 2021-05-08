#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h> 
#include <time.h>

#define PORT 8080
#define MAX_LENGTH 1000
#define SVR "./FILES/"

pthread_t input, received;

void *input_main();
int error(char *err){
	perror(err);
	exit(EXIT_FAILURE);
}

struct user {
    char name[1000];
    char pwd[1000];
    char file[1000];
    char mode[1000];
    int is_auth;
    int socket;
} user_data;

//Fungsi untuk mengirimkan pesan ke client
void message(char input[]) {
    char buffer[1024];
    sprintf(buffer, "\n%s\n", input);
    send(user_data.socket, buffer, 1024, 0);
}

//Fungsi untuk melakukan login
int login(char id[], char password[]) {
    FILE *fp = fopen("akun.txt", "r"); //Buka file akun
    int is_auth = 0;
    char buffer[1024];
    while (fgets(buffer, 1024, fp) != NULL && is_auth == 0) {
        char file_id[1024], file_password[1024];
        //Memisahkan id dan password pada file
        char *token = strtok(buffer, ":");
        strcpy(file_id, token);
        token = strtok(NULL, "\n");
        strcpy(file_password, token);

        //Jika id dan password sesuai, maka return ke 1
        if (strcmp(id, file_id) == 0 && strcmp(password, file_password) == 0) {
            is_auth = 1;
        } else {
            is_auth = 0;
        }
    }
    fclose(fp);
    return is_auth;
}

//Fungsi untuk melakukan register
void regist(char id[], char password[]) {
    FILE *fp = fopen("akun.txt", "a"); //Menulis id dan password di akun.txt
    fprintf(fp, "%s:%s\n", id, password);
    fclose(fp);
}

//Fungsi tambahkan ke file tsv
int addtsv(int year) {
    FILE *fp;
    fp = fopen("files.tsv", "a+");
    if(fp == NULL) {
        printf("File tidak dapat dibuka");
        return 1;
    }
    fprintf(fp, "\nFILES/%s\t%s\t%d", user_data.file, user_data.name, year);
    fclose(fp);
    return 0;
}

void catatlog(char argument[]) { //Fungsi untuk mencetak di log
    FILE *fp;
    fp = fopen("running.log", "a+");
    fprintf(fp, argument);
    fclose(fp);
}

//Fungsi untuk menerima file
int receive_file(int socket,char* fname){
	char buffer[MAX_LENGTH] = {0}; char fpath[MAX_LENGTH];
    strcpy(fpath,SVR);
    strcat(fpath,fname);
	FILE *file_masuk = fopen(fpath, "wb");
    if(file_masuk == NULL) {
        printf("File %s, Tidak dapat dibuat di server.\n", fname);
    } else {
        bzero(buffer, MAX_LENGTH); 
        int file_size = 0;
        while((file_size = recv(socket, buffer, MAX_LENGTH, 0)) > 0) {
            int write_size = fwrite(buffer, sizeof(char), file_size, file_masuk);
            if(write_size < file_size){
                error("Gagal menulis file diserver.");
            }
            bzero(buffer, MAX_LENGTH);
            if (file_size == 0 || file_size != MAX_LENGTH ) {
                break;
            }
        }
        if(file_size < 0) {
            if (errno == EAGAIN) {
                printf("Waktu habis.\n");
            } else {
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
    catatlog(log); //Tambahkan log
    strcpy(user_data.mode, "recvstrings"); //Set mode ke input strings
}

//Fungsi untuk mengecek apakah ada file diserver
int fileExist(char *fname){
	int found = 0;
	DIR *di;
	struct dirent *dir;
	di = opendir(SVR);
	while ((dir = readdir(di)) != NULL){
		if(strcmp(dir->d_name,fname)==0){
			found=1;
			break;
		}
	}
	closedir(di);
	return found;
}

int send_file(int socket, char* fname) {
	char buffer[MAX_LENGTH] = {0};
    char fpath[MAX_LENGTH];
    strcpy(fpath,SVR);
    strcat(fpath,fname);
	FILE *file = fopen(fpath, "r");
    if(file == NULL) {
      printf("File %s tidak ada.\n", fname);
      return -1;
    }
    bzero(buffer, MAX_LENGTH); 
    int file_size; 
    while((file_size = fread(buffer, sizeof(char), MAX_LENGTH, file)) > 0) {
      if(send(socket, buffer, file_size, 0) < 0) {
          fprintf(stderr, "Gagal mengirimkan file %s. (errno = %d)\n", fname, errno);
          break;
      }
      bzero(buffer, MAX_LENGTH);
    }
  fclose(file);
  return 0;
}

//Fungsi untuk download files
void download(int accept_sockfd, char* fname){
    char buffer[MAX_LENGTH];
    if(fileExist(fname)){
        //Mengirimkan message keserver dan client
        printf("File %s Terdapat diserver\n", fname);
        char msg[1024];
        //Mengirimkan files ke client
        if(send_file(accept_sockfd, fname) == 0) {
            printf("File berhasil dikirimkan!!\n");
            strcpy(user_data.mode, "recvstrings");
        } else {
            printf("File gagal dikirimkan\n");
            strcpy(user_data.mode, "recvstrings");
        }
    } else{
        char msg[1024];
        sprintf(msg, "File %s tidak ada diserver", fname);
        message(msg);
        strcpy(user_data.mode, "recvstrings");
    }
}

//Fungsi Tambahkan file ke server
void addfile() {
    if(fileExist(user_data.file)) { //Jika terdapat file diserver
        message("File Sudah ada diserver\n");
        strcpy(user_data.mode, "recvstrings");
    } else { //Jika tidak ada file diserver
        message("File tidak ada diserver, bersiap menerima file!");
        strcpy(user_data.mode, "recvimage");
        receive_file(user_data.socket,user_data.file);
    }
}

void deletedb(char word[]) { //Fungsi hapus data di file .tsv
    FILE *fp;
    char line[1024];
    char *buffer;
    char *ptr;

    buffer = (char *)malloc(1000 * sizeof(char));
    memset(buffer, 0, 1000 * sizeof(char));
    ptr = buffer;

    fp = fopen("files.tsv", "r");
    if (fp != NULL) {
        while (!feof(fp)) {
            fgets(line, 1000, fp);
            if (strstr(line , word )== NULL) {
                //printf("%s",line);
                strcpy(ptr, line);
                ptr += strlen(line);
            }
        }
        fclose(fp);

        fp = fopen("files.tsv", "w");
        fprintf(fp, "%s", buffer);
        fclose(fp);
    } else {
        printf("File gagal dibuka");
    }
}
//Fungsi untuk menghapus file
void deletefile() {
    char newName[1000], name[1000], db[1000];
    chdir("FILES"); //Ganti directory ke files
    sprintf(newName, "old-%s", user_data.file);
    if (rename(user_data.file, newName) == 0) {
        message("Berhasil menghapus files");
        printf("File %s telah dihapus.", user_data.file);
        chdir("../"); //Kembali ke folder server
        sprintf(name, "Hapus : %s (%s:%s)", user_data.file, user_data.name, user_data.pwd);
        catatlog(name); //Mencatat dilog
        sprintf(db, "FILES/%s", user_data.file);
        deletedb(db);
    } else {
        printf("File %s gagal dihapus.", user_data.file);
        message("Gagal menghapus files");
    }
}

void seeallfile() { //Lihat isi files.tsv
   	FILE* fp = fopen("files.tsv", "r");
	if (!fp) {
		printf("Can't open file\n");
	} else {
		char buffer[1024], buff[1024];
        char path[1024], publisher[1024];
        char getname2[1024], publisher2[1024], year[1024], ext[1024], path2[1024];
		int row = 0;
		int column = 0;

		while (fgets(buffer, 1024, fp)) {
			column = 0;
			row++;

			if (row == 1) //Supaya column header tidak dicetak
				continue;

			char* value = strtok(buffer, "\t");
			while (value != NULL) {
				if (column == 0) {
                    sprintf(path, "%s", value);
				} else if (column == 1) {
                    sprintf(publisher, "%s", value);
				}else if (column == 2) {
                    char* getname = strtok(path, "FILES/");
                    sprintf(getname2, "Nama: %s", getname);
                    message(getname2);
                    sprintf(publisher2, "Publisher: %s", publisher);
                    message(publisher2);
                    sprintf(year, "Tahun Publishing: %s", value);
                    message(year);
                    if (!strrchr(getname, '.')) {
                        message("Ekstensi File: No Extensions");
                    } else {
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

void find(char *word){ //Cari kata di files.tsv
   char line[1024] ;
   char buffer[1024];
   FILE* fp = fopen("files.tsv", "r");
   while (fgets(line , sizeof(line) , fp )!= NULL)
   {
      if (strstr(line , word ) == NULL)
      {
         message(line);
      }
   }
   fclose(fp);
}

void setfile(char cmd[]) { //Split input dan set struct file
    char *fname;
    cmd = strtok(NULL, " "); //Split input
    fname = cmd;
    memset(user_data.file, 0, 1000);
    strcpy(user_data.file, fname);
}

void loginsukses() {
    char msg[1024], buffer[1024];
    message("\e[1;1H\e[2J");
    printf("\nUser %s telah berhasil login.\n", user_data.name);
    message("Login berhasil!");
    while (strcmp(buffer, "exit") != 0 || strcmp(user_data.mode, "recvstrings") == 0) {
        message("Input Command: ");
        read(user_data.socket, buffer, 1024);
        //Memisahkan input dengan filenya
        char cmd_line[MAX_LENGTH];
        strcpy(cmd_line, buffer);
        char *cmd = strtok(cmd_line, " ");
        //Merubah input ke lowercase
        for (int i = 0; cmd[i]; i++) { 
            cmd[i] = tolower(cmd[i]);
        }
        if (strcmp(cmd, "add") == 0) { //Tambahkan file jika add
            setfile(cmd);
            addfile(); //Menambahkan file diserver
        } else if (strcmp(cmd, "download") == 0) { //Fungsi download
            setfile(cmd);
            download(user_data.socket, user_data.file);
        } else if (strcmp(cmd, "delete") == 0) { //Fungsi delete
            setfile(cmd);
            deletefile();
        } else if (strcmp(cmd, "see") == 0) { //Fungsi see
            seeallfile();
        } else if (strcmp(cmd, "find") == 0) { //Fungsi find
            setfile(cmd);
            find(user_data.file);
            //printf("\nCari file %s", user_data.file);
        } else if (strcmp(cmd, "clear") == 0) {
            message("\e[1;1H\e[2J");
        } else {
            printf("\nArgument Invalid!");
            message("Argument Invalid!");
        }
    }
}

void *input_main()
{
    char buffer[1024];
    while (1) { 
        if (user_data.is_auth == 0) { //Menampilkan Menu Pilihan
            message("1. Login\n"
                    "2. Register\n"
                    "Choices: ");

            read(user_data.socket, buffer, 1024); //Dapatkan data dari pengguna
            for (int i = 0; buffer[i]; i++) { //Merubah input ke lowercase
                buffer[i] = tolower(buffer[i]);
            }
            //Jika user memilih atau mengetik login
            if (strcmp(buffer, "login") == 0 || strcmp(buffer, "1") == 0) {
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
                if (user_data.is_auth == 0) { //Tampilkan pesan kesalahan di server dan client
                    message("\e[1;1H\e[2J");
                    message("Login gagal id/password salah!");
                    printf("Login gagal id/password salah!\n");
                    break;
                } else if (user_data.is_auth == 1) {
                    strcpy(user_data.name, id);
                    strcpy(user_data.pwd, password);
                    strcpy(user_data.mode, "recvstrings");
                    loginsukses();
                }
            }
            //Jika user mengetik register atau memilih 2
            if (strcmp(buffer, "register") == 0 || strcmp(buffer, "2") == 0) {
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
        } else if (user_data.is_auth == 1) { //Jika berhasil login maka ke process selanjutnya.
            loginsukses();
        }
    }
}

int main() {
    int server_fd, clientsocket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Server berhasil dijalankan\nMembuat folder FILES...\n");
        int check; char* dirname = "FILES";
        check = mkdir(dirname,0777);
    
        // check if directory is created or not
        if (!check) {
            printf("Folder Berhasil dibuat\n");
            printf("Menunggu sambungan dari klien.....\n");
        } else {
            printf("Folder gagal dibuat\n");
            exit(1);
        }
    }

    //Jumlah user maksimal 1, jika lebih maka akan mengantri
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((clientsocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    } else {
        printf("Terhubung dengan klien yang beralamat: %d\n", clientsocket);
        user_data.socket = clientsocket;
    }
    user_data.is_auth = 0; //Menginisialisasi auth dengan 0
    
    pthread_create(&input, NULL, &input_main, 0);
    while(1) { //Agar terus berjalan
        if (pthread_join(input, NULL) == 0) {
            pthread_create(&input, NULL, &input_main, 0);
        } 
    }
}