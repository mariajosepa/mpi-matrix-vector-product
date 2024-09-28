#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ERROR_CODE  -1
#define ROOT  0
#define SIZE  1

//Processes and dimensions MUST be powers of two
//Run individually:  mpicc mpi-mult.c -o mpi-mult && mpirun --oversubscribe -np 4 ./mpi-mult < input.csv

int main(int argc, char * argv[]){

    int p, my_rank;
    int size;
    int * matriz, * matriz_local, * res;
    int * vector;
    int * ans;

    //IInitialize MPI
    MPI_Init(&argc , &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (p < 2){MPI_Abort( MPI_COMM_WORLD, ERROR_CODE);}
    //Reading matrix-vector dimension
    if (my_rank == ROOT){

        scanf("%d",&size);
        int val = 0;
        matriz = (int *)malloc(sizeof(int)*size*size);
        //Llenamos la matriz linealmente 
        for (int i = 0; i < size*size; i++){scanf("%d",&val);matriz[i] = val;}
    }

    // Obtain group of all processes
    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    //send the dimension to other processes
    MPI_Bcast(&size,1,MPI_INT,ROOT,MPI_COMM_WORLD);

    // Exclude processes that are not needed (if there are more processes than rows)
    MPI_Group new_group;
    if (p > size){
        int ranges[1][3] = { {size, p-1, 1} };
        MPI_Group_range_excl(world_group, 1, ranges, &new_group);
    }else{MPI_Comm_group(MPI_COMM_WORLD, &new_group);}

    // Create a new communicator
    MPI_Comm newcom;
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &newcom);

    //Make uneeded processes finish
    if (newcom == MPI_COMM_NULL)
        {
            // Bye bye cruel world
            MPI_Finalize();
            exit(0);
        }
    
    //Get new_group without uneeded processes
    MPI_Comm_size(newcom,&p);

    // From now on use newworld instead of MPI_COMM_WORLD
    //calculate rows per process
    int rows = size / p;
    //Calculate element number for each process
    int elements = size * rows;
    //allocate mem for sub matrix
    matriz_local = (int *)malloc(sizeof(int) * elements);
    //allocate mem for vector
    vector = (int *)malloc(sizeof(int) * size);
    //allocate mem for partial answers
    res = (int *)malloc(sizeof(int) * rows);

    //fill the vector
    if (my_rank == ROOT){
        int val = 0;
        for (int i = 0; i < size; i++)
        {
            scanf("%d",&val);
            vector[i] = val;
        }
    }

    //broadcast vector to everyone
    MPI_Bcast(vector,size,MPI_INT,ROOT,newcom);
    
    //scatter matrix rows to processes
    MPI_Scatter(matriz, elements, MPI_INT, matriz_local,elements, MPI_INT,ROOT,newcom);

    for (int i = 0; i < rows; i++) {res[i] = 0;}

    //hcalculate matrix-vector product
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < size; j++) {
            res[i] += matriz_local[i * size + j] * vector[j];
        }
    }

    if (my_rank == ROOT){ans = (int *)malloc(sizeof(int)*size);}
    
    //Gather partial matrix-vector products
    MPI_Gather( res, rows, MPI_INT, ans, rows, MPI_INT , ROOT, newcom);

    if (my_rank == ROOT){
        //Mostramos la respuesta
        printf("El producto matriz-vector es: ");
        for (int i = 0; i < size; i++){printf("%d ",ans[i]);}
        printf("\n");
    }
    
    if (my_rank == ROOT){
        free(matriz);
        free(ans);
    }
    
    free(matriz_local);
    free(vector);
    free(res);
    
    MPI_Finalize();
    return 0;
}