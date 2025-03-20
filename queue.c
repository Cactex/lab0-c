#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head)) {
        free(head);
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        element_t *target_ele = list_entry(node, element_t, list);
        q_release_element(target_ele);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    new_element->value = malloc(strlen(s) + 1);
    if (!new_element->value) {
        q_release_element(new_element);
        return false;
    }
    strlcpy(new_element->value, s, strlen(s) + 1);
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    new_element->value = malloc(strlen(s) + 1);
    if (!new_element->value) {
        q_release_element(new_element);
        return false;
    }
    strlcpy(new_element->value, s, strlen(s) + 1);
    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *target_node = head->next;
    element_t *target_ele = list_entry(target_node, element_t, list);
    strlcpy(sp, target_ele->value, bufsize);
    list_del(target_node);
    return target_ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *target_node = head->prev;
    element_t *target_ele = list_entry(target_node, element_t, list);
    strlcpy(sp, target_ele->value, bufsize);
    list_del(target_node);
    return target_ele;
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
    struct list_head *nprev = head->prev;
    struct list_head *nnext = head->next;
    while ((nprev != nnext) && (nnext->next != nprev)) {
        nprev = nprev->prev;
        nnext = nnext->next;
    }
    list_del(nprev);
    q_release_element(list_entry(nprev, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *node, *safe;
    bool isdup = 0;
    list_for_each_safe (node, safe, head) {
        element_t *curr = list_entry(node, element_t, list);
        if (safe != head) {
            element_t const *next = list_entry(safe, element_t, list);
            if (strcmp(curr->value, next->value) == 0) {
                list_del(&curr->list);
                q_release_element(curr);
                isdup = true;
                continue;
            }
        }
        if (isdup) {
            list_del(&curr->list);
            q_release_element(curr);
            isdup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *node1, *node2;
    for (node1 = head->next, node2 = head->next->next;
         node1 != head && node2 != head;
         node1 = node1->next, node2 = node1->next) {
        node1->prev->next = node2;
        node2->next->prev = node1;
        node1->next = node2->next;
        node2->prev = node1->prev;
        node1->prev = node2;
        node2->next = node1;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    for (struct list_head *last = head->prev, *front = head->next,
                          *current = head;
         last != front; last = head->prev, current = current->next) {
        head->prev = last->prev;
        last->prev->next = head;

        current->next->prev = last;
        last->next = current->next;
        last->prev = current;
        current->next = last;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k == 1 || list_is_singular(head))
        return;
    int length = q_size(head);
    if (length == k)
        q_reverse(head);
    int counter = 0;
    LIST_HEAD(tmp);
    LIST_HEAD(reverse);
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        if (++counter == k) {
            list_cut_position(&tmp, head, node);
            q_reverse(&tmp);
            list_splice_tail_init(&tmp, &reverse);
            counter = 0;
        }
    }
    list_splice_init(&reverse, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
