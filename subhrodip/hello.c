#include<stdio.h>
#include<mpi.h>
int main(int argc,char** argv){

MPI_Init(NULL,NULL);
int world_size;
MPI_Comm_size(MPI_COMM_WORLD,&world_size);
int rank;
MPI_Comm_rank(MPI_COMM_WORLD,&rank);
char processor_name[MPI_MAX_PROCESSOR_NAME];
int name_length;
MPI_Get_processor_name(processor_name,&name_length);
printf("Hello World from Processor %s,rank %d,out of %d processors",processor_name,rank,world_size);
MPI_Finalize(); 
}

