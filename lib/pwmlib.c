#include"pwmlib.h"

#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define DEBUG 1

int countPwms(DIR *d, char* path)
{
	char* fname;
	FILE *fp;
	struct dirent entry;
	int npwm=0;
	char c_npwm[5];
	int pwmIndex=0;
	
	
	while((entry=readdir(d))!=NULL)
	{
		if(!strcmp(entry->d_name,"npwm")
		{
			fname=(char*)malloc((strlen(path)+strlen(entry->d_name)+1)*sizeof(char));
			sprintf(fname,"%s/%s",path,entry->d_name);

			#if DEBUG
			printf("count %s\n",fname);
			#endif
			
			if(!(fp=fopen(fname,"r"))
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
	fp=fopen(fname);
	
	for(i=0;i<npwms;i++)
		fprintf(fp, "%d", i);
	
	fclose(fp)
	while((entry=readdir(d))!=NULL)
	{
		if(!(strncmp(entry->d_name, "pwm", 3))
		{
			i=*index;
			pwms[i]->path=(char*)malloc((sizeof(path)+sizeof(entry->d_name)+1)*sizeof(char));
			sprintf(pwms[i]->path,"%s/%s",path,entry->d_name);
			pwms[i]->period=DEFAULT_PERIOD;
			pwms[i]->duty=DEFAULT_PERIOD/2;
			pwms[i]->enable=0;
			*index++;
		}
	}
	
}

int initPwms(pwmStatus **pwms)
{
	int pwmCount = 0;
	ssize_t pathLen;
	char lnkPath[2048];

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
					path=(char*)malloc((strlen(PWMDIR)+strlen(entry->d_name)+1)*sizeof(char);
					sprintf(path,"%s/%s",PWMDIR,entry->d_name);
					
					//Get real path to pwmchip
					pathLen=readlink(path, lnkPath, 2048);
					lnkPath[pathLen]=0;
					
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
		
		#if DEBUG
		printf("PWM count: %d\n",pwmCount);
		#endif
		
		pwms=(pwmStatus**)malloc(pwmCount*sizeof(pwmStatus*));
		for(i=0;i<pwmCount;i++)
		{
			pwms[i]=(pwmStatus*)malloc(sizeof(pwmStatus));
		}
		
		rewinddir(d);
		
		while((entry=readdir(d))!=NULL)
		{
		//Find each pwmchip directory
			if(entry->d_type==DT_LNK && strstr(entry->d_name, "pwmchip"))
			{
					path=(char*)malloc((strlen(PWMDIR)+strlen(entry->d_name)+1)*sizeof(char);
					sprintf(path,"%s/%s",PWMDIR,entry->d_name);
					
					//Get real path to pwmchip
					pathLen=readlink(path, lnkPath, 2048);
					lnkPath[pathLen]=0;
					
					#if DEBUG
					printf("exporting ... %s -> %s\n",path, lnkPath);
					#endif
					
					//export and store PWM file structure
					if(subd=opendir(lnkPath))
					{
						exportPwms(subd, lnkPath, pwms, &pwmIndex);
						closedir(subd);
					}
					free(path);
			}
		}
		closedir(d);
	}
	return pwmCount;
}
