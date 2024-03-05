#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
/* #include "time.h" */

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *h = malloc(sizeof(struct list_head));

    if (!h)
        return NULL;

    INIT_LIST_HEAD(h);

    return h;
}

/* Fisher-Yates shuffle queue, no effect if header is NULL */
void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* srand(time(0)); */

    int len = q_size(head);
    struct list_head *i_th = head->prev;

    for (int i = len; i > 0; i--) {
        struct list_head *r_th = head;
        int r = rand() % i + 1;
        if (r != i) {
            // find r_th
            for (int j = 0; j < r; j++)
                r_th = r_th->next;

            // swap r_th and i_th element
            element_t *e_rth = list_entry(r_th, element_t, list);
            element_t *e_ith = list_entry(i_th, element_t, list);
            char *temp = e_rth->value;
            e_rth->value = e_ith->value;
            e_ith->value = temp;
        }
        i_th = i_th->prev;
    }
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    while (!list_empty(head)) {
        element_t *e = list_first_entry(head, element_t, list);
        list_del(&e->list);
        free(e->value);
        free(e);
    }

    free(head);

    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    char *v = malloc(strlen(s) + 1);
    if (!v)
        return false;

    strncpy(v, s, strlen(s) + 1);

    element_t *e = malloc(sizeof(element_t));

    if (!e) {
        free(v);
        return false;
    }

    e->value = v;
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    char *v = malloc(strlen(s) + 1);
    if (!v)
        return false;

    strncpy(v, s, strlen(s) + 1);

    element_t *e = malloc(sizeof(element_t));

    if (!e) {
        free(v);
        return false;
    }

    e->value = v;
    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_first_entry(head, element_t, list);

    list_del(&e->list);

    if (sp && bufsize > 0) {
        strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_last_entry(head, element_t, list);

    list_del(&e->list);

    if (sp && bufsize > 0) {
        strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *forward = head->next;
    struct list_head *backward = head->prev;

    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }

    element_t *e = list_entry(forward, element_t, list);
    list_del(&e->list);
    free(e->value);
    free(e);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    if (list_empty(head) || list_is_singular(head))
        return true;

    element_t *e = list_first_entry(head, element_t, list);
    struct list_head *next = e->list.next;

    while (next != head) {
        bool dup = false;
        while (next != head) {
            element_t *next_e = list_entry(next, element_t, list);
            next = next->next;
            if (strcmp(e->value, next_e->value) == 0) {
                dup = true;
                free(next_e->value);
                list_del(&next_e->list);
                free(next_e);
            } else {
                break;
            }
        }

        next = e->list.next;  // using next to store the next e temporarily

        if (dup) {
            free(e->value);
            list_del(&e->list);
            free(e);
        }

        if (next != head) {
            e = list_entry(next, element_t, list);
            next = e->list.next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    // aa <-> a <-> b <-> bb
    struct list_head *a = head->next;
    struct list_head *b = a->next;

    struct list_head *aa = a->prev;
    struct list_head *bb = b->next;

    while (a != head && b != head) {
        aa->next = b;
        b->prev = aa;
        b->next = a;
        a->prev = b;
        a->next = bb;
        bb->prev = a;

        a = bb;
        b = a->next;
        aa = a->prev;
        bb = b->next;
    }

    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node, *safe, *tmp;
    list_for_each_safe (node, safe, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k < 2)
        return;

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    while (q_size(head) >= k) {
        struct list_head *p = head;
        for (int i = 0; i < k; i++) {
            p = p->next;
        }
        struct list_head to;
        INIT_LIST_HEAD(&to);
        list_cut_position(&to, head, p);

        q_reverse(&to);
        list_splice_tail(&to, &tmp);
    }
    list_splice(&tmp, head);
    return;
}

int valuecmp(const char *a, const char *b, bool descend)
{
    int r = strcmp(a, b);
    if (descend)
        r = -r;
    return r;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    // find the center node
    struct list_head *forward = head->next;
    struct list_head *backward = head->prev;

    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }

    struct list_head left, right;
    INIT_LIST_HEAD(&left);
    INIT_LIST_HEAD(&right);

    list_cut_position(&left, head, forward);
    list_splice_init(head, &right);

    q_sort(&left, descend);
    q_sort(&right, descend);

    // merge
    if (!list_empty(&left) && !list_empty(&right)) {
        // fast case 1
        element_t *l_f = list_first_entry(&left, element_t, list);
        element_t *r_l = list_last_entry(&right, element_t, list);
        if (valuecmp(r_l->value, l_f->value, descend) <= 0) {
            list_splice_tail(&left, &right);
            list_splice(&right, head);
            return;
        }

        // fast case 2
        element_t *l_l = list_last_entry(&left, element_t, list);
        element_t *r_f = list_first_entry(&right, element_t, list);
        if (valuecmp(l_l->value, r_f->value, descend) <= 0) {
            list_splice_tail(&right, &left);
            list_splice(&left, head);
            return;
        }
    }

    int l_size = q_size(&left), r_size = q_size(&right);
    while (l_size > 0 && r_size > 0) {
        element_t *l = list_first_entry(&left, element_t, list);
        element_t *r = list_first_entry(&right, element_t, list);
        if (valuecmp(l->value, r->value, descend) > 0) {
            list_move_tail(&r->list, head);
            r_size--;
        } else {
            list_move_tail(&l->list, head);
            l_size--;
        }
    }

    if (l_size > 0)
        list_splice_tail(&left, head);
    else
        list_splice_tail(&right, head);

    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    element_t *e = list_first_entry(head, element_t, list);

    while (e->list.next != head) {
        element_t *p = list_entry(e->list.next, element_t, list);
        if (strcmp(e->value, p->value) <= 0) {
            e = p;
        } else {
            list_del(&p->list);
            free(p->value);
            free(p);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    element_t *e = list_last_entry(head, element_t, list);

    while (e->list.prev != head) {
        element_t *p = list_entry(e->list.prev, element_t, list);
        if (strcmp(e->value, p->value) <= 0) {
            e = p;
        } else {
            list_del(&p->list);
            free(p->value);
            free(p);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    struct list_head null_queues;  // temporary storage of NULL queues
    INIT_LIST_HEAD(&null_queues);
    while (!list_is_singular(head)) {
        queue_contex_t *a = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *b = list_entry(a->chain.next, queue_contex_t, chain);

        struct list_head *left = a->q;
        struct list_head *right = b->q;
        struct list_head tmp;

        INIT_LIST_HEAD(&tmp);

        // merge two lists
        int l_size = a->size, r_size = b->size;
        a->size += b->size;
        bool done = false;

        // fast cases
        if (!list_empty(left) && !list_empty(right)) {
            // fast case 1
            element_t *l_f = list_first_entry(left, element_t, list);
            element_t *r_l = list_last_entry(right, element_t, list);

            // fast case 2
            element_t *l_l = list_last_entry(left, element_t, list);
            element_t *r_f = list_first_entry(right, element_t, list);

            if (valuecmp(r_l->value, l_f->value, descend) <= 0) {
                list_splice_tail_init(left, right);
                list_splice_init(right, &tmp);
                done = true;
            } else if (valuecmp(l_l->value, r_f->value, descend) <= 0) {
                list_splice_tail_init(right, left);
                list_splice_init(left, &tmp);
                done = true;
            }
        }

        if (!done) {
            while (l_size > 0 && r_size > 0) {
                element_t *l = list_first_entry(left, element_t, list);
                element_t *r = list_first_entry(right, element_t, list);
                if (valuecmp(l->value, r->value, descend) > 0) {
                    list_move_tail(&r->list, &tmp);
                    r_size--;
                } else {
                    list_move_tail(&l->list, &tmp);
                    l_size--;
                }
            }

            if (l_size > 0)
                list_splice_tail_init(left, &tmp);
            else
                list_splice_tail_init(right, &tmp);
        }

        list_splice(&tmp, a->q);

        /* b->q = NULL; */
        b->size = 0;

        list_move_tail(&b->chain, &null_queues);
        list_move_tail(&a->chain, head);  // move the merged queue to tail
    }

    list_splice_tail(&null_queues, head);
    queue_contex_t *a = list_first_entry(head, queue_contex_t, chain);

    return a->size;
}
