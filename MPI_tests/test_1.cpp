#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mpi.h>

using namespace std;

int* createArray() {
    srand((unsigned)time(NULL));
    int* Arr = new int[10000];
    for (int i = 0; i < 10000; i++) {
        Arr[i] = rand() % 101;
    }
    return Arr;
}

int main(int argc, char** argv)
{
    int rank, size, i, root = 0;
    double timeStart, timeEnd;
    int howMuch = 1;
    int* values = new int[10000];
    int* recived = new int[10000];


    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	//pobranie czasu 
    if (rank == 0) {
        timeStart = MPI_Wtime();
    }
	//uzupelnienie losowymi wartoscia od 0 do 100
    values = createArray();
    if (rank == root) {
        int tag = 0;
		//wysylanie do pozostalych procesorow 
        for (i = 1; i < size; i++)
            MPI_Send(values, 10000, MPI_INT, i, tag, MPI_COMM_WORLD);
    }
    else {
		//odbieranie tablicy 
        MPI_Recv(recived, 10000, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        int result = 0;
        for (i = 0; i < 10000; i++) {
			//sumowanie
            result += recived[i];
        }
        printf("Procesor: %d, Result: %d\n", rank, result);
    }

    if (rank == 0) {
		//pobieranie czasu koncowego
        timeEnd = MPI_Wtime();
        printf("Procesorow: %d\n", size);
		//roznica 
        double time = timeEnd - timeStart;
        cout << "Czas: " << time << endl;
    }
    MPI_Finalize();
    return 0;
}
