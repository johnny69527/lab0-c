#include "quick_sort.h"
#include <stdio.h>
#include <string.h>

int qs_valuecmp(const char *a, const char *b, bool descend)
{
    int r = strcmp(a, b);
    if (descend)
        r = -r;
    return r;
}

typedef struct {
    struct list_head value;
    struct list_head list;
} stack_t;

stack_t *stack_push(struct list_head *head)
{
    stack_t *stk = malloc(sizeof(stack_t));
    if (!stk)
        return NULL;

    INIT_LIST_HEAD(&(stk->value));

    list_add_tail(&(stk->list), head);
    return stk;
}

stack_t *stack_pop(struct list_head *head)
{
    if (!head || list_empty(head))
        return NULL;

    /* cppcheck-suppress nullPointer */
    stack_t *stk = list_last_entry(head, stack_t, list);

    list_del(&stk->list);

    return stk;
}

void quick_sort(struct list_head *head, bool descend)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head stacks;
    INIT_LIST_HEAD(&stacks);

    stack_t *stk = stack_push(&stacks);
    list_splice_init(head, &(stk->value));

    element_t *pivot_e;

    while (!list_empty(&stacks)) {
        stk = stack_pop(&stacks);
        if (!list_empty(&(stk->value)) && !list_is_singular(&(stk->value))) {
            /* cppcheck-suppress nullPointer */
            pivot_e = list_first_entry(&(stk->value), element_t, list);
            list_del(&pivot_e->list);

            stack_t *left = stack_push(&stacks);
            stack_t *p = stack_push(&stacks);
            stack_t *right = stack_push(&stacks);

            list_add(&pivot_e->list, &(p->value));

            struct list_head *safe, *node;
            list_for_each_safe (node, safe, &(stk->value)) {
                list_del(node);
                /* cppcheck-suppress nullPointer */
                element_t *node_e = list_entry(node, element_t, list);
                if (qs_valuecmp(pivot_e->value, node_e->value, descend) < 0)
                    list_add_tail(node, &(left->value));
                else
                    list_add_tail(node, &(right->value));
            }
        } else {
            list_splice_tail_init(&(stk->value), head);
        }

        free(stk);
    }
}
