#include <iostream>
#include <mpi.h>

using namespace std;
int get_next_process(const int rank, const int size)
{
	return rank < size - 1 ? rank + 1 : 0;
}

int get_previous_process(const int rank)
{
	return rank > 0 ? rank - 1 : 0;
}

void send_to_next_process(const int size, const int rank, const int tag, int& message)
{
	message += rank;
	const int next = get_next_process(rank, size);
	MPI_Send(&message, 1, MPI_INT, next, tag, MPI_COMM_WORLD);

	if (rank == 0)
	{
		cout << "Process 0:";
	}
	cout << " sent " << message << endl;
}

void receive_from_last_process(const int rank, const int tag, const int source, int& message, MPI_Status& status)
{
	MPI_Recv(&message, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
	cout << "Process " << rank << ": " << "received " << message;
}

int main(int argc, char** argv)
{
	srand(time(nullptr));

	int size;
	int rank;
	int tag = 42;
	int message = rand() % 10;
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0)
	{
		send_to_next_process(size, rank, tag, message);
		receive_from_last_process(rank, tag, size - 1, message, status);
	}
	else
	{
		receive_from_last_process(rank, tag, get_previous_process(rank), message, status);
		send_to_next_process(size, rank, tag, message);
	}

	MPI_Finalize();

	return 0;
}
