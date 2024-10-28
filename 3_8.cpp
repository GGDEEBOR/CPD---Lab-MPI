#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

void merge(vector<int>& v1, vector<int>& v2, vector<int>& result) {
    size_t i = 0, j = 0;
    result.clear();
    while (i < v1.size() && j < v2.size()) {
        if (v1[i] < v2[j]) {
            result.push_back(v1[i]);
            i++;
        } else {
            result.push_back(v2[j]);
            j++;
        }
    }
    while (i < v1.size()) result.push_back(v1[i++]);
    while (j < v2.size()) result.push_back(v2[j++]);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, comm_sz, n;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (rank == 0) {
        // Leer el valor de n en el proceso 0 y enviarlo a todos los procesos
        cout << "Ingrese el número total de elementos a ordenar: ";
        cin >> n;
    }

    // Broadcast para enviar el valor de n a todos los procesos
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = n / comm_sz; // Cada proceso maneja n / comm_sz elementos

    // Generar números aleatorios y llenarlos en un vector local
    vector<int> local_data(local_n);
    srand(time(0) + rank);
    for (int i = 0; i < local_n; ++i) {
        local_data[i] = rand() % 100;
    }

    // Ordenar el vector local
    sort(local_data.begin(), local_data.end());

    int step = 1;
    while (step < comm_sz) {
        if (rank % (2 * step) == 0) {
            if (rank + step < comm_sz) {
                // Recibir datos del proceso vecino
                int neighbor_size;
                MPI_Recv(&neighbor_size, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                vector<int> neighbor_data(neighbor_size);
                MPI_Recv(neighbor_data.data(), neighbor_size, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                // Combinar con los datos locales
                vector<int> merged_data;
                merge(local_data, neighbor_data, merged_data);
                local_data = merged_data; // Actualizar datos locales después del merge
            }
        } else {
            // Enviar datos al proceso vecino
            int neighbor = rank - step;
            int local_size = local_data.size();
            MPI_Send(&local_size, 1, MPI_INT, neighbor, 0, MPI_COMM_WORLD);
            MPI_Send(local_data.data(), local_size, MPI_INT, neighbor, 0, MPI_COMM_WORLD);
            break; // Termina después de enviar los datos
        }
        step *= 2;
    }

    if (rank == 0) {
        cout << "Arreglo ordenado: ";
        for (const int& val : local_data) {
            cout << val << " ";
        }
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}