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
    int rank, size, dest, tag, i, howMuch = 1, valueSent;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	//petla wykonuje az uzytkownik nie poda wartosci ujemnej 
    while (howMuch > 0) {

        if (rank == 0) {
            tag = 0;
            cout << "Podaj liczbe:" << endl;
			//wartunek petli 
            cin >> howMuch;

            //wysylanie do procesu pierwszego
            MPI_Send(&howMuch, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
            if (howMuch < 0) {
                MPI_Finalize();
                return(0);
            }
			//otrzymywanie od ostatniego procesu 
            MPI_Recv(&valueSent, 1, MPI_INT, size - 1, 1, MPI_COMM_WORLD, &status);
            if (valueSent > 0) {
                printf("Proces %d dostal %d od procesu %d\n", rank, valueSent, status.MPI_SOURCE);
            }
        }

        for (i = 1; i < size; i++) {
            if (rank == i) {
                dest = 0;
                tag = 0;
				//otrzymywanie od wszystkich procesow pokolei - z wyjatkiem pierwszego 
                MPI_Recv(&valueSent, 1, MPI_INT, i - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if (valueSent > 0) {
					//jesli wartosci powyzej zera to piszemy  
                    printf("Proces %d dostal %d od procesu %d\n", rank, valueSent, status.MPI_SOURCE);
                }

                if (rank != size - 1) {
                    cout << "" << endl;
					//po otrzymaniu przeslanie wartosci dalej 
                    MPI_Send(&valueSent, 1, MPI_INT, i + 1, tag, MPI_COMM_WORLD);
                    if (valueSent < 0) {
						//sprawdzenie warunku 
                        MPI_Finalize();
                        return(0);
                    }
                }

                if (rank == size - 1) {
                    cout << "" << endl;
					//wyslanie do zerowego procesu 
                    MPI_Send(&valueSent, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
                    if (valueSent < 0) {
						//sprawdzenie warunku 
                        MPI_Finalize();
                        return(0);
                    }
                }
            }
        }
    }
    MPI_Finalize();
    return(0);
}
