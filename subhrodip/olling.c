#include <stdio.h>
#include <stdlib.h>
#include<assert.h>
#include<pthread.h>
#include <unistd.h>
#include <poll.h>
#include<termios.h>
#define NB_ENABLE 0
#define NB_DISABLE 1
int kbhit()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state){
	struct termios ttystate;
	tcgetattr(STDIN_FILENO, &ttystate);

	if (state==NB_ENABLE)
	{
		ttystate.c_lflag &= ~ICANON;
		ttystate.c_cc[VMIN] = 1;
	}
	else if (state==NB_DISABLE)
	{ttystate.c_lflag |= ICANON;
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}
int main()
{
	char c;
	int i=0;
        nonblock(NB_ENABLE);
	while(!i)
	{
		usleep(1);
		i=kbhit();
		if (i!=0)
		{
			c=fgetc(stdin);
			if (c=='q')
				i=1;
			else
				i=0;
		}

		fprintf(stderr,"%d ",i);
	}
	printf("\n you hit %c. \n",c);
        nonblock(NB_DISABLE);
	return 0;
}
