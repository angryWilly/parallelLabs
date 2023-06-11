#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include <mpi.h>


double IntegralFunction(const double x)
{
    return 2 * sin(x) + 1;
}

double IntegrateMidRectangle(const double a, const double b, const int n, const int size, const int rank)
{
    double sum = 0;
    const double h = (b - a) / n;
	for (int i = rank; i < n; i += size)
	{
		const double x = h * (i + 0.5) + a;
		sum += IntegralFunction(x);
	}

    return 	sum *= h;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const double A = M_PI_2;
    const double B = M_PI;
    int N = 100000000;

    double startTime = MPI_Wtime();
    double localSum = IntegrateMidRectangle(A, B, N, size, rank);

    double groupSum = 0;
    MPI_Reduce(&localSum, &groupSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double endTime = MPI_Wtime();

    if (rank == 0)
    {
        std::cout << "Result = " << groupSum << std::endl;
        const double resultTime = endTime - startTime;
        std::cout << "Time (parallel) = " << resultTime << "s" << std::endl;

        const double seqStartTime = MPI_Wtime();
        const double seqSum = IntegrateMidRectangle(A, B, N, 1, 0);
        const double seqEndTime = MPI_Wtime();

        std::cout << "Result = " << seqSum << std::endl;
        const double resultSeqTime = seqEndTime - seqStartTime;
        std::cout << "Time (sequential) = " << resultSeqTime << "s" << std::endl;

        std::cout << "Boost = " << resultSeqTime / resultTime;
    }

    MPI_Finalize();
    return 0;
}
