#include"pwmlib.h"
#include<stdio.h>
#include<stdlib.h>
int main()
{
	pwmStatus** pwml;
	int npwms,i;
	pwmStatus ***pwms;
	pwms=(pwmStatus***)malloc(sizeof(pwmStatus**));
	npwms=initPwms(pwms);
	printf("there are %d pwms.\n",npwms);
	pwml=*pwms;
	for(i=0;i<npwms;i++)
		printf("%s\n",pwml[i]->path);

	char select='s';
	
	while(select!='x')
	{
		printf("Please make a selection (0 - %d to test PWMs 0 to %d, 'x' to exit)\n", npwms-1,npwms-1);
		printf("$>");
		select=getchar();
		if(isdigit(select))
		{
			i=select-'0';
			if(i<npwms)
			{
				printf("\nTesting %s\nEnter period: ", pwml[i]->path);
				scanf("%d",&(pwml[i]->period));
				printf("Enter duty: ");
				scanf("%d",&(pwml[i]->duty));
				pwml[i]->enable=1;
				setPwm(pwml[i]);
				printf("Now running...\nPeriod = %d \t Duty = %d\n",pwml[i]->period,pwml[i]->duty);
				printf("Confirming PWM data...\n");
				getPwm(pwml[i]);
				printf("Actual values:\nPeroid = %d \t Duty = %d \t Enable = %d\n", pwml[i]->period, pwml[i]->duty, pwml[i]->enable);
			}
			else printf("\nInvalid index, %d\n",i);
			while(getchar()!='\n');
		}
		else if(select!='x')("\nInvalid Entry\n");
		
	}
	
	

	return 0;

}
