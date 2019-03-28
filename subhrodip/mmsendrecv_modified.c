#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc,char **argv) {
	int data=10000,rank,size;
	double start,finish;
	char ch;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	srand(time(NULL));
	printf("hi\n");
	for(int k=0;;) {
		if(rank==0) {	
			printf("\nDo you want to continue?");
			int stop=0;
			while(!stop) {
				scanf("%c",&ch);
				if(ch=='t') {
					for(int i=0;i<size;i++) {
		                		if(rank==i)
                		        		continue;
                				int d=rand()%5+1;
						printf("\nAmount of %d is sent from process %d to process %d . \n\n",d,rank,i);
                				MPI_Send(&d,1,MPI_INT,i,0,MPI_COMM_WORLD);
                				data-=d;
                				MPI_Recv(&d,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
						printf("\nAmount of %d is recieved by process %d from process %d. \n\n",d,rank,i);
                				data+=d;
        				}
					printf("\nProcess %d FINAL SUM=%d\n\n",rank,data);
					break;
				}
				else  
					continue;
				printf("\nProcess %d FINAL SUM=%d\n\n",rank,data);	
			}		
		}
		else {
			for(int i=0;i<size;i++) {
                                if(rank==i)
                                        continue;
                                int d=rand()%5+1;
				printf("\nAmount of %d is sent from process %d to process %d . \n\n",d,rank,i);
                                MPI_Send(&d,1,MPI_INT,i,0,MPI_COMM_WORLD);
                                data-=d;
                                MPI_Recv(&d,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
				printf("\nAmount of %d is recieved by process %d from process %d. \n\n",d,rank,i);
                                data+=d;
			}
                	printf("\nProcess %d FINAL SUM=%d\n\n",rank,data);
			printf("Hi");
		}
	}	
	finish=MPI_Wtime();
	MPI_Finalize();
	printf("\nParallel Elapsed time for process %d: %f seconds\n",rank,finish-start);
	return 0;
}
