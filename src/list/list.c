#include "list.h"

static inline void
__list_add(struct list_head *prev, struct list_head *next,
           struct list_head *entry)
{
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

static inline void
__list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

extern inline void
list_head_init(struct list_head *head)
{
    head->next = head;
    head->prev = head;
}

void
list_add(struct list_head *head, struct list_head *entry)
{
    __list_add(head, head->next, entry);
}

void
list_add_tail(struct list_head *head, struct list_head *entry)
{
    __list_add(head->prev, head, entry);
}

void
list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

void
list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    list_head_init(entry);
}

extern inline void
list_replace(struct list_head *old, struct list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

extern inline void
list_replace_init(struct list_head *old,
                  struct list_head *new)
{
    list_replace(old, new);
    list_head_init(old);
}

void
list_move(struct list_head *head, struct list_head *entry)
{
    list_del(entry);
    list_add(head, entry);
}

void
list_move_tail(struct list_head *head, struct list_head *entry)
{
    list_del(entry);
    list_add_tail(head, entry);
}

bool
list_is_last(const struct list_head *head, const struct list_head *entry)
{
    return entry->next == head;
}

bool
list_is_empty(const struct list_head *head)
{
    return head->next == head;
}

