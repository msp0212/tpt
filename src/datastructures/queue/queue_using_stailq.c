#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/queue.h>

struct q_element {
    int data;
    STAILQ_ENTRY(q_element) q_entry;
};

STAILQ_HEAD(q_element_head, q_element);

struct data_q {
    struct q_element_head q_head;
};

void q_init(struct data_q *q) 
{
    STAILQ_INIT(&q->q_head);
}

void q_add(struct data_q *q, int data)
{
    struct q_element *q_elem;

    q_elem = malloc(sizeof(*q_elem));
    q_elem->data = data;
    STAILQ_INSERT_TAIL(&q->q_head, q_elem, q_entry);
}

int q_remove(struct data_q *q)
{
    struct q_element *q_elem;
    int data;

    assert(!STAILQ_EMPTY(&q->q_head));
    q_elem = STAILQ_FIRST(&q->q_head);
    STAILQ_REMOVE_HEAD(&q->q_head, q_entry);
    data = q_elem->data;
    free(q_elem);
    return data;
}

void q_print(struct data_q *q)
{
    struct q_element *q_iter;

    STAILQ_FOREACH(q_iter, &q->q_head, q_entry) {
        printf("%d ", q_iter->data);
    }
    printf("\n");
}

void q_free(struct data_q *q)
{
    struct q_element *q_tmp;

    while (!STAILQ_EMPTY(&q->q_head)) {
        q_tmp = STAILQ_FIRST(&q->q_head);
        STAILQ_REMOVE_HEAD(&q->q_head, q_entry);
        free(q_tmp);
    }
}

void test1()
{
    struct data_q q;
    int i;

    q_init(&q);

    for (i = 0; i < 5; i++) {
        q_add(&q, i);
    }
    printf("q = ");
    q_print(&q);
    for (i = 0; i < 5; i++) {
        q_remove(&q);
        printf("q = ");
        q_print(&q);
    }
}

void test2()
{
    struct data_q q;
    int i;

    q_init(&q);

    for (i = 0; i < 5; i++) {
        q_add(&q, i);
    }
    q_remove(&q);
    printf("q = ");
    q_print(&q);

    q_free(&q);
}
int main()
{
    test1();
    test2();
    return 0;
}

