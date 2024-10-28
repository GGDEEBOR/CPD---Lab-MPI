/*
Utilice MPI para implementar el programa de histograma que se analiza en la Sección 2.7.1. 
Haga que el proceso 0 lea los datos de entrada y los distribuya 
entre los procesos. Haga que el proceso 0 imprima también el histograma.
*/

#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

int Find_bin(float value, float min_meas, float max_meas, int bin_count) {
    float bin_width = (max_meas - min_meas) / bin_count;
    int bin = (int)((value - min_meas) / bin_width);
    if (bin == bin_count) bin--; 
    return bin;
}



int main(int argc, char* argv[]) {
    int process_id, process_count, data_count = 20;
    float min_meas = 0.0, max_meas = 5.0;
    int bin_count = 5;
    vector<float> data;
    
    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    int local_data_count = data_count / process_count;
    vector<int> bin_counts(bin_count, 0);
    vector<int> local_bin_counts(bin_count, 0);

    // Inicialización de datos en el proceso 0 y distribución
    if (process_id == 0) {
        data = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3,
                4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};

        // Distribución de datos entre procesos
        for (int i = 1; i < process_count; i++) {
            MPI_Send(data.data() + i * local_data_count, local_data_count, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        data.resize(local_data_count);
        MPI_Recv(data.data(), local_data_count, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < local_data_count; i++) {
        int bin = Find_bin(data[i], min_meas, max_meas, bin_count);
        local_bin_counts[bin]++;
    }

    // Recolección de resultados en el proceso 0
    MPI_Reduce(local_bin_counts.data(), bin_counts.data(), bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (process_id == 0) {
        cout << "Histograma final:\n";
        for (int i = 0; i < bin_count; i++) {
            cout << "Bin " << i << ": " << bin_counts[i] << endl;
        }
    }

    // Finalización de MPI
    MPI_Finalize();
    return 0;
}