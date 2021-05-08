#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define M 4
#define N 3
#define O 6

//void *multiply(void *args)

int main()
{
        int A[M][N];
        int B[N][O];

        // pre-made matrix so i can debug faster
        // int A[M][N] = { { 1, 2, 3 },
        //                 { 3, 1, 2 },
        //                 { 3, 1, 2 },
        //                 { 2, 1, 3 } };

        // int B[N][O] = { { 1, 2, 2, 1, 2, 1 },
        //                 { 1, 2, 2, 2, 2, 1 },
        //                 { 1, 1, 1, 2, 0, 0 } };
        
        int *res;
        int temp_matrix[M][O];

        key_t key = 1234;
        int shmid = shmget(key, sizeof(res), IPC_CREAT | 0666);
        res = shmat(shmid, 0, 0);
    
        printf("Input elemen matriks pertama (4 x 3):\n");
        //input elemen matriks pertama
        for (int i = 0; i < M; i++){
            for(int j = 0; j < N; j++){
                scanf("%d", &A[i][j]);
            }
        }
        
        printf("Input elemen matriks kedua (3 x 6):\n");
        //input elemen matriks kedua
        for (int i=0; i < N; i++){
            for(int j=0; j < O; j++){
                scanf("%d", &B[i][j]);
            }
        }

        // perkalian
        for (int i = 0; i < M; i++){ 
            for(int j = 0; j < O; j++){ 
                temp_matrix[i][j] = 0;
                for (int k = 0; k < N; k++){ 
                    temp_matrix[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        //tes print
        int iter = 0;
        for (int i = 0; i < M; ++i){
            for(int j = 0; j < O; ++j){
                printf("%d ", temp_matrix[i][j]);
                res[iter] = temp_matrix[i][j]; //store in a 1d array
                iter++;
            }
            printf("\n");
        }
        

        // shmdt(res);
        // shmctl(shmid, IPC_RMID, NULL);
        return 0;
}
