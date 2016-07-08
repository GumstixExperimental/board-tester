#ifndef __PWMLIB_H__
#define __PWMLIB_H__

#define DEFAULT_PERIOD 50000

#include<stdio.h>
#include<dirent.h>

#define PWMDIR "/sys/class/pwm"

typedef struct pwmStatus_t{
	char *path;
	int period;
	int duty;
	int enable;
	}pwmStatus;
	
int initPwms(pwmStatus ***pwms);
int closePwms(pwmStatus **pwms, int npwms);
int getPwm(pwmStatus *pwm);
int setPwm(pwmStatus *pwm);

#endif
