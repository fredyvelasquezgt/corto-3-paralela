#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 5000

int main(int argc, char** argv) {
    int rank, size;
    int local_sum = 0, global_sum;
    int local_max = 0, global_max;
    int local_min = INT_MAX, global_min;
    int* values = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) {
        values = (int*)malloc(sizeof(int) * N);
        srand(time(NULL));
        for(int i = 0; i < N; i++) {
            values[i] = rand() % 10000;  // generando valores aleatorios entre 0 y 9999
        }
    }

    // Distribuir el array entre todos los procesos
    int* local_values = (int*)malloc(sizeof(int) * N/size);
    MPI_Scatter(values, N/size, MPI_INT, local_values, N/size, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso calcula la suma, el máximo y el mínimo local
    for(int i = 0; i < N/size; i++) {
        local_sum += local_values[i];
        if(local_values[i] > local_max) {
            local_max = local_values[i];
        }
        if(local_values[i] < local_min) {
            local_min = local_values[i];
        }
    }

    // Reducción para obtener la suma global
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Reducción para obtener el máximo global
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    // Reducción para obtener el mínimo global
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        printf("Suma total = %d\n", global_sum);
        printf("Máximo valor = %d\n", global_max);
        printf("Mínimo valor = %d\n", global_min);
        free(values);
    }

    free(local_values);
    MPI_Finalize();
    return 0;
}
