#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Función para generar un número aleatorio entre -1 y 1
double random_double() {
    return static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    long long int number_of_tosses;
    long long int local_number_in_circle = 0;
    long long int global_number_in_circle = 0;

    if (world_rank == 0) {
        // Inicializa la semilla para la generación de números aleatorios
        srand(static_cast<unsigned int>(time(0)));

        // Solicita el número de lanzamientos de dardos
        std::cout << "Enter the number of tosses: ";
        std::cin >> number_of_tosses;
    }

    // Transmite el número de lanzamientos a todos los procesos
    MPI_Bcast(&number_of_tosses, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Calcula el número de lanzamientos que manejará cada proceso
    long long int local_tosses = number_of_tosses / world_size;

    // Genera lanzamientos y cuenta cuántos caen dentro del círculo
    srand(static_cast<unsigned int>(time(0) + world_rank)); // Semilla única por proceso
    for (long long int toss = 0; toss < local_tosses; toss++) {
        double x = random_double();
        double y = random_double();
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1.0) {
            local_number_in_circle++;
        }
    }

    // Suma los resultados de todos los procesos en el proceso 0
    MPI_Reduce(&local_number_in_circle, &global_number_in_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // El proceso 0 calcula la estimación de π y la imprime
    if (world_rank == 0) {
        double pi_estimate = 4.0 * static_cast<double>(global_number_in_circle) / number_of_tosses;
        std::cout << "Estimated value of pi: " << pi_estimate << std::endl;
    }

    MPI_Finalize();
    return 0;
}
