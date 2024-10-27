#include <mpi.h>
#include <iostream>
#include <cmath>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int local_value = world_rank + 1; // Ejemplo: el valor es el rank + 1
    int sum = local_value;

    // Determinamos la mayor potencia de dos menor o igual a world_size
    int nearest_power_of_two = 1;
    while (nearest_power_of_two <= world_size / 2) {
        nearest_power_of_two *= 2;
    }

    // Ajustar procesos que están fuera de la potencia de dos
    if (world_rank >= nearest_power_of_two) {
        int destination = world_rank - nearest_power_of_two;
        MPI_Send(&sum, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
    } else {
        if (world_rank + nearest_power_of_two < world_size) {
            int received_value;
            MPI_Recv(&received_value, 1, MPI_INT, world_rank + nearest_power_of_two, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += received_value;
        }

        // Realizar la suma en mariposa con la potencia de dos más cercana
        int step = 1;
        while (step < nearest_power_of_two) {
            int partner = world_rank ^ step;
            int received_value;

            MPI_Sendrecv(&sum, 1, MPI_INT, partner, 0, &received_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += received_value;
            step *= 2;
        }

        // El proceso 0 imprime la suma global
        if (world_rank == 0) {
            std::cout << "La suma global es: " << sum << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
