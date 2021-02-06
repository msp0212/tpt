#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/queue.h>

struct stack_node {
    int data;
    SLIST_ENTRY(stack_node) st_entry; 
};

SLIST_HEAD(stack_head, stack_node);

struct stack {
    struct stack_head st_head;
};

void stack_init(struct stack *s);
void stack_push(struct stack *s, int data);
int stack_pop(struct stack *s);
int stack_top(struct stack *s);
void stack_print(struct stack *s);
void stack_free(struct stack *s);

void stack_init(struct stack *s)
{
    SLIST_INIT(&s->st_head);    
}

void stack_push(struct stack *s, int data)
{
    struct stack_node *new_node;
    
    new_node = malloc(sizeof(*new_node));
    new_node->data = data;
    SLIST_INSERT_HEAD(&s->st_head, new_node, st_entry);
}

int stack_pop(struct stack *s)
{
    struct stack_node *tmp;
    int data;
    
    assert(!SLIST_EMPTY(&s->st_head) && "stack is empty");
    tmp = SLIST_FIRST(&s->st_head);
    SLIST_REMOVE_HEAD(&s->st_head, st_entry);
    data = tmp->data;
    free(tmp);
    return data;
}

int stack_top(struct stack *s)
{
    struct stack_node *tmp;
    
    assert(!SLIST_EMPTY(&s->st_head) && "stack is empty");
    tmp = SLIST_FIRST(&s->st_head);
    return tmp->data;
}

void stack_print(struct stack *s)
{
    struct stack_node *tmp;

    SLIST_FOREACH(tmp, &s->st_head, st_entry) {
        printf("%d ", tmp->data);
    }
    printf("\n");
}

void stack_free(struct stack *s)
{
    struct stack_node *tmp;
    
    while (!SLIST_EMPTY(&s->st_head)) {
        tmp = SLIST_FIRST(&s->st_head);
        SLIST_REMOVE_HEAD(&s->st_head, st_entry);
        free(tmp);
    }
}

int main(int argc, char *argv[])
{
    struct stack s;
    int data; 
    
    stack_init(&s);
    
    stack_push(&s, 1);
    stack_push(&s, 2);
    stack_push(&s, 3);
    stack_push(&s, 4);
    stack_push(&s, 5);
    
    stack_print(&s);
    
    data = stack_pop(&s);
    printf("popped value = %d\n", data);
    
    data = stack_top(&s);
    printf("top value = %d\n", data);
    
    data = stack_pop(&s);
    printf("popped value = %d\n", data);
    data = stack_pop(&s);
    printf("popped value = %d\n", data);
    
    stack_free(&s);
    return 0;
}
