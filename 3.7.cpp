#include <mpi.h>
#include <iostream>
#include <ctime>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 2) {
        if (world_rank == 0) {
            std::cerr << "Este programa requiere exactamente 2 procesos." << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    const int PING_PONG_LIMIT = 10000;
    int ping_pong_count = 0;
    int partner_rank = (world_rank == 0) ? 1 : 0;

    // Medición del tiempo usando clock
    clock_t start_clock = clock();
    while (ping_pong_count < PING_PONG_LIMIT) {
        if (world_rank == 0) {
            // Si es el proceso 0 y es su turno, envía y luego recibe
            if (ping_pong_count % 2 == 0) {
                MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
                MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        else {
            // Si es el proceso 1 y es su turno, recibe y luego envía
            if (ping_pong_count % 2 == 1) {
                MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
            }
        }
        ping_pong_count++;
    }
    clock_t end_clock = clock();
    double time_clock = static_cast<double>(end_clock - start_clock) / CLOCKS_PER_SEC;

    if (world_rank == 0) {
        std::cout << "Tiempo medido con clock: " << time_clock << " segundos" << std::endl;
    }

    // Medición del tiempo usando MPI_Wtime
    ping_pong_count = 0;
    double start_time = MPI_Wtime();
    while (ping_pong_count < PING_PONG_LIMIT) {
        if (world_rank == 0) {
            if (ping_pong_count % 2 == 0) {
                MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
                MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        else {
            if (ping_pong_count % 2 == 1) {
                MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
            }
        }
        ping_pong_count++;
    }
    double end_time = MPI_Wtime();
    double time_mpi_wtime = end_time - start_time;

    if (world_rank == 0) {
        std::cout << "Tiempo medido con MPI_Wtime: " << time_mpi_wtime << " segundos" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
