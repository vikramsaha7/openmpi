#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include<assert.h>
#include<pthread.h>
#include<time.h>
#include<termios.h>
#include <unistd.h>
#include<poll.h>

#define MAX_TRANSFER 50     //maximum transfer amount at an instant
#define CONSTANT 97        

int money=10000,id,p;
pthread_mutex_t count_mutex;

int set=0;

int abc = 0,xyz=17;
int one=1,two=0;

/*Random function for selecting node*/
int randsend(int n)
{
	if(one == 1)
	{
		abc=abc+1;
		abc=abc%n;
		one=0;
		two=1;
	}
	else if(two==1)
	{
		abc=abc+2;
		abc=abc%n;
		one =1;
		two=0;
	}
	return (abc);
}

int onep=1,twop=0;
int modu=10;

/*Random function for selecting sending_amount*/
int randmoney(int n)
{
        if(onep == 1)
        {
        xyz = xyz + modu;
		modu++;
		xyz = xyz%modu;
		modu = modu % CONSTANT;
                onep=0;
                twop=1;
		return (xyz%n);
        }
        else if(twop ==1)
        {
                xyz = xyz - modu;
		if(xyz <=0)
		{
			xyz = xyz+CONSTANT;
		}
		modu++;
                xyz = xyz%modu;
                modu = modu % CONSTANT;
                onep= 1;
                twop=0;
        	return (xyz%n);
	}
}


void *Send_Func_For_Thread(void *arg)
{
	MPI_Request req;
	double i,j,k,x;
	while(1)                       //execute continuosly
	{
		if(set==1)                 //if polled then stop sending (polling will make set=1)
			break;
		x = id;
		do
		{
			x=randsend(p);         //select reciever node             
		}
		while(x==id);
		int d;
		do{
			d = randmoney(MAX_TRANSFER > money ? money:MAX_TRANSFER);              //select random sending amount
		}
		while(d>=money);

		pthread_mutex_lock(&count_mutex);      //deduct money
		money = money-d;
		pthread_mutex_unlock(&count_mutex);

		MPI_Send(&d,1,MPI_INT,x,0,MPI_COMM_WORLD);  //send money

	//	printf("\nSending money = %d   from rank %d  to rank %d . \n\n",d,id,x);
	}

	pthread_exit((void *)NULL);

}

void *Recv_Func_For_Thread(void *arg)
{
	MPI_Status status;
	MPI_Request req;
	int flag;           //will set to true on completion of request
	int d,j;
	while(1)           //exectue continuosly
	{
		MPI_Irecv(&d,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&req);    //Recv from any source, any tag
		MPI_Test(&req,&flag,&status);               //Test for the completion of request

		time_t st = time(NULL);

		while(!flag && difftime(time(NULL),st) < 5)   //wait for 5 second to receive all messages
		{
			//MPI_Irecv(&d,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&req);
			MPI_Test(&req,&flag,&status);
		}

		if(!flag)   //if request not completed cancel it
		{
			MPI_Cancel(&req);
			MPI_Request_free(&req);
			break;
		}

		if(status.MPI_TAG == 7)      //if Recieved message tag==7 then break sending Thread by making set=1
		{
			set = 1;
			continue;
		}

		pthread_mutex_lock(&count_mutex);
		money = money + d;                   //update amount
		pthread_mutex_unlock(&count_mutex);

	//	printf("\nRecieved money from rank %d . \n",status.MPI_SOURCE);
	//	printf("My balance of rank %d is :    %d\n\n",id,money);
	}
	pthread_exit((void *)NULL);

}

int main ( int argc, char *argv[] )
{
	double i; 
	int j,d=0,flag;
	MPI_Request req;
	MPI_Status status;

	srand(time(0));

	pthread_t thread[2];     // array for Send and Recv thread

	int provided,count=0;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_rank ( MPI_COMM_WORLD, &id );
	MPI_Comm_size ( MPI_COMM_WORLD, &p );

     /*Sending & Receiving Thread Starts */
	pthread_create(&thread[0], NULL, Send_Func_For_Thread, (void *)0);  //Send Thread
	pthread_create(&thread[1], NULL, Recv_Func_For_Thread, (void *)1);  //Recieve Thread

    /*Polling Mechanism */

	struct pollfd mypoll = {STDIN_FILENO,POLLIN|POLLPRI };
	int t;
	if(id==0){    // take polling input from node 0 only
		while(1){
			if(poll(&mypoll,1,100)){   // poll with timeout of 100 ms
				scanf("%d",&t);        //accept input
				if(t==1){              //if input is 1
					set=1;             //set=1 will terminate Send Thread
					for(j=1;j<p;j++)
					{
						MPI_Send(&d,1,MPI_INT,j,7,MPI_COMM_WORLD);    //sending message with special tag (7) to initialize (set=1) for all other nodes
					}
					break;	
				}
			}
		}
	}
	pthread_join(thread[0],NULL);    //wait for Send Thread termination
	pthread_join(thread[1],NULL);    //wait for Recv Thread termination

	printf("\nFinal balance of rank %d is %d\n",id,money);


	if(id!=0)
	{
		MPI_Send(&money,1,MPI_INT,0,0,MPI_COMM_WORLD);
	}
	 
	
	if(id==0)
	{
		int totmoney = money;
		while(1)
		{
			MPI_Irecv(&d,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&req);    //Recv from any source, any tag
                	MPI_Test(&req,&flag,&status);               //Test for the completion of request
	
        	        time_t st = time(NULL);
	
        	        while(!flag && difftime(time(NULL),st) < 1)   //wait for 5 second to receive all messages
        	        {
        	                 MPI_Test(&req,&flag,&status);
			}
			
			if(!flag)
			{
                	        MPI_Cancel(&req);
                	        MPI_Request_free(&req);
                	        break;
               		}
			totmoney +=d;
		}
		
		printf("\nTotal Money in system = %d \n\n",totmoney);

	}


	MPI_Finalize();                    //terminate MPI execution environment
	pthread_exit((void *)NULL) ;
}
