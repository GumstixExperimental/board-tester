
#include"pwmlib.h"

#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define DEBUG 1

int countPwms(DIR *d, char* path)
{
	char* fname;
	FILE *fp;
	struct dirent *entry;
	int npwm=0;
	char c_npwm[5];
	
	
	while((entry=readdir(d))!=NULL)
	{
		if(!strcmp(entry->d_name,"npwm"))
		{
			fname=(char*)malloc((strlen(path)+strlen(entry->d_name)+1)*sizeof(char));
			sprintf(fname,"%s/%s",path,entry->d_name);

			#if DEBUG
			printf("count %s\n",fname);
			#endif
			
			if(!(fp=fopen(fname,"r")))
				return 0;
			
			fscanf(fp,"%s", c_npwm);
			npwm=atoi(c_npwm);
			fclose(fp);
			free(fname);
			return npwm;
		}
	}
	return 0;
}

void exportPwms(DIR *d, char* path, pwmStatus **pwms, int *index)
{
	char *fname;
	int npwms, i;
	FILE *fp;
	struct dirent *entry;
	
	npwms=countPwms(d,path);
	
	if(!npwms) return;
	
	fname=(char*)malloc((strlen(path)+strlen("export")+1)*sizeof(char));
	sprintf(fname,"%s/%s",path,"export");
	
	for(i=0;i<npwms;i++)
	{
		fp=fopen(fname,"w");
		fprintf(fp, "%d", i);
		fclose(fp);
	}
	while((entry=readdir(d))!=NULL)
	{
		if(!(strncmp(entry->d_name, "pwm", 3)))
		{
			i=*index;
			pwms[i]->path=(char*)malloc((sizeof(path)+sizeof(entry->d_name)+1)*sizeof(char));
			sprintf(pwms[i]->path,"%s/%s",path,entry->d_name);
			pwms[i]->period=DEFAULT_PERIOD;
			pwms[i]->duty=DEFAULT_PERIOD/2;
			pwms[i]->enable=0;
			(*index)++;
		}
	}
	
}

int initPwms(pwmStatus ***pwms)
{
	int pwmCount = 0;
	ssize_t pathLen;
	char lnkPath[2048];
	int pwmIndex=0;
	//PWM root directory
	DIR *d, *subd;

	//Object inside directory
	struct dirent *entry;
	
	char *path;
	int i;
	
	/* First the PWMs must be counted in order to allocate the array of pwmStatus structs */
	d=opendir(PWMDIR);
	if(d)
	{
		while((entry=readdir(d))!=NULL)
		{
		//Find each pwmchip directory
			if(entry->d_type==DT_LNK && strstr(entry->d_name, "pwmchip"))
			{
					path=(char*)malloc((strlen(PWMDIR)+strlen(entry->d_name)+1)*sizeof(char));
					sprintf(path,"%s/%s",PWMDIR,entry->d_name);
					
					//Get real path to pwmchip
					pathLen=readlink(path, lnkPath, 2048);
					lnkPath[pathLen]=0;
					sprintf(lnkPath,"%s%s","/sys",(char*)(lnkPath+5));
					#if DEBUG
					printf("%s -> %s\n",path, lnkPath);
					#endif
					
					//count pwms in chip
					if(subd=opendir(lnkPath))
					{
						pwmCount+=countPwms(subd, lnkPath);
						closedir(subd);
					}
					free(path);
			}
		}
		
		*pwms=(pwmStatus**)malloc(pwmCount*sizeof(pwmStatus*));
		for(i=0;i<pwmCount;i++)
		{
			(*pwms)[i]=(pwmStatus*)malloc(sizeof(pwmStatus));
		}
		
		#if DEBUG
		printf("PWM count: %d\nPWM array allocated\n",pwmCount);
		#endif
		
		rewinddir(d);
		
		while((entry=readdir(d))!=NULL)
		{
		//Find each pwmchip directory
			if(entry->d_type==DT_LNK && strstr(entry->d_name, "pwmchip"))
			{
					path=(char*)malloc((strlen(PWMDIR)+strlen(entry->d_name)+1)*sizeof(char));
					sprintf(path,"%s/%s",PWMDIR,entry->d_name);
					
					//Get real path to pwmchip
					pathLen=readlink(path, lnkPath, 2048);
					lnkPath[pathLen]=0;
					sprintf(lnkPath,"%s%s","/sys",(char*)(lnkPath+5));
					#if DEBUG
					printf("exporting ... %s -> %s\n",path, lnkPath);
					#endif
					
					//export and store PWM file structure
					if(subd=opendir(lnkPath))
					{
						exportPwms(subd, lnkPath, *pwms, &pwmIndex);
						closedir(subd);
					}
					free(path);
			}
		}
		closedir(d);
	}
	return pwmCount;
}

