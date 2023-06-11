#include <mpi.h>
#include <cstdio>
#include <ctime>
#include <iostream>

using namespace std;
void init_x_vector(const int size, int* x)
{
	std::cout << "Vector x :\n";
	for (int i = 0; i < size; i++)
	{
		x[i] = rand() % 42;
		std::cout << x[i] << " ";
	}
}

void init_A_matrix(const int size, int* A)
{
	cout << "\nMatrix A :\n";
	for (int i = 0; i < size * size; i++)
	{
		A[i] = rand() % 16;
		cout << A[i] << ' ';
		if (i % size == size - 1) cout << "\n";
	}
	cout << endl;
}

int get_multiply_row(const int size, const int* x, const int* a)
{
    int result = 0;
	for (int i = 0; i < size; i++)
	{
		result += a[i] * x[i];
	}

    return result;
}

void test_multiply_matrix(const int size, const int* x, const int* A, int* test)
{
	for (int i = 0; i < size; i++)
	{
		test[i] = 0;
		for (int j = 0; j < size; j++) {
			test[i] += A[i * size + j] * x[j];
		}
	}
}

void result_output(const int size, const int* y, const int* test)
{
	for (int i = 0; i < size; i++)
	{
		cout << "Proof : " << test[i] << "\t y : " << y[i] << "\n";
	}
}

int main(int argc, char** argv)
{
    srand(time(nullptr));

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int* x = new int[size];
    int* A = nullptr;
    int* a = new int[size];
    int* y = nullptr;

    if (rank == 0)
    {
        A = new int[size * size];
        y = new int[size];

        init_x_vector(size, x);

        init_A_matrix(size, A);
    }

    MPI_Bcast(x, size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, size, MPI_INT, a, size, MPI_INT, 0, MPI_COMM_WORLD);

    const int result = get_multiply_row(size, x, a);
    
    cout << "Process " << rank << " :" << result << endl;
    MPI_Gather(&result, 1, MPI_INT, y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        int* test = new int[size];
        test_multiply_matrix(size, x, A, test);

        result_output(size, y, test);

        delete A;
        delete y;
        delete[] test;
    }

    delete[] x;
    delete[] a;

    MPI_Finalize();
}