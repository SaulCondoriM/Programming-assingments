#include <mpi.h>
#include <iostream>
#include <vector>

// Función para calcular en qué bin cae cada dato
int get_bin(double value, double min_meas, double max_meas, int bin_count) {
    double bin_width = (max_meas - min_meas) / bin_count;
    int bin = (value - min_meas) / bin_width;
    if (bin >= bin_count) bin = bin_count - 1;  // Si está en el límite superior
    return bin;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int data_count;
    double min_meas = 0.0, max_meas = 5.0;
    int bin_count = 5;
    std::vector<double> data;
    std::vector<int> local_bins(bin_count, 0);
    std::vector<int> global_bins(bin_count, 0);

    if (world_rank == 0) {
        // Datos de ejemplo
        data = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 
                3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
        data_count = data.size();
    }

    // Distribuir el tamaño del conjunto de datos
    MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_size = data_count / world_size;
    std::vector<double> local_data(local_size);

    // Dividir los datos entre los procesos
    MPI_Scatter(data.data(), local_size, MPI_DOUBLE, local_data.data(), local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calcular el histograma local
    for (double value : local_data) {
        int bin = get_bin(value, min_meas, max_meas, bin_count);
        local_bins[bin]++;
    }

    // Reducir los histogramas locales al proceso 0
    MPI_Reduce(local_bins.data(), global_bins.data(), bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        // Imprimir el histograma global
        std::cout << "Histogram:" << std::endl;
        for (int i = 0; i < bin_count; i++) {
            std::cout << "Bin " << i << ": " << global_bins[i] << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
