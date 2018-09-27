#include<stdio.h>
#include<stdlib.h>

typedef struct _ll
{
	int num;
	struct _ll *next;
}LL;

void addNode(LL **head,int n)
{
	LL *newNode,*tmp;

	newNode = (LL*)calloc(1,sizeof(LL));
	newNode->num = n;
	newNode->next = NULL;

	if(*head == NULL)
	{	
		printf("Adding the node first time...\n");
		*head = newNode;
	}
	else
	{
		tmp = *head;
		while(tmp->next != NULL)
		{	
			tmp = tmp->next;
		}

		tmp->next = newNode;
	}
}

void disp(LL *head)
{
	LL *tmp;
	if(head == NULL)
	{
		printf("Empty List...\n");
	}
	else
	{
		tmp = head;
		while(tmp != NULL)
		{
			printf("%d ",tmp->num);
			tmp = tmp->next;
		}
		printf("\n");
	}

}

void cleanup(LL *head)
{
	LL *node,*tmp;
	if(head == NULL)
	{
		printf("Nothing to clean...empty LL\n");
	}
	else
	{
		tmp = head;
		while(tmp != NULL)
		{
			node = tmp;
			tmp = tmp->next;
			free(node);
			node = NULL;
		}
		head = NULL;
	}
}


int main()
{
	LL *head = NULL;
	int i;

	for(i=0;i<5;i++)
	{
		addNode(&head,i);
	}

	disp(head);
	cleanup(head);
	disp(head);
	cleanup(head);
	return 0;
}
