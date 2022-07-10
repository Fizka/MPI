#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mpi.h>

using namespace std;

//zakres pietra do max num procesora, dla pietra docelowego wykluczane jest pietro startowe  
int losujPietro(int size, int obecny) {
    int random = obecny;
    while (obecny == random) {
        srand((unsigned)time(NULL));
        random = rand() % size;
    }
    return random;
}

//struktura windy i kolejki do niej 
typedef struct {
    int pasazerSkad[100];
    int pasazerGdzie[100];
    int aktywna;
    int numerPasazera;
    int nastepneWolneMiejsce;
} Winda;

int main(int argc, char** argv)
{
    int rank, size, newIle;
    int howMuch = 1;
    const int root = 0;

    MPI_Datatype new_type;
    MPI_Datatype type[5] = { MPI_INT,MPI_INT,MPI_INT,MPI_INT,MPI_INT };
    int blen[5] = {  100,100, 1, 1, 1 };
    MPI_Aint disp[5];
    MPI_Aint base, addr;
    Winda tabrecord, valueSent;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;
    MPI_Get_address(&tabrecord, &base);
    MPI_Get_address(&(tabrecord.pasazerSkad), &addr);
    disp[0] = addr - base;
    MPI_Get_address(&(tabrecord.pasazerGdzie), &addr);
    disp[1] = addr - base;
    MPI_Get_address(&(tabrecord.aktywna), &addr);
    disp[2] = addr - base;
    MPI_Get_address(&(tabrecord.numerPasazera), &addr);
    disp[3] = addr - base;
    MPI_Get_address(&(tabrecord.nastepneWolneMiejsce), &addr);
    disp[4] = addr - base;


    MPI_Type_create_struct(5, blen, disp, type, &new_type);
    MPI_Type_commit(&new_type);
    tabrecord.nastepneWolneMiejsce = 0;
    tabrecord.numerPasazera = 0;
    tabrecord.pasazerGdzie[tabrecord.nastepneWolneMiejsce] = -1;
    tabrecord.pasazerSkad[tabrecord.nastepneWolneMiejsce] = -1;
    tabrecord.aktywna = 0;


    while (howMuch >= 0)
    {
        if (rank == 0)
        {
			//sprawdz czy num obecnego pasazera nie rowna sie max num pasazera
            if (tabrecord.numerPasazera != tabrecord.aktywna) {
                cout << "Jest nowy pasazer? podaj liczbe nowych pasazerow, lub wpisz -1, zeby kontynuowac ";
                cin >> newIle;
                if (newIle != -1) {
                    tabrecord.aktywna = newIle + tabrecord.aktywna;
                }                         
                newIle = 0;
            }
            else if (tabrecord.numerPasazera == tabrecord.aktywna) {
				//jesli num pasazera rowna sie numerow max pasazera to znaczy, ze kolejka jest pusta 
                cout << "Konczymy? Wpisz -1, mamy pasazerow? podaj liczbe nowych pasazerow: ";
                cin >> newIle;
                if (newIle == -1){
					//konczymy 
                    tabrecord.aktywna = newIle;
                }
                else {
                    tabrecord.aktywna = newIle + tabrecord.aktywna;
                }
            }
            else {
                cout << "Wyjscie";
                newIle = -1;
                tabrecord.aktywna = newIle;
            }

            if (newIle > 0) {
                cout << "Dodawanie do kolejki: " << newIle << " pasazerow. " << endl;
            }

            for (int j = 0; j < newIle; j++) {
				//uzupelnianie losowe pietra pasazerow docelowego i startowego 
                int skad = losujPietro(size, -1);
                int dokad = losujPietro(size, skad);

                tabrecord.pasazerGdzie[tabrecord.nastepneWolneMiejsce] = dokad;
                tabrecord.pasazerSkad[tabrecord.nastepneWolneMiejsce] = skad;
                cout << "Chce jechac z " << skad << " na pietro: " << dokad << endl;
				//zmienna przechowujace ifo o nastepnym wolnym miejscu w kolejce 
                tabrecord.nastepneWolneMiejsce++;  

                if (tabrecord.nastepneWolneMiejsce > 99) {
                    tabrecord.nastepneWolneMiejsce = 0;
                }
            }

            if (tabrecord.aktywna - tabrecord.numerPasazera > 2) {
				//jesli w kolejce jest co najmniej trzech pasazerow 
                for (int j = tabrecord.numerPasazera + 2; j < tabrecord.aktywna; j++) {
					//sprawdz gdzie winda jest 
                    int nastepny = tabrecord.pasazerGdzie[tabrecord.numerPasazera];
					//sprawdz gdzie bedzie jechac
                    int dokad = tabrecord.pasazerSkad[tabrecord.numerPasazera + 1];
					//zobaczy czy ktorys z pasaazerow nie jest na tym samym pietrze gdzie winda 
                    if (nastepny == tabrecord.pasazerSkad[j]) {
                        int min;
                        int max;
                        if (nastepny > dokad) {
                            max = nastepny;
                            min = dokad;
                        }
                        else {
                            max = dokad;
                            min = nastepny;
                        }
						//sprawdz czy ten pasazer jedzie na pietro pomiedzy docelowym pitrem a obecnym 
                        if (tabrecord.pasazerGdzie[j] < max && min < tabrecord.pasazerGdzie[j]) {
							//jesli tak to zamieniamy miejsca w kolejce 
                            cout << "Winda bierze po drodze pasazera! Zmiana kolejki!" << endl;
                            cout << "Wsiada z " << tabrecord.pasazerSkad[j]
                                << " jedzie do " << tabrecord.pasazerGdzie[j] << endl;
                            int temp[100];
                            int temp2[100];

                            int y = tabrecord.numerPasazera + 1;
                            int index = 0;
                            while (y < tabrecord.aktywna) {
                               
                                if (y != tabrecord.numerPasazera + 2) {
                                    temp[index] = tabrecord.pasazerGdzie[y];
                                    temp2[index] = tabrecord.pasazerSkad[y];
                                  
                                    index++;
                                }
                                y++;
                            }
							//zamiana 
                            tabrecord.pasazerGdzie[tabrecord.numerPasazera + 1] = tabrecord.pasazerGdzie[j];
                            tabrecord.pasazerSkad[tabrecord.numerPasazera + 1] = tabrecord.pasazerSkad[j];
                           
                            for (int o = tabrecord.numerPasazera + 2; o <= tabrecord.aktywna; o++) {
                                int index_ = 0;
                                tabrecord.pasazerGdzie[o] = temp[index_];
                                tabrecord.pasazerSkad[o] = temp2[index_];
                                index_++;
                            }
                        }
                    }
                }
            }
            
            tabrecord.numerPasazera = 1 + tabrecord.numerPasazera;
        }

        MPI_Bcast(&tabrecord, 1, new_type, 0, MPI_COMM_WORLD);

        howMuch = tabrecord.aktywna;
 
        if (tabrecord.pasazerGdzie[tabrecord.numerPasazera-1] == rank) {

            if (tabrecord.aktywna > 0) {
				//pasazer dojechal na miejsce
                cout << endl;
                cout << "Obslugiwany numer pasazera: " << tabrecord.numerPasazera - 1 << endl;
                cout << "Pasazer dojechal na pietro: ";
                cout << tabrecord.pasazerGdzie[tabrecord.numerPasazera -1] << endl;
                cout << "Pasazer przyjechal z pietra: ";
                cout << tabrecord.pasazerSkad[tabrecord.numerPasazera - 1] << endl;
                cout << endl;
                cout << "Winda wolna, sprawdzanie kolejki..." << endl;

            }
        }

        MPI_Barrier(MPI_COMM_WORLD);  
    }

    MPI_Finalize();
    return 0;
}
