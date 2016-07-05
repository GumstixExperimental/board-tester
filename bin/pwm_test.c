#include"pwmlib.h"
#include<stdio.h>
int main()
{
	int npwms,i;
	pwmStatus **pwms;
	npwms=initPwms(pwms);
	printf("there are %d pwms.\n",npwms);
	for(i=0;i<npwms;i++)
		printf("%s\n",pwms[i]->path);
	return 0;
}
