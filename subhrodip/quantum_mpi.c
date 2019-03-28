#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int randint(int n)
{
	if ((n - 1) == RAND_MAX)
	{
    		return rand();
 	}
	else
	{
		long end = RAND_MAX/n;
		assert (end > 0L)
		end *= n;
		int r;
		while((r=rand()) >= end);
		return r%n;
	}
}


int main(int argc,char **argv)
{
	int rank,size,mpi_status;
	mpi_status=MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
	int i,j,nloop;
	int my_local_val;
	my_local_val= (randint(9)+1)*100;   
        printf("My rank : %d, BEFORE--> My_local_val: %ld", rank, my_local_val);
	MPI_Barrier(MPI_COMM_WORLD);
	while( flag != -1)
	{
		if(rank==0)
		{
			for(i=0;i<nloop;i++)
			{
				my_local_val-=QUANTUM;
				total+=QUANTUM;
			}
			for(i=1;i<size;i++)
			{
				for(j=0;j<nloop;j++)
				{
					MPI_Recv(&QUANTUM,1,MPI_INT,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
					total+=QUANTUM;
				}
			}
			printf("From Process %d local_value=%ld,Total = %ld\n",rank,my_local_val,total);
		}
		else
		{
			for(i=0;i<nloop;i++)
                	{
                	        my_local_val-=QUANTUM;
			        printf("From Process %d local_value=%ld\n",rank,my_local_val);
                	        MPI_Send(&QUANTUM,1,MPI_INT,0,1,MPI_COMM_WORLD);
                	}
	
		}
		printf("My rank : %d, AFTER --> My_local_val: %ld, Total: %ld\n", rank, my_local_val, total);
	}
	MPI_Finalize();
	return 0;
}
