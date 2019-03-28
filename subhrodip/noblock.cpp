#include<conio.h>
#include<iostream>

using namespace std;

int main()
{
	while(1)
	{
		cout << "hi\n" ;
		sleep(1);
		if(kbhit())
		{
			break;
		}
	}
}
