#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
int main(int argc, char* argv[])
{

	struct timeval tv;
	time_t t;
	t = gettimeofday(&tv,NULL);
	printf("time of day = %gsec\n", t);
	return 0;
}
