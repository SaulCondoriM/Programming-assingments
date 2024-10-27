#include <mpi.h>
#include <iostream>
#include <cmath>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Verificar si el número de procesos es una potencia de dos
    if ((world_size & (world_size - 1)) != 0) {
        if (world_rank == 0) {
            std::cerr << "El numero de procesos debe ser una potencia de dos." << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Cada proceso inicializa un valor local
    int local_value = world_rank + 1; // Ejemplo: el valor es el rank + 1
    int sum = local_value;

    int step = 1;
    while (step < world_size) {
        int partner = world_rank ^ step; // Calcula el proceso par usando XOR
        int received_value;

        MPI_Sendrecv(&sum, 1, MPI_INT, partner, 0, &received_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sum += received_value;
        step *= 2;
    }

    // El proceso 0 imprime la suma global
    if (world_rank == 0) {
        std::cout << "La suma global es: " << sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
