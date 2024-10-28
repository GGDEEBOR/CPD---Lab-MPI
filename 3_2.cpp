#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
using namespace std;

int main(int argc, char* argv[]) {
    int process_id, process_count;
    long long int number_of_tosses, local_tosses;
    long long int number_in_circle = 0;
    long long int global_number_in_circle = 0;

    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    if (process_id == 0) {
        cout << "Ingrese el número total de lanzamientos de dardos: ";
        cin >> number_of_tosses;
    }

    // Se distribuye el número total de lanzamientos a todos los procesos
    MPI_Bcast(&number_of_tosses, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Cada proceso calcula su cantidad local de lanzamientos
    local_tosses = number_of_tosses / process_count;
    if (process_id < number_of_tosses % process_count) {
        local_tosses++;
    }

    // Inicialización de la semilla para el generador de números aleatorios
    srand(time(NULL) + process_id); 

    // Simulación de lanzamientos de dardos
    for (long long int toss = 0; toss < local_tosses; toss++) {
        double x = (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0;
        double y = (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0;
        double distance_squared = x * x + y * y;

        if (distance_squared <= 1.0) {
            number_in_circle++;
        }
    }

    // Sumar los resultados locales en el proceso 0
    MPI_Reduce(&number_in_circle, &global_number_in_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Calcular e imprimir la estimación de π en el proceso 0
    if (process_id == 0) {
        double pi_estimate = 4.0 * global_number_in_circle / static_cast<double>(number_of_tosses);
        cout << "Estimación de π: " << pi_estimate << endl;
    }

    // Finalizar MPI
    MPI_Finalize();
    return 0;
}