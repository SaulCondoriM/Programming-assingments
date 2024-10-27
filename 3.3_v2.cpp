#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Cada proceso inicializa un valor local
    int local_value = world_rank + 1; // Ejemplo: el valor es el rank + 1
    int sum = local_value;

    // Si el número de procesos no es potencia de dos, ajustamos los procesos extra
    int nearest_power_of_two = 1;
    while (nearest_power_of_two <= world_size / 2) {
        nearest_power_of_two *= 2;
    }

    if (world_rank >= nearest_power_of_two) {
        int destination = world_rank - nearest_power_of_two;
        MPI_Send(&sum, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
    } else {
        if (world_rank + nearest_power_of_two < world_size) {
            int received_value;
            MPI_Recv(&received_value, 1, MPI_INT, world_rank + nearest_power_of_two, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += received_value;
        }

        // Proceso de suma en estructura de árbol para la potencia de dos más cercana
        int step = 1;
        while (step < nearest_power_of_two) {
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
    }

    MPI_Finalize();
    return 0;
}
