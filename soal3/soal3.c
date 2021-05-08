#include<dirent.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <libgen.h>

pthread_t tid[PATH_MAX];
int argi;
char **args;
int j = 0;

char *get_filename(const char *filename) {
    const char *dot = strrchr(filename, '/');
    // kalau langsung filename di argumennya
    if(!dot) return filename;
    return dot + 1;
}

char *get_filename_ext(const char *filename) {
    const char *dot;
    const char needle[10] = ".tar.gz";
    if (strstr(filename, needle) != NULL){
        dot = strstr(filename, needle);
    }else{
        dot = strrchr(filename, '.');
    }

    if(filename[0] == '.') return "Hidden";
    if(!dot || dot == filename) return "Unknown";
    
    char *lowered = dot;
    for (int i = 0; lowered[i]; i++)
    {
        lowered[i] = tolower(lowered[i]);
        /* code */
    }
    
    return lowered + 1;
}


void *move(void *arg){
    pthread_t id=pthread_self();
    // arg = /path/to/file.jpg how to misah?

    char *filename;
    char *path = strdup((char *)arg);
    char cwd[PATH_MAX];
    char result[80];
    
    // because "-d" and "*"" arguments only brings file name, 
    // we need to concatenate (char* path) with their respective path
    if (args[2] != NULL && (strcmp(args[1], "-d") == 0)) {
        snprintf(result, sizeof(result), "%s/%s", args[2], path);
        path = strdup(result);
        strcpy(result, "");
    }
    else if (strcmp(args[1], "*") == 0) {
        snprintf(result, sizeof(result), "%s/%s", getcwd(cwd, sizeof(cwd)), path);
        path = strdup(result);
        strcpy(result, "");
    }
    
    //printf ("%s\n", path);
    filename = basename(path);
    mkdir (get_filename_ext(filename), 0777);
    
    // move the desired file
    //char result[80];
    snprintf(result, sizeof(result), "%s/%s/%s", getcwd(cwd, sizeof(cwd)),get_filename_ext(filename), filename);
    //printf ("path:%s\nfilename:%s\nresultDir:%s\nextension:%s\n\n", path, filename, result, get_filename_ext(filename));
    rename(path, result);
    free(path);
}

void *move2(void *arg){
    //sama seperti move hanya tidak ada pengecekan argumen
    // arg = /path/to/file
    pthread_t id=pthread_self();
   
    char *filename;
    char *path = strdup((char *)arg);
    char cwd[PATH_MAX];
    char result[80];

    //printf ("%s\n", path);
    filename = basename(path);
    mkdir (get_filename_ext(filename), 0777);
    
    // move the desired file
    //char result[80];
    snprintf(result, sizeof(result), "%s/%s/%s", getcwd(cwd, sizeof(cwd)),get_filename_ext(filename), filename);
    //printf ("path:%s\nfilename:%s\nresultDir:%s\nextension:%s\n\n", path, filename, result, get_filename_ext(filename));
    rename(path, result);
    free(path);
}

void moveDir(const char *name){
    DIR *dir;
    struct dirent *ent;
    char cwd[PATH_MAX];
    if (!(dir = opendir(name))) return;

    while ((ent = readdir (dir)) != NULL) 
        {
            // normal file
            if (ent->d_type == DT_REG){
                char paths[100];
                snprintf(paths, sizeof(paths), "%s/%s", name, ent->d_name);
                //printf ("%s\n", paths);
                pthread_create(&tid[j], NULL, &move2, paths);
            }
            else if (ent->d_type == DT_DIR){
                // kalau folder, 
                char path[1024];
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

                if (args[2] != NULL && (strcmp(args[1], "-d") == 0)) {
                    snprintf(path, sizeof(path), "%s/%s", args[2], ent->d_name);
                }
                else if (strcmp(args[1], "*") == 0) {
                    snprintf(path, sizeof(path), "%s/%s", getcwd(cwd, sizeof(cwd)), ent->d_name);
                }

                moveDir(path);
            }
            j++;
        }

        closedir (dir);
}

int main(int argc, char **argv)
{
    argi = argc;
    args = argv;
    int err;
	if (argc < 2)
    {
        printf("missing argument\n");
        exit(-1);
    }
    if (strcmp(argv[1], "-f") == 0 && argc > 2)
    {
        pthread_t tid[argc-2]; //inisialisasi array untuk menampung thread dalam kasus ini ada argc-2 thread
        // loop every argc
        for (int i = 2; i < argc; i++)
        {
            err = pthread_create(&tid[j], NULL, &move, argv[i]);
            if(err!=0) //cek error
            {
                printf("%s : Sad, gagal :(\n",get_filename((char*)argv[i]));
            }
            else
            {
                printf("%s : Berhasil Dikategorikan\n",get_filename((char*)argv[i]));
            }
            j++;
        }
        for (int j = 0; j < argc - 2; j++){
            pthread_join(tid[j], NULL);
        }
    }
    else if ((strcmp(argv[1], "-d") == 0 && argv[2] != NULL) || (strcmp(argv[1], "*") == 0 && argc == 2))
    {
        // do something
        DIR *dir;
        struct dirent *ent;
        char cwd[PATH_MAX];
        if (strcmp(argv[1], "-d") == 0 ){
            if ((dir = opendir(argv[2])) == NULL)
            {
                printf("Can't open %s\n", (char *)argv[2]);
                return 0;
            }
        }
        else {
            dir = opendir (getcwd(cwd, sizeof(cwd)));
        }

        while ((ent = readdir (dir)) != NULL) 
        {
            // normal file
            if (ent->d_type == DT_REG){
                err = pthread_create(&tid[j], NULL, &move, ent->d_name);
            }
            else if (ent->d_type == DT_DIR){
                // kalau folder, 
                char path[1024];
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

                if (args[2] != NULL && (strcmp(args[1], "-d") == 0)) {
                    snprintf(path, sizeof(path), "%s/%s", args[2], ent->d_name);
                }
                else if (strcmp(args[1], "*") == 0) {
                    snprintf(path, sizeof(path), "%s/%s", getcwd(cwd, sizeof(cwd)), ent->d_name);
                }

                moveDir(path);
            }
            j++;
        }
        for (int x = 0; x < j; x++){
             pthread_join(tid[x], NULL);
        }
        closedir (dir);
        if(err!=0) //cek error
        {
            printf("Direktori sukses disimpan!\n");
        }
        else
        {
            printf("Yah, gagal disimpan :(\n");
        }
            j++;
    }
	return 0;
}
