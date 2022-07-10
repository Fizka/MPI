#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include <iostream>

using namespace std; 

//struktura ksiazka 
typedef struct {
	char title[20];
	char author[20];
	char publisher[20];
	int year;
	int pages;
} Book;


int main(int argc, char** argv)
{
	int rank, size, r;
	double timeStart, timeEnd; 
	MPI_Status status;
	MPI_Datatype new_type;
	MPI_Datatype type[5] = { MPI_CHAR,MPI_CHAR, MPI_CHAR, MPI_INT, MPI_INT };
	int blen[5] = { 20, 20, 20,1, 1 };
	MPI_Aint disp[5];
	MPI_Aint base, addr;
	Book tabrecord, tabrecieved;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0)
		printf("MPI_Type_create_struct()\n");

	MPI_Get_address(&tabrecord, &base);
	MPI_Get_address(&(tabrecord.title), &addr);
	disp[0] = addr - base;
	MPI_Get_address(&(tabrecord.author), &addr);
	disp[1] = addr - base;
	MPI_Get_address(&(tabrecord.publisher), &addr);
	disp[2] = addr - base;
	MPI_Get_address(&(tabrecord.year), &addr);
	disp[3] = addr - base;
	MPI_Get_address(&(tabrecord.pages), &addr);
	disp[4] = addr - base;


	MPI_Type_create_struct(5, blen, disp, type, &new_type);
	MPI_Type_commit(&new_type);

//pomiar czasu 
	timeStart = MPI_Wtime();
	if (rank != 0)
	{
		strcpy(tabrecieved.title, "The best book ever!");
		strcpy(tabrecieved.author, "Jan Kowlaski");
		strcpy(tabrecieved.publisher, "XXX");
		tabrecieved.year = 2234;
		tabrecieved.pages = 23;
		printf("In process %d author name is %s book title is %s\n", rank, tabrecieved.author, tabrecieved.title);
		cout << "publisher " << tabrecieved.publisher << " year: "
			<< tabrecieved.year << " pages: " << tabrecieved.pages << endl; 
		MPI_Recv(&tabrecieved, 1, new_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("In process %d author name is %s book title is %s\n", rank, tabrecieved.author, tabrecieved.title);
		cout << "publisher " << tabrecieved.publisher << " year: "
			<< tabrecieved.year << " pages: " << tabrecieved.pages << endl; 
	}
	else
	{
		strcpy(tabrecord.title, "The worst book ever!");
		strcpy(tabrecord.author, "Anna Nowak");
		strcpy(tabrecord.publisher, "ZZZ");
		tabrecord.year = 34;
		tabrecord.pages = 3456;
		for (r = 1; r < size; r++)
			MPI_Send(&tabrecord, 1, new_type, r, 0, MPI_COMM_WORLD);	
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
	{
		//pomair czasu 
		timeEnd = MPI_Wtime();
		cout << "Time: " << timeEnd - timeStart << endl;

	}
	MPI_Finalize();
	return 0;
}
