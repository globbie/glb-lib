#pragma once

// see original Linux Kernel lists

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define container_of(ptr, type, member) \
    (type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member))

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

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

static inline void
list_head_init(struct list_head *head)
{
    head->next = head;
    head->prev = head;
}

static inline void
list_add(struct list_head *head, struct list_head *entry)
{
    __list_add(head, head->next, entry);
}

static inline void
list_add_tail(struct list_head *head, struct list_head *entry)
{
    __list_add(head->prev, head, entry);
}

static inline void
list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void
list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    list_head_init(entry);
}

static inline void
list_replace(struct list_head *old, struct list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

static inline void
list_replace_init(struct list_head *old,
                  struct list_head *new)
{
    list_replace(old, new);
    list_head_init(old);
}

static inline void
list_move(struct list_head *head, struct list_head *entry)
{
    list_del(entry);
    list_add(head, entry);
}

static inline void
list_move_tail(struct list_head *head, struct list_head *entry)
{
    list_del(entry);
    list_add_tail(head, entry);
}

static inline bool
list_is_last(const struct list_head *head, const struct list_head *entry)
{
    return entry->next == head;
}

static inline bool
list_is_empty(const struct list_head *head)
{
    return head->next == head;
}

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
    list_entry((ptr)->prev, type, member)

#define list_prev_entry(pos, type, member) \
    list_entry((pos)->member.prev, type, member)

#define list_next_entry(pos, type, member) \
    list_entry((pos)->member.next, type, member)

#define list_foreach(iter, head) \
    for (iter = (head)->next; iter != (head); iter = iter->next)

#define list_foreach_safe(iter, save, head)                                     \
    for (iter = (head)->next, save = iter->next; iter != (head);                \
            iter = save, save = iter->next)

#define list_foreach_entry(iter, type, head, member)                            \
    for (iter = list_first_entry(head, type, member);                           \
            &iter->member != (head); iter = list_next_entry(iter, type, member))

#define list_foreach_entry_safe(iter, save, type, head, member)                 \
    for (iter = list_first_entry(head, type, member),                           \
            save = list_next_entry(iter, type, member);                         \
            &iter->member != (head);                                            \
            iter = save, save = list_next_entry(save, type, member))

#ifdef __cplusplus
}
#endif

