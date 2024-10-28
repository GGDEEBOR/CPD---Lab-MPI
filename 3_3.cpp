#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    int process_id, comm_sz;
    int local_value, received_value;

    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Asignamos a cada proceso un valor local (por simplicidad, igual a su rank)
    local_value = process_id;

    // Estructura de árbol para suma global
    int step = 1;
    while (step < comm_sz) {
        if (process_id % (2 * step) == 0) {
            // Procesos que reciben y suman el valor de sus vecinos
            int source = process_id + step;
            if (source < comm_sz) { // Verificar que el proceso vecino existe
                MPI_Recv(&received_value, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_value += received_value;
            }
        } else if (process_id % step == 0) {
            // Procesos que envían su valor local
            int destination = process_id - step;
            MPI_Send(&local_value, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
        }
        step *= 2;
    }

    if (process_id == 0) {
        cout << "Suma global: " << local_value << endl;
    }

    // Finalización de MPI
    MPI_Finalize();
    return 0;
}