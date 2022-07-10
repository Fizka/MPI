#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv)
{
    int rank, size;
    int howMuch = 1;
    const int root = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	//petla wykonuje sie az uzytkownika nie poda ujemnej wartosci
    while (howMuch >= 0)
    {
        if (rank == 0)
        {
            cout << "Podaj liczbe:";
			//wartosc od ktorej zalezy petla
            cin >> howMuch;
        }
		//rozsylanie wartosci
        MPI_Bcast(&howMuch, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0 && howMuch >= 0)
            cout << "Proces " << rank << " dostal " << howMuch << endl;
		//bariera 
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
