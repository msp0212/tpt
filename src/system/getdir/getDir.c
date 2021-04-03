#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DIR_SEP "/"
int g_NumOfFirstLevDir = 16;
int g_NumOfSecondLevDir =256;

int getDirectoryForUser(char *msisdn,char **dirPath)
{
	int sumF = 0;
	int sumS = 0;
	int flev = 0;
	int slev = 0;
	char first2Dig[4]={0,};
	char last4Dig[8]={0,};
	int len;
	if(msisdn == NULL || dirPath == NULL)
	{
			fprintf(stderr,"%s:Error in passing arguments !!!\n",__FUNCTION__);
			return -1;
	}

	*dirPath = (char*)calloc(32,sizeof(char));
	if(*dirPath == NULL)
	{
			fprintf(stderr,"%s:Calloc FAILED!!!\n",__FUNCTION__);
			return -1;
	}
	
	len =strlen(msisdn);
	strncpy(first2Dig,msisdn+(len-2),2);
	strncpy(last4Dig,msisdn+(len-2-6),6);
	printf("first2Dig = %s last6Dig = %s\n",first2Dig,last4Dig);
	sumF = atoi(first2Dig);
	sumS = atoi(last4Dig);
	flev = sumF % g_NumOfFirstLevDir;
	slev = sumS % g_NumOfSecondLevDir;


	flev = sumF % g_NumOfFirstLevDir;
	slev = sumS % g_NumOfSecondLevDir;
	sprintf(*dirPath,"%02x"DIR_SEP"%02x"DIR_SEP"%s",flev,slev,msisdn);
	return 0;
}




int main(int argc,char *argv[])
{
	char *dirPath = NULL;
	int iRet = 0;
	if(argc != 4)
	{
		fprintf(stderr,"Usage : %s <msisdn> <noOfFirstLevelDirs> <noOfSecondLevelDirs>\n",argv[0]);
		exit(1);
	}
	g_NumOfFirstLevDir = atoi(argv[2]);
	g_NumOfSecondLevDir = atoi(argv[3]);
	if(g_NumOfFirstLevDir ==0 || g_NumOfSecondLevDir == 0)
	{
		fprintf(stderr,"%s:Invalid Value for noOfFirstLevelDirs or noOfSecondLevelDirs...Can't be 0\n",__FUNCTION__);
		exit(1);
	}
	iRet = getDirectoryForUser(argv[1],&dirPath);
	if(iRet != 0)
	{
		fprintf(stderr,"%s:getDirectoryForUser FAILED !!!\n",__FUNCTION__);
		exit(1);
	}
	fprintf(stderr,"Directory for %s   =   %s\n",argv[1],dirPath);
	free(dirPath);
	return 0;
}
