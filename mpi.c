#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int get_index(int row, int column, int size){
    return row*size + column;
}


void read_matrix(FILE* file, float** matrix, int* mat_size) {
    int N;
    float tmp;
    fscanf(file, "%d", &N);
    *mat_size = N;
    *matrix = (float *)malloc(N*N * sizeof(float));
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
             fscanf(file, "%f", &tmp);
             (*matrix)[get_index(i, j, *mat_size)] = tmp;
//              printf("%f \n", matrix[i][j]);
        }
    }
}

void read_vector(FILE* file, float** vec) {
    
    int N;
    float tmp;
    fscanf(file, "%d", &N);

    *vec = (float *)malloc(N * sizeof(float));
    
    for(int i = 0; i < N; i++) {
        fscanf(file, "%f", &tmp);
        (*vec)[i] = tmp;
    }
}

void get_block(float* matrix, float* vec, int x, int y, int block_size, int mat_size, float** block, float** vec_part)
{
    *block = (float *)malloc(block_size*block_size * sizeof(float));
    *vec_part = (float *)malloc(block_size * sizeof(float));
    
    for(int i = x; i < x + block_size; i++) {
        for(int j = y; j < y + block_size; j++) {
            (*block)[get_index(i-x, j-y, block_size)] = matrix[get_index(i, j, mat_size)];
            (*vec_part)[j - y] = vec[j];
        }
    }
}

void print_block(float* matr, int size){
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            printf("%f ", matr[get_index(i, j, size)]);
        }
        printf("\n");
    }
}

void mat_vec_mul(float* mat, float* vec, int size, float* vec_out){
    float tmp;
    for(int i = 0; i < size; i++) {
        tmp = 0;
        for(int j = 0; j < size; j++) {
            tmp += mat[get_index(i, j, size)]*vec[j];
        }
        vec_out[i] = tmp;
    }
}

int main(int argc, char** argv) {
    
    int block_size;
    float res[2];
    MPI_Init(NULL, NULL);
    
    //cartesian grid
    MPI_Comm cartcomm;
    
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    int dim[2] = {2, 2}, period[2] = {0, 0}, reorder = 0;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, reorder, &cartcomm);
//     float mm = malloc(sizeof(float) * 4);
    if(world_rank == 0) {

        FILE* fp;

        fp = fopen("mat", "r");
        float* matr;
        float* vec;
        float* vec_part;
        int mat_size;
        read_matrix(fp, &matr, &mat_size);
        FILE* fp2;
        fp2 = fopen("vect", "r");
        read_vector(fp2, &vec);
        print_block(matr, 4);
        printf("%d \n", mat_size);
        float* block;
        int coord[2];
        for(int i=1; i < world_size; i++) {
            MPI_Cart_coords(cartcomm, i, 2, coord);
            printf("COORDS %d %d\n", coord[0], coord[1]);
            get_block(matr, vec, coord[0]*2, coord[1]*2, 2, mat_size, &block, &vec_part);
            printf("SENDING to %d\n", i);
            print_block(block, 2);
            printf("VEC %f %f\n", vec_part[0], vec_part[1]);
            MPI_Send(block, 4, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            MPI_Send(vec_part, 2, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
        }
    }

    float* mat_part = (float*)malloc(2*2*sizeof(float));
    float* vec_part = (float*)malloc(2*sizeof(float));

    if(world_rank != 0) {
        MPI_Recv(mat_part, 4, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(vec_part, 2, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("\nBLOCK at %d\n %f %f\n%f %f\n", world_rank, mat_part[0], mat_part[1], mat_part[2], mat_part[3]);
        printf("VEC at %d %f %f\n",world_rank, vec_part[0], vec_part[1]);
        
        
        mat_vec_mul(mat_part, vec_part, 2, res);
        printf("RES at %d %f %f\n",world_rank, res[0], res[1]);
    }


    // Print off a hello world message
//     printf("Hello world from processor %s, rank %d out of %d processors\n",
//            processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();
}