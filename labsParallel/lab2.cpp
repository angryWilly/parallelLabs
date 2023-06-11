#include <iostream>
#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = 10000000;
    double* x = 0;
    double* y = 0;

    if (rank == 0)
    {
        std::cout << "Processes : " << size << std::endl;
        x = new double[n];
        y = new double[n];

        //srand(time(nullptr));
        for (int i = 0; i < n; ++i) {
            x[i] = double(rand()) / RAND_MAX;
            y[i] = double(rand()) / RAND_MAX;
        }
    }

    const int local_size = n / size;
    double* local_x = new double[local_size];
    double* local_y = new double[local_size];

    MPI_Scatter(x, local_size, MPI_DOUBLE,
        local_x, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Scatter(y, local_size, MPI_DOUBLE,
        local_y, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	double parallel_process_time = MPI_Wtime();

    double local_result = 0;
    for (int i = 0; i < local_size; ++i) {
        local_result += local_x[i] * local_y[i];
    }

    double global_result;
    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_MIN
        , 0, MPI_COMM_WORLD);

    double end_parallel_time = MPI_Wtime() - parallel_process_time;

    if (rank == 0) {
        std::cout << "Processes result : " << global_result << std::endl;
        std::cout << "Processes time : " << end_parallel_time << std::endl;

        double root_procces_time = MPI_Wtime();

        double sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += x[i] * y[i];
        }
        std::cout << "Root result : " << sum << std::endl;


        double end_root_time = MPI_Wtime() - root_procces_time;
        std::cout << "Root time : " << end_root_time << std::endl;
        std::cout << "Speeding up : " << end_root_time / end_parallel_time << std::endl;

        delete[] x;
        delete[] y;
    }

    delete[] local_x;
    delete[] local_y;

    MPI_Finalize();
    return 0;
}
