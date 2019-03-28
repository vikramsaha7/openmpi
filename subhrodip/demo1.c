#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include<stdlib.h>
int main()
{
	struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
	int t;
	while(1){
		if( poll(&mypoll, 1, 1000) )
		{
			scanf("%d",&t);
			//printf("Read string - %s\n", string);
			if(t==1)
			exit(1);
		}
		else
		{
			puts("Read nothing");
		}
	}

	return 0;
}
