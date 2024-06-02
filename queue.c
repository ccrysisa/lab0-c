#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

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
void q_free(struct list_head *l)
{
    struct list_head *head = l, *node, *safe;
    if (!head)
        return;
    list_for_each_safe (node, safe, head) {
        list_del(node);
        element_t *elem = list_entry(node, element_t, list);
        q_release_element(elem);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *elem = malloc(sizeof(element_t));
    if (!elem)
        return false;
    elem->value = strdup(s);
    if (!elem->value) {
        free(elem);
        return false;
    }
    list_add(&elem->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *elem = malloc(sizeof(element_t));
    if (!elem)
        return false;
    elem->value = strdup(s);
    if (!elem->value) {
        free(elem);
        return false;
    }
    list_add_tail(&elem->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *elem = list_first_entry(head, element_t, list);
    list_del_init(&elem->list);
    if (sp) {
        memcpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *elem = list_last_entry(head, element_t, list);
    list_del_init(&elem->list);
    if (sp) {
        memcpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *node;
    list_for_each (node, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *p = head->next;
    struct list_head *q = head->prev;
    while (!(p == q || p->next == q)) {
        p = p->next;
        q = q->prev;
    }
    list_del_init(q);
    element_t *elem = list_entry(q, element_t, list);
    q_release_element(elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        struct list_head *temp = safe;
        element_t *e_node = list_entry(node, element_t, list);
        while (!(safe == head)) {
            element_t *e_safe = list_entry(safe, element_t, list);
            if (strcmp(e_node->value, e_safe->value))
                break;
            safe = safe->next;
            list_del(&e_safe->list);
            q_release_element(e_safe);
        }
        if (temp != safe) {
            list_del(node);
            q_release_element(e_node);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *node, *safe, *prev, *next;
    list_for_each_safe (node, safe, head) {
        if (safe == head)
            break;
        prev = node->prev;
        next = safe->next;
        node->prev = safe;
        safe->next = node;

        node->next = next;
        safe->prev = prev;
        prev->next = safe;
        next->prev = node;

        safe = next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *safe, *prev;
    list_for_each_safe (node, safe, head) {
        prev = node->prev;
        node->prev = safe;
        node->next = prev;
    }
    prev = head->prev;
    head->prev = head->next;
    head->next = prev;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;
    struct list_head *node, *safe, *prev, *next;
    list_for_each_safe (node, safe, head) {
        // get prev and next nodes around K nodes
        prev = node->prev;
        next = node;
        int cnt = 0;
        while (cnt < k && next != head) {
            cnt++;
            next = next->next;
        }
        if (cnt < k)
            break;
        safe = next->prev;

        // reverse K nodes
        struct list_head *p = node, *q;
        while (p != next) {
            q = p->next;
            p->next = p->prev;
            p->prev = q;

            p = q;
        }

        // setup node, safe, prev, next
        node->next = next;
        next->prev = node;
        safe->prev = prev;
        prev->next = safe;

        safe = next;
    }
}

/* Bubble sort */
/*
static void q_bubble_sort(struct list_head *head, bool descend)
{
    if (!head)
        return;

    bool swapped = true;
    struct list_head *node, *safe, *prev, *next;

    while (swapped) {
        swapped = false;
        list_for_each_safe (node, safe, head) {
            if (safe == head)
                break;

            element_t *e_node = list_entry(node, element_t, list);
            element_t *e_safe = list_entry(safe, element_t, list);

            int cmp = strcmp(e_node->value, e_safe->value);
            if ((descend && cmp < 0) || (!descend && cmp > 0)) {
                swapped = true;

                // swap
                prev = node->prev;
                next = safe->next;
                node->prev = safe;
                safe->next = node;

                node->next = next;
                safe->prev = prev;
                prev->next = safe;
                next->prev = node;

                // set next node
                safe = node;
            }
        }
    }
}
*/

/* Insertion sort */
/*
static void q_insertion_sort(struct list_head *head, bool descend)
{
    if (!head)
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        struct list_head *prev = node->prev, *next;
        // one node is already sorted
        if (prev == head)
            continue;

        // remove
        list_del(node);
        element_t *e_node = list_entry(node, element_t, list);
        element_t *e_prev = list_entry(prev, element_t, list);

        // find position
        int cmp = strcmp(e_prev->value, e_node->value);
        while ((descend && cmp < 0) || (!descend && cmp > 0)) {
            prev = prev->prev;
            if (prev == head)
                break;
            e_prev = list_entry(prev, element_t, list);
            cmp = strcmp(e_prev->value, e_node->value);
        }

        // insertion
        next = prev->next;
        prev->next = node;
        node->prev = prev;
        node->next = next;
        next->prev = node;
    }
}
*/

/* Selection sort */
/*
static void q_selection_sort(struct list_head *head, bool descend)
{
    if (!head)
        return;

    struct list_head *node, *safe, *prev = head;
    list_for_each_safe (node, safe, head) {
        struct list_head *temp = node->next, *sele = node;

        // selection
        while (temp != head) {
            element_t *e_sele = list_entry(sele, element_t, list);
            element_t *e_temp = list_entry(temp, element_t, list);

            int cmp = strcmp(e_sele->value, e_temp->value);
            if ((descend && cmp < 0) || (!descend && cmp > 0)) {
                sele = temp;
            }
            temp = temp->next;
        }

        // insertion
        list_del(sele);
        prev->next->prev = sele;
        sele->next = prev->next;
        prev->next = sele;
        sele->prev = prev;

        // set next node
        prev = sele;
        safe = sele->next;
    }
}
*/

/* Merge two linked list */
static void merge(struct list_head **l1,
                  struct list_head **const l2,
                  bool descend)
{
    struct list_head **temp = l1;
    struct list_head *node1 = *l1;
    struct list_head *node2 = *l2;

    while (node1 && node2) {
        element_t *elem1 = list_entry(node1, element_t, list);
        element_t *elem2 = list_entry(node2, element_t, list);

        int cmp = strcmp(elem1->value, elem2->value);
        if ((descend && cmp < 0) || (!descend && cmp > 0)) {
            *temp = node2;
            node2 = node2->next;
        } else {
            *temp = node1;
            node1 = node1->next;
        }
        temp = &(*temp)->next;
    }

    *temp = node1 ? node1 : node2;
}

/* Merge sort */
static void q_merge_sort(struct list_head **head, bool descend)
{
    if (!(*head) || !(*head)->next)
        return;

    // get the middle node by fast and slow pointers
    struct list_head *p = *head;
    struct list_head *q = (*head)->next;
    while (q && q->next) {
        p = p->next;
        q = q->next->next;
    }

    // set an additional list head
    struct list_head *l2 = p->next;
    p->next = NULL;

    q_merge_sort(head, descend);
    q_merge_sort(&l2, descend);
    merge(head, &l2, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    // q_bubble_sort(head, descend);
    // q_insertion_sort(head, descend);
    // q_selection_sort(head, descend);

    if (!head)
        return;
    head->prev->next = NULL;
    q_merge_sort(&head->next, descend);
    struct list_head *node, *prev = head;
    for (node = head->next; node; node = node->next) {
        node->prev = prev;
        prev = node;
    }
    prev->next = head;
    head->prev = prev;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;
    q_reverse(head);
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        if (safe == head)
            break;
        element_t *e_node = list_entry(node, element_t, list);
        element_t *e_safe = list_entry(safe, element_t, list);
        while (strcmp(e_node->value, e_safe->value) < 0) {
            safe = safe->next;
            list_del(safe->prev);
            q_release_element(e_safe);
            if (safe == head)
                break;
            e_safe = list_entry(safe, element_t, list);
        }
    }
    q_reverse(head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere
 * to the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;
    q_reverse(head);
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        if (safe == head)
            break;
        element_t *e_node = list_entry(node, element_t, list);
        element_t *e_safe = list_entry(safe, element_t, list);
        while (strcmp(e_node->value, e_safe->value) > 0) {
            safe = safe->next;
            list_del(safe->prev);
            q_release_element(e_safe);
            if (safe == head)
                break;
            e_safe = list_entry(safe, element_t, list);
        }
    }
    q_reverse(head);
    return q_size(head);
}

/* Merge two lists */
static void q_merge2(struct list_head *l1, struct list_head *l2, bool descend)
{
    queue_contex_t *q1 = list_entry(l1, queue_contex_t, chain);
    queue_contex_t *q2 = list_entry(l2, queue_contex_t, chain);
    struct list_head *h1 = q1->q->next;
    struct list_head *h2 = q2->q->next;
    struct list_head **head = &q1->q;

    while (h1 != q1->q && h2 != q2->q) {
        element_t *e1 = list_entry(h1, element_t, list);
        element_t *e2 = list_entry(h2, element_t, list);

        int cmp = strcmp(e1->value, e2->value);
        if ((descend && cmp < 0) || (!descend && cmp > 0)) {
            (*head)->next = h2;
            h2->prev = (*head);
            h2 = h2->next;
        } else {
            (*head)->next = h1;
            h1->prev = (*head);
            h1 = h1->next;
        }
        head = &(*head)->next;
    }

    if (h1 != q1->q) {
        (*head)->next = h1;
        h1->prev = (*head);
        head = &q1->q->prev;
    }
    if (h2 != q2->q) {
        (*head)->next = h2;
        h2->prev = (*head);
        head = &q2->q->prev;
    }

    (*head)->next = q1->q;
    q1->q->prev = (*head);
    INIT_LIST_HEAD(q2->q);
    q1->size += q2->size;
}

/* Merge lists in region [lh, rh) */
static void q_mergeK(struct list_head *lh, struct list_head *rh, bool descend)
{
    if (lh == rh || lh->next == rh)
        return;
    struct list_head *p = lh;
    struct list_head *q = rh->prev;
    while (!(p == q || p->next == q)) {
        p = p->next;
        q = q->prev;
    }
    q_mergeK(lh, q, descend);
    q_mergeK(q, rh, descend);
    q_merge2(lh, q, descend);
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    q_mergeK(head->next, head, descend);
    return list_entry(head->next, queue_contex_t, chain)->size;
}
