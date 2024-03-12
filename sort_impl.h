#pragma once

#include "list_sort.h"

void timsort(void *priv, struct list_head *head, list_cmp_func_t cmp);
