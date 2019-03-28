#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main ( int argc, char *argv[] )

{
  int id;
  int p,deduct;
  int *arr;
  int money,amt,count =0,max,min,mini,maxi,flag=0,i;
  double j;

  MPI_Init ( &argc, &argv );
  MPI_Comm_rank ( MPI_COMM_WORLD, &id );
  MPI_Comm_size ( MPI_COMM_WORLD, &p );
  
  arr = (int*)malloc(sizeof(int)*p);
  money = (1+ (rand() % 9))*100;
  
  printf("My initial money is %d and my rank is %d. \n",money,id);

  do
  {
	 
  	if(id==0)
  	{
  		for(i=0;i<p;i++)
  		{
  			if(i==0)
  			{
  				arr[i]=money;
  			}
  			else
  			{
  				MPI_Recv(&amt,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  				arr[i] = amt;
  			}
  		}

  		max = -1;
  		min = 10000;
  		maxi=-1;
  		mini=-1;

  		for(i=0;i<p;i++)
  		{
  			if(arr[i]>max)
  			{
  				max = arr[i];
  				maxi = i;
  			}

  			if(arr[i]<min)
  			{
  				min = arr[i];
  				mini = i;
  			}
  		}

  		deduct = (1 + rand()%9)*10;

  		max = max-deduct;
  		min = min + deduct;

  		if(maxi == 0)
  			money = max;
  		else if(mini == 0)
  			money = min;

		printf("Amount of %d has been sent from rank %d to rank %d. \n",deduct,maxi,mini);
		
  		for(i=1;i<p;i++)
  		{
  			if(i == maxi)
  				MPI_Send(&max, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  			else if(i == mini)
  				MPI_Send(&mini, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  			else
  				MPI_Send(&arr[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			//MPI_Barrier(MPI_COMM_WORLD);
  		}
		count++ ;

 	       for(j=0;j<30000000;j=j+0.1);

        	if(count%5 == 0)
        	{
                	printf("Enter 1 to stop else 0 to continue : \n");
                	scanf("%d",&flag);
        	}	

  	}

  	else
  	{
  		MPI_Send(&money, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  		MPI_Recv(&amt,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  		money = amt;
		
		printf ("The amount of money with me of rank %d is :       %d\n",id,money);
  	}
	
	count++ ;

	for(j=0;j<30000000;j=j+0.1);

	if(count%5 == 0)
	{
		printf("Enter 1 to stop else 0 to continue : \n");
		scanf("%d",&flag);
	}
  }
  while(flag!=1);
  MPI_Finalize();
}
