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
    int rank, ranksent, size, source, dest, tag, i, len, howMuch = 1, sentValue, x = 0;
    char name[20];
    len = 20;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (howMuch > 0) {

        if (rank == 0) {
            tag = 0;
            cout << "Podaj liczbe:" << endl;
            cin >> howMuch;
            
            MPI_Send(&howMuch, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
            if (howMuch < 0) {
                MPI_Finalize();
                return(0);
            }
            MPI_Recv(&sentValue, 1, MPI_INT, size - 1, 1, MPI_COMM_WORLD, &status);
            if (sentValue > 0) {
                printf("Proces %d dostal %d od procesu %d\n", rank, sentValue, status.MPI_SOURCE);
            }
        }

        for (int j = 1; j < size; j++) {
            if (rank == j) {
                dest = 0;
                tag = 0;
                MPI_Recv(&sentValue, 1, MPI_INT, j - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if (sentValue > 0) {
                    printf("Proces %d dostal %d od procesu %d\n", rank, sentValue, status.MPI_SOURCE);
                }

                if (rank != size - 1) {
                    cout << "" << endl;
                    MPI_Send(&sentValue, 1, MPI_INT, j + 1, tag, MPI_COMM_WORLD);
                    if (sentValue < 0) {
                        MPI_Finalize();
                        return(0);
                    }
                }

                if (rank == size - 1) {
                    cout << "" << endl;
                    MPI_Send(&sentValue, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
                    if (sentValue < 0) {
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
