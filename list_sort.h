#pragma once

#include "queue.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

typedef int (*list_cmp_func_t)(void *, struct list_head *, struct list_head *);

void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);
