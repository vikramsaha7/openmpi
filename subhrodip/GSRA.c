#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define bool int
#define F(i,a,b) for(int i=a;i<b;i++)
#define false 0
#define true 1
#define RED 0 
#define GREEN 1
#define MAXNODES 100000
#define null 0

//declaration section
typedef struct node {
	int state;			//state of the channel
	bool allgreen;			//variable to check whether all incoming channel markers have been received  
	int channel_status[MAXNODES];	//status of incoming channels
	int channel_record[MAXNODES];	//record buffer of incoming channels
	int channel_trans[MAXNODES];
}node;

node g_node;    //A node to represent each processor 
int data=10000; //initialize the data to $10,000. 
int rank,size;  //determines the rank and size of processor
MPI_Status status;
char MARKER='m'; 
int initiator=0;//default initiator node
int d;
int sum=0;

//function prototypes
void display();
void perform_money_transaction_system();


void display() {
	printf("\t\t");	
	F(i,0,size) {
                if(rank==i) {
			printf("0\t");
                        continue;
		}
                else 
                        printf("%d\t",g_node.channel_trans[i]);
        }
	printf("\n\n");
	MPI_Barrier(MPI_COMM_WORLD);
}

void perform_money_transaction_system() {
	F(i,0,size) {
		 if(rank==i)
			continue;
                 d=rand()%10+1;
		 //g_node.channel_trans[i]=d;
                 MPI_Send(&d,1,MPI_INT,i,0,MPI_COMM_WORLD);
                 data-=d;
		 g_node.channel_trans[i]=d;               
                 MPI_Recv(&d,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
                 data+=d;                                     
         }
}

//main code begins here
int main(int argc,char **argv) {

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Request request;
    	MPI_Status status;
	//initialize
	F(i,0,size) {
		g_node.channel_status[i]=RED;
		g_node.channel_record[i]=null;
	}
	g_node.state=data;
	srand(time(NULL));

	while(true) {
		//perform_money_system_transaction
		perform_money_transaction_system();
		scanf("%c",&MARKER);
		if(rank==0) {
			printf("PROCESSOR\tSTATE\t\t");
                        for(int i=0;i<size;i++)
                             printf("P%d\t",i);

		}		
		//Assuming, ( N X N ) INTERCONNECTION NETWORK
		
		//SENDING MARKER
		F(i,0,size) {
			if(rank==i)
				continue;
			else if(rank==initiator) {
				//start recording on incoming channels,make incoming channels wide open
				F(j,1,size) 
					g_node.channel_status[j]=RED;
			}	
			//send markers to other processes		
			MPI_Send(&MARKER,1,MPI_CHAR,i,0,MPI_COMM_WORLD);
			g_node.state=data;	//record own state
		}

		//RECEIVING MARKER
		F(i,0,size) {
			if(rank==i)
				continue;
			//received marker for the first time
		
			MPI_Recv(&MARKER,1,MPI_CHAR,i,0,MPI_COMM_WORLD,&status);
			//if marker is received for the first time, record own state
			if(g_node.channel_status[i]==RED) {
				g_node.state=data;	//record state
				g_node.channel_record[rank]=0;	//intiate NULL
				g_node.channel_status[i]=GREEN; //close the channel
			}
		
			else { //if received earlier
				
				if(rank==initiator) { 
					if(g_node.channel_status[i]==GREEN)
						;
					//check if all the incoming channels are recorded
						F(j,0,size) {
							if(j==initiator)
								continue;
							if(g_node.channel_status[j]==RED) {
								g_node.allgreen=false;
								break;
							}
						}
					//if all are green we are done
					if(g_node.allgreen) 
						break;
					else {
						g_node.allgreen=true;	
						break;
					}
				}
				else {
					//for non-initiator processes check if all are green
					F(j,0,size) {
						if(rank==j)
							continue;
						if(g_node.channel_status[j]==RED) {
							g_node.allgreen=false;
							break;
						}    
					}
					if(!g_node.allgreen)
						continue;
					else {
						g_node.allgreen=true;
						break;
					}
				}
			}
			
		}
		sum=0;
		
		if(rank==0) {
                                /*printf("PROCESSOR\tSTATE\t\t");
                                for(int i=0;i<size;i++)
                                        printf("P%d\t",i); */
				for(int i=0;i<size;i++) {
		                        sum+=g_node.channel_trans[i];
                		}
                		printf("\n");
                		for(int i=0;i<size;i++)
                        		g_node.state-=(g_node.channel_trans[i]);
                		sum+=g_node.state;
                		printf("\n");
                		printf("%d\t\t%d",rank,g_node.state);
                		display();
                		printf("\n");


                }
		if(rank!=0) {
			for(int i=0;i<size;i++) {
				sum+=g_node.channel_trans[i];
			}
			printf("\n");
			for(int i=0;i<size;i++) 
				g_node.state-=(g_node.channel_trans[i]);
			sum+=g_node.state;
			printf("\n");
			printf("%d\t\t%d",rank,g_node.state);
			display();
			printf("\n");
			MPI_Send(&sum,1,MPI_INT,0,0,MPI_COMM_WORLD);
		//MPI_Barrier(MPI_COMM_WORLD);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank==0) {
			int final_sum=0,tsum;
                        final_sum+=sum;
                        for(int i=1;i<size;i++)
                        {
                                MPI_Recv(&tsum,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
                                final_sum+=tsum;
                        }
                        printf("\nTotal Sum = %d\n",final_sum);

		}	
		scanf("%c",&MARKER,&initiator);
	}
	printf("\n");
	MPI_Finalize();
	return 0;
}