int setPwm(pwmStatus *pwm)
{
	FILE *fp;
	char *path=(char*)malloc((strlen(pwm->path)+16)*sizeof(char));
	if(!pwm->enable)
	{
		sprintf(path,"%s/enable",pwm->path);
		fp=fopen(path,"w");
		if(!fp)
			return 1;
		fprintf(fp,"%d",0);
		fclose(fp);
		
	}
	
	sprintf(path,"%s/period",pwm->path);
	fp=fopen(path,"w");
	if(!fp)
		return 2;
	fprintf(fp,"%d",pwm->period);
	fclose(fp);
	
	sprintf(path,"%s/duty_cycle",pwm->path);
	fp=fopen(path, "w");
	if(!fp)
		return 3;
	fprintf(fp, "%d", pwm->duty);
	fclose(fp);
	
	if(pwm->enable)
	{
		sprintf(path,"%s/enable",pwm->path);
		fp=fopen(path, "w");
		if(!fp)
			return 1;
		fprintf(fp, "%d", 1);
		fclose(fp);
	}
	return 0;
}

int getPwm(pwmStatus *pwm)
{
	FILE *fp;
	char *path=(char*)malloc((strlen(pwm->path)+16)*sizeof(char));

	sprintf(path,"%s/enable",pwm->path);
	fp=fopen(path,"r");
	if(!fp)
		return 1;
	fscanf(fp,"%d",&(pwm->enable));
	fclose(fp);

	sprintf(path,"%s/period",pwm->path);
	fopen(path,"r");
	if(!fp)
		return 2;
	fscanf(fp,"%d",&(pwm->period));
	fclose(fp);
	
	sprintf(path,"%s/duty_cycle",pwm->path);
	fopen(path, "r");
	if(!fp)
		return 3;
	fscanf(fp, "%d", &(pwm->duty));
	fclose(fp);
	
	
	return 0;
}

int closePwms(pwmStatus **pwms int npwms)
{
	FILE* fp;
	char *a;
	char *path;
	char *check;
	int i,j;
	a=0;
	for(i=0;i<npwms;i++)
	{
		path=(char*)malloc((sizeof(pwms[i]->path)+10)*sizeof(char));
		sprintf(path,"%s",pwms[i]->path);
		for(j=0;j<npwms;j++)
		{
			sprintf(check,"pwm%d",j);
			a=strstr(path,check);
			if(a)
			{
				sprintf(a,"unexport");
				break;
			}
		}
		if(!a)
		{
		#if DEBUG
			printf("Did not find \"pwm#\" in \"%s\"\n",pwms[i]->path);
		#endif
			return -1;
		}
		fp=fopen(path,"w");
		fprintf(fp,"%d",j);
	}
	
	
	sprintf(path,"%s",left->path);
	a=strstr(path, "pwm0");
		
	if(!a)
	{
		a=strstr(path,"pwm1");
	}
	if(!a)
	{
	#if DEBUG
		printf("failed to find path\n");
	#endif	
		return 1;
	}
	sprintf(a, "unexport");
#if DEBUG
	printf("%s\n",path);
#endif
	fp=fopen(path,"w");
	if(!fp)
		return 1;
	fprintf(fp,"0");
	fclose(fp);
	fp=fopen(path,"w");
	if(!fp)
		return 1;
	fprintf(fp,"1");
	fclose(fp);
	return 0;
}
