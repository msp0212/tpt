#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 10
int g_noOfDigits;
char g_digits[SIZE]={'0','1','2','3','4','5','6','7','8','9'};

void generator(char *digits,int numDigits);
unsigned long  myPower(int base, int exp);

int main(int argc,char **argv)
{
	int i;
	if(argc != 2)
	{
		fprintf(stderr,"Usage : %s <number>\n",argv[0]);
		exit(1);
	}
	g_noOfDigits =atoi(argv[1]);
	generator(g_digits, g_noOfDigits);
	return 0;
}

void generator(char *digits ,int numDigits)
{
	unsigned long i;
	int j,to_print;
	unsigned long  max = myPower(SIZE,numDigits);
	char *number =NULL;
	char msisdn[16]={0,};

	number = (char*)calloc(numDigits + 1, sizeof(char));
	number[numDigits] = '\0';
	for(i=0; i < max; i++)
	{
		to_print = i;
		for(j=1; j <= numDigits; j++)
		{	
			number[numDigits-j] = digits[to_print % SIZE];
			to_print/=SIZE;
		}
		sprintf(msisdn,"268%s",number);
		printf("%s\n",msisdn);
	}
	free(number);
}

unsigned long myPower(int base, int exp)
{
	int retVal=1;
	int i;
	for(i=0;i<exp;i++)
	{	
		retVal*=base;
	}

	return retVal;
}

