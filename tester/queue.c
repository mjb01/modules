/* list.c ---
 *
 *
 * Author: Gebriel Belaineh, Andy Trinh & Michael Bongo
 * Created: Sun Jan 22 22:58:50 2023 (-0500)
 * Version: 1.0
 *
 * Description: implements the list.h module
 *
 */

#include "queue.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct q_node
{
    void *data;
    struct q_node *next;
} q_node_t;

typedef struct q
{
    q_node_t *front;
    q_node_t *back;
} q_t;

/* create an empty queue */
queue_t *qopen(void)
{
    q_t *q = (q_t *)malloc(sizeof(q_t));
    if (q == NULL)
    {
        return NULL;
    }
    q->front = NULL;
    q->back = NULL;
    return (queue_t *)q;
}

/* deallocate a queue, frees everything in it */
void qclose(queue_t *qp)
{
    q_t *q = (q_t *)qp;
    if (qp != NULL)
    {
        q_node_t *current = q->front;
        q_node_t *next;
        while (current != NULL)
        {
            next = current->next;
            free(current->data);
            free(current);
            current = next;
        }
    }
    free(q);
}

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise
 */
int32_t qput(queue_t *qp, void *elementp)
{
    if (qp == NULL)
    {
        return 1;
    }
    q_t *q = (q_t *)qp;
    q_node_t *node = (q_node_t *)malloc(sizeof(q_node_t));

    if (node == NULL)
    {
        return 1;
    }
    node->data = elementp;
    node->next = NULL;

    if (q->front == NULL)
    {
        q->front = node;
    }
    else
    {
        q->back->next = node;
    }
    q->back = node;
    return 0;
}

/* get the first first element from queue, removing it from the queue */
void *qget(queue_t *qp)
{
    if (qp == NULL)
    {
        return NULL;
    }
    q_t *q = (q_t *)qp;
    q_node_t *node = q->front;
    if (node == NULL)
    {
        return NULL;
    }
    void *data = node->data;
    q->front = node->next;
    node->next = NULL;
    if (q->front == NULL)
    {
        q->back = NULL;
    }
    free(node);
    return data;
}

/* apply a function to every element of the queue */
void qapply(queue_t *qp, void (*fn)(void *elementp))
{
    if (qp == NULL)
    {
        return;
    }
    q_t *q = (q_t *)qp;
    q_node_t *current = q->front;
    while (current != NULL)
    {
        (*fn)(current->data);
        current = current->next;
    }
}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void *qsearch(queue_t *qp, bool (*searchfn)(void *elementp, const void *keyp), const void *skeyp)
{
    if (qp == NULL)
    {
        return NULL;
    }
    q_t *q = (q_t *)qp;
    q_node_t *current = q->front;

    while (current != NULL)
    {
        bool search_result = (*searchfn)(current->data, skeyp);
        if (search_result)
        {
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}

/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void *qremove(queue_t *qp,
              bool (*searchfn)(void *elementp, const void *keyp),
              const void *skeyp)
{
    if (qp == NULL)
    {
        return NULL;
    }
    q_t *q = (q_t *)qp;

    q_node_t *current = q->front;
    q_node_t *prev = NULL;

    while (current != NULL)
    {
        if (searchfn(current->data, skeyp))
        {
            void *data = current->data;
            if (prev == NULL) // first element
            {
                q->front = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            if (current == q->back)
                q->back = prev;
            free(current);
            return data;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}

/* concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion
 */
void qconcat(queue_t *q1p, queue_t *q2p)
{
    if (q1p != NULL && q2p != NULL)
    {
        q_t *q1 = q1p;
        q_t *q2 = q2p;

        if (q1->front == NULL)
        {
            if (q2->front != NULL)
            {
                q1->front = q2->front;
                q1->back = q2->back;
            }
        }
        else
        {
            if (q2->front != NULL)
            {
                q1->back->next = q2->front;
                q1->back = q2->back;
            }
        }
        q2->back = NULL;
        q2->front = NULL;
        qclose(q2p);
    }
}
