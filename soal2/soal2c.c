#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>

int pid, pid_2, status, waiting;
int fd[2];  // Store pipe 1
int fd2[2]; // Store pipe 2

void closepipes() {// Fungsi untuk menutup 2 pipes
    close(fd[0]);
    close(fd[1]);
    close(fd2[0]);
    close(fd2[1]);
}

void head() {
    dup2(fd2[0], 0);
    closepipes(); //Tutup pipes
    // eksekusi command head
    execlp("/usr/bin/head", "head", "-5", NULL);
    perror("exec"); //Jika terjadi error
    exit(0);
}

void sort() {
    dup2(fd[0], 0);
    dup2(fd2[1], 1);
    closepipes(); //Tutup pipes
    //Eksekusi command sort
    execlp("/usr/bin/sort", "sort", "-nrk", "3,3", NULL);
    perror("exec"); //Jika terjadi error
    exit(0);
}

void psaux() {
    dup2(fd[1], 1);//replace ouput dengan output pipes
    closepipes(); //Tutup pipes
    //Eksekusi command ps
    execlp("/bin/ps", "ps", "aux", NULL);
    perror("exec"); //Jika terjadi error
    exit(0);
}

int main(int argc, char **argv) {
    // Buat pipes baru
    pipe(fd);
    pipe(fd2);

    if (pipe(fd) == -1) { 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 
	if (pipe(fd2) == -1) { 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 
    pid = fork(); //fork child pertama
    if (pid < 0) { 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} else if (pid == 0) {
        pid_2 = fork(); // fork child's child
        if (pid_2 < 0) { 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	    } else if (pid_2 == 0) {
            head(fd, fd2);
        }
        sort();
    } else if (pid > 0) { // parents
        psaux();
        while ((waiting = wait(&status)) > 0) {
            printf("Child has completed.\n");
        }
    }
    return 0;
}