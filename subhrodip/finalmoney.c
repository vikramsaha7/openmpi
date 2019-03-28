#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include<assert.h>
#include<pthread.h>
#include<time.h>
#include<termios.h>
#include <unistd.h>
#include<poll.h>

#define MAX_TRANSFER 50

int money=10000,id,p;
pthread_mutex_t count_mutex;

int set=0;

int randint(int n)
{
	if((n-1) == RAND_MAX) {
		return rand();
	}
	else
	{
		long end = RAND_MAX/n;
		assert(end > 0L);
		end*=n;
		int r;
		while((r=rand()) >= end);

		return (r%n)+1;
	}
}

void *Send_Func_For_Thread(void *arg)
{
	MPI_Request req;
	double i,j;
	while(1)
		//for(j=0;j<100;j++)
	{
		if(set==1)
			break;
		//for(i=0;i<50000000;i=i+0.1);
		//sleep(1);
		int x = id;
		do{
			x=(randint(p)-1);
		}
		while(x==id);
		int d;
		do{
			 d = randint(MAX_TRANSFER);
		}
		while(d>money);

		pthread_mutex_lock(&count_mutex);
		money = money-d;
		pthread_mutex_unlock(&count_mutex);

		MPI_Send(&d,1,MPI_INT,x,0,MPI_COMM_WORLD);
		printf("\nSending money = %d   from rank %d  to rank %d . \n\n",d,id,x);
	}

	pthread_exit((void *)NULL);

}

void *Recv_Func_For_Thread(void *arg)
{
	MPI_Status status;
	MPI_Request req;
	int flag;
	int d,j;
	while(1)
		//for(j=0;j<5;j++)
	{
		//sleep(1);
		MPI_Recv(&d,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		//MPI_Test(&req,&flag,&status);

		/*time_t st = time(NULL);

		while(!flag && difftime(time(NULL),st) < 10)
		{
			//MPI_Irecv(&d,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&req);
			MPI_Test(&req,&flag,&status);
		}

		if(!flag)
		{
			MPI_Cancel(&req);
			MPI_Request_free(&req);
			break;
		}*/

		if(status.MPI_TAG == 7)
		{
			set = 1;
			continue;
		}

		pthread_mutex_lock(&count_mutex);
		money = money + d;
		pthread_mutex_unlock(&count_mutex);

		printf("\nRecieved money from rank %d . \n",status.MPI_SOURCE);
		printf("My balance of rank %d is :    %d\n\n",id,money);
	}
	pthread_exit((void *)NULL);

}

int main ( int argc, char *argv[] )
{
	double i; 
	int j,d=0;
	MPI_Request req;
	MPI_Status status;

	srand((unsigned)time(NULL));

	pthread_t thread[2];

	int provided,count=0;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_rank ( MPI_COMM_WORLD, &id );
	MPI_Comm_size ( MPI_COMM_WORLD, &p );

	pthread_create(&thread[0], NULL, Send_Func_For_Thread, (void *)0);
	pthread_create(&thread[1], NULL, Recv_Func_For_Thread, (void *)1);

	struct pollfd mypoll = {STDIN_FILENO,POLLIN|POLLPRI };
	int t;
	if(id==0){
		while(1){
			if(poll(&mypoll,1,100)){
				scanf("%d",&t);
				if(t==1){
					set=1;
					for(j=1;j<p;j++)
					{
						MPI_Send(&d,1,MPI_INT,j,7,MPI_COMM_WORLD);
					}
					break;	
				}
			}
		}
	}
	pthread_join(thread[0],NULL);
	sleep(10);
	pthread_kill(thread[1],0);

	//MPI_Barrier(MPI_COMM_WORLD);
	printf("\nFinal balance of rank %d is %d\n",id,money);
	MPI_Finalize();
	pthread_exit((void *)NULL) ;
}
