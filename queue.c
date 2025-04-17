#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"


/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


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

struct list_head *mergeTwoLists(struct list_head *left, struct list_head *right)
{
    struct list_head head;
    struct list_head *h = &head;

    while (left && right) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) <= 0) {
            h->next = left;
            left = left->next;
        } else {
            h->next = right;
            right = right->next;
        }
        h = h->next;
    }

    h->next = left ? left : right;
    return head.next;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *slow = head;
    for (const struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow->next;
    // remove mid and its prev link
    slow->next = NULL;
    struct list_head *left = merge_sort(head), *right = merge_sort(mid);
    return mergeTwoLists(left, right);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    if (list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = merge_sort(head->next);

    // reconnect list
    struct list_head *c = head, *n = head->next;
    while (n) {
        n->prev = c;
        c = n;
        n = n->next;
    }
    c->next = head;
    head->prev = c;


    if (descend)
        q_reverse(head);
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    element_t *safe, *node;
    char const *tmpch = (list_entry(head->prev, element_t, list))->value;
    for (node = list_entry((head)->prev, typeof(*node), list),
        safe = list_entry(node->list.prev, typeof(*node), list);
         &node->list != (head);
         node = safe, safe = list_entry(safe->list.prev, typeof(*node), list)) {
        if (strcmp(node->value, tmpch) > 0) {
            list_del(&node->list);
            continue;
        }
        tmpch = node->value;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    element_t *safe, *node;
    char const *tmpch = (list_entry(head->prev, element_t, list))->value;
    for (node = list_entry((head)->prev, typeof(*node), list),
        safe = list_entry(node->list.prev, typeof(*node), list);
         &node->list != (head);
         node = safe, safe = list_entry(safe->list.prev, typeof(*node), list)) {
        if (strcmp(node->value, tmpch) < 0) {
            list_del(&node->list);
            continue;
        }
        tmpch = node->value;
    }
    return q_size(head);
}

void merge_2_queue(struct list_head *list1,
                   struct list_head *list2,
                   bool descend)
{
    element_t *safe, *node1, *node2;
    node1 = list_entry((list1)->next, typeof(*node1), list);
    node2 = list_entry((list2)->next, typeof(*node2), list);
    safe = list_entry(node2->list.next, typeof(*node2), list);
    char const *tmpch1 = (list_entry(list1->next, element_t, list))->value;
    char const *tmpch2 = (list_entry(list2->next, element_t, list))->value;
    char remove_value[10];

    if (descend) {
        while (&node1->list != (list1) && &node2->list != (list2)) {
            if (strcmp(tmpch1, tmpch2) > 0) {
                node1 = list_entry((node1)->list.next, typeof(*node1), list);
                tmpch1 = node1->value;
                continue;
            }
            safe = list_entry((node2)->list.next, typeof(*node1), list);
            node2 = q_remove_head(list2, remove_value, 10);
            node1->list.prev->next = &node2->list;
            node2->list.prev = node1->list.prev;
            node2->list.next = &node1->list;
            node1->list.prev = &node2->list;
            node2 = safe;
            tmpch2 = node2->value;
        }
        if (&node1->list == list1) {
            list_splice_tail_init(list2, list1);
        }
        return;
    }

    while (&node1->list != (list1) && &node2->list != (list2)) {
        if (strcmp(tmpch1, tmpch2) < 0) {
            node1 = list_entry((node1)->list.next, typeof(*node1), list);
            tmpch1 = node1->value;
            continue;
        }
        safe = list_entry((node2)->list.next, typeof(*node1), list);
        node2 = q_remove_head(list2, remove_value, 10);
        node1->list.prev->next = &node2->list;
        node2->list.prev = node1->list.prev;
        node2->list.next = &node1->list;
        node1->list.prev = &node2->list;
        node2 = safe;
        tmpch2 = node2->value;
    }
    if (&node1->list == list1) {
        list_splice_tail_init(list2, list1);
    }
    return;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *list1 = list_entry(head->next, queue_contex_t, chain);
    struct list_head *chain_queue = head->next->next;
    if (chain_queue == head)
        return list1->size;
    int count = list1->size;
    for (; chain_queue != head; chain_queue = chain_queue->next) {
        queue_contex_t *merge_list =
            list_entry(chain_queue, queue_contex_t, chain);
        if (!merge_list->q || list_empty(merge_list->q)) {
            continue;
        }
        count = count + merge_list->size;
        merge_2_queue(list1->q, merge_list->q, descend);
    }
    return count;
}
