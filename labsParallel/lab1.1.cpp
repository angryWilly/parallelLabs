#include <mpi.h>
#include <iostream>

using namespace std;
void get_message(int message, const int size, const int rank, const int tag, MPI_Status& status)
{

	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
		{
			message = i * 2;
			MPI_Send(&message, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(&message, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		cout << "Process " << rank << ": " << message << endl;
	}
}

int main(const int argc, char** argv)
{
	int size, rank;
	int tag = 42;

	int message = 0;
	MPI_Status status;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	get_message(message, size, rank, tag, status);

	MPI_Finalize();

	return 0;
}
