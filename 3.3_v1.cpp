#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Verificar si world_size es una potencia de dos
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

    // Proceso de suma en estructura de árbol
    int step = 1;
    while (step < world_size) {
        if (world_rank % (2 * step) == 0) {
            int source = world_rank + step;
            int received_value;
            MPI_Recv(&received_value, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += received_value;
        } else if ((world_rank % (2 * step)) == step) {
            int destination = world_rank - step;
            MPI_Send(&sum, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    // El proceso 0 imprime el resultado final
    if (world_rank == 0) {
        std::cout << "La suma global es: " << sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
