#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void read_matrix(FILE* file, float*** matrix, int* mat_size) {
    int N;
    float tmp;
    fscanf(file, "%d", &N);
    *mat_size = N;
    *matrix = (float **)malloc(N * sizeof(float *));
    for(int i = 0; i < N; i++)
        (*matrix)[i] = (float *)malloc(N * sizeof(float));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
             fscanf(file, "%f", &tmp);
             (*matrix)[i][j] = tmp;
//              printf("%f \n", matrix[i][j]);
        }
    }
}

void get_block(float** matrix, int x, int y, int block_size, float*** block)
{
    *block = (float **)malloc(block_size * sizeof(float *));
    for(int i=0; i < block_size; i++)
        (*block)[i] = (float *)malloc(block_size * sizeof(float));
    for(int i = x; i < x + block_size; i++) {
        for(int j = y; j < y + block_size; j++) {
            (*block)[i-x][j-y] = matrix[i][j];
        }
    }
}

int main(int argc, char** argv) {
    
    int block_size;

    MPI_Init(NULL, NULL);
    
    //cartesian grid
    MPI_Comm cartcomm;
    
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    
//     float mm = malloc(sizeof(float) * 4);
    if(world_rank == 0) {

    FILE *fp;
//     char buff[255];

    fp = fopen("mat", "r");
    float** matr;
    int mat_size;
    read_matrix(fp, &matr, &mat_size);
     printf("%d \n", mat_size);
    float** block;
    get_block(matr, 0, 0, 2,  &block);
    printf("%f", block[0][0]);
//      block_size = mat_size/sqrt(world_size);
    }
   
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
//     printf("Hello world from processor %s, rank %d out of %d processors\n",
//            processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();
}