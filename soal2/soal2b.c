#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include<pthread.h>

#define M 4
#define N 3
#define O 6

struct arg_struct
{
    int arg1; // matrix from shared mem
    int arg2; // destination matrix
    int x; // index
    int y; // index
};

long long resultMatrix[M][O];

long long factorial(int n)
{
    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}

void *prerequisite(void *arguments){
    pthread_t id=pthread_self();
    struct arg_struct *args = (struct arg_struct*)arguments;
    int a = (*args).arg1, b = (*args).arg2, i = (*args).x, j = (*args).y;

    // resultMatrix[i][j] = factorial(a);
    // printf ("matriks A %d, %d: %lld\n", i, j, resultMatrix[i][j]);
    if (a == 0 || b == 0){
        resultMatrix[i][j] = 0;
    }
    else if (a >= b){
        resultMatrix[i][j] = factorial(a)/factorial(a-b);
    }
    else if (a < b){
        resultMatrix[i][j] = factorial(a);
    }
    // free (arguments);
    return NULL;
}


int main()
{
    int *res;
    int A[M][O];

    // pre-made matrix so i can debug faster
    // int A[M][O] = { { 1, 2, 2, 1, 2, 4 },
    //                 { 2, 2, 1, 4, 5, 9 },
    //                 { 4, 4, 4, 0, 7, 2 },
    //                 { 1, 8, 1, 2, 0, 5 } };

    key_t key = 1234;
    int shmid = shmget(key, sizeof(res), IPC_CREAT | 0666);
    res = shmat(shmid, 0, 0);

    //input matriks kedua
    printf("Input elemen matriks kedua (4 x 6):\n");
    for (int i=0; i < M; i++){
        for(int j=0; j < O; j++){
            scanf("%d", &A[i][j]);
        }
    }

    pthread_t tid[24];
    struct arg_struct *args;
    int iter = 0; // for iterating through shared memory
    for (int i = 0; i < M; i++){
        for(int j = 0; j < O; j++){
            args = malloc(sizeof(struct arg_struct));
            (*args).arg1 = res[iter];
            (*args).arg2 = A[i][j];
            (*args).x = i;
            (*args).y = j;
            pthread_create(&(tid[i]), NULL, &prerequisite, (void*) args);
            iter++;
        }
    }
    for (int i = 0; i < M; i++){
        for(int j = 0; j < O; j++){
            pthread_join(tid[i],NULL);
        }
    }

    sleep (1); // tunggu selesai kalkulasi
    // print matriks hasil faktorial
    for (int i = 0; i < M; i++){
        for(int j = 0; j < O; j++){
            printf("%lld ", resultMatrix[i][j]);
        }
        printf("\n");
    }

    shmdt(res);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
