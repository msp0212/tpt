#include <stdio.h>
#include <stdlib.h>

struct ll_node {
	int data;
	struct ll_node *next;
};


struct ll_node *ll_insert_head(struct ll_node *head, int data)
{
	struct ll_node *node;

	node = malloc(sizeof(*node));
	node->data = data;
	node->next = head;
	head = node;
	return head;
}

void ll_print(struct ll_node *head)
{
	struct ll_node *node = head;

	while (node != NULL) {
		printf("%d ", node->data);
		node = node->next;
	}
	printf("\n");
}

struct ll_node *ll_reverse(struct ll_node *head)
{
	struct ll_node *curr = head;
	struct ll_node *prev = NULL;
	struct ll_node *next = curr->next;

	while (next != NULL) {
		curr->next = prev;
		prev = curr;
		curr = next;
		next = curr->next;
	}
	curr->next = prev;
	return curr;
}

int main(int argc, char *argv[])
{
	struct ll_node *head = NULL;
	int i;


	for (i = 1; i < argc; i++) {
		head = ll_insert_head(head, atoi(argv[i]));
	}
	ll_print(head);
	head = ll_reverse(head);
	ll_print(head);

	return 0;
}
