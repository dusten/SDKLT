/*! \file shr_dq.h
 *
 * Defines a generic Doubly (Circular) linked list data structure.
 *
 * The following provides an example of how to use the SHR_DQ MACROs.
 *
 * Note that the example list is created to be sorted intentionally,
 * in order to better demonstrate how these INSERT/DELETE MACROs work.
 *
 * The usages of SHR_SL are very similar to SHR_DQ, hence omitted.
 *
 * **\code{.c}
 * **typedef struct dq_example_node_s {
 * **    int xxx;
 * **    shr_dq_t listnode;
 * **} dq_example_data_t;
 * **
 * **typedef struct dq_search_node_s {
 * **    int xxx;
 * **    shr_dq_t *listnode;
 * **} dq_search_node_t;
 * **
 * **static int
 * **dq_example_cmp(shr_dq_t *p_elem, dq_search_node_t *target)
 * **{
 * **    int match = 0;;
 * **    dq_example_data_t *data;
 * **    data = SHR_DQ_ELEMENT_GET(dq_example_data_t *, p_elem, listnode);
 * **    if (data->xxx == target->xxx) {
 * **        target->listnode = p_elem;
 * **        match = 1;
 * **    }
 * **    return match;
 * **}
 * **
 * **void dq_example() {
 * **    shr_dq_t dq_example_list;
 * **    int i;
 * **    dq_example_data_t user_datas[10];
 * **    shr_dq_t dq_example_list2;
 * **    dq_search_node_t search_data;
 * **    shr_dq_t *elem;
 * **    int unit = 0;
 * **
 * **    for (i = 0; i < COUNTOF(user_datas); i++) {
 * **        user_datas[i].xxx = i;
 * **    }
 * **
 * **    SHR_DQ_INIT(&dq_example_list);
 * **
 * **    SHR_DQ_INSERT_HEAD(&dq_example_list, &user_datas[4].listnode);
 * **    SHR_DQ_INSERT_HEAD(&dq_example_list, &user_datas[1].listnode);
 * **    SHR_DQ_INSERT_TAIL(&dq_example_list, &user_datas[5].listnode);
 * **    SHR_DQ_INSERT_PREV(&user_datas[4].listnode, &user_datas[3].listnode);
 * **    SHR_DQ_INSERT_NEXT(&user_datas[1].listnode, &user_datas[2].listnode);
 * **    SHR_DQ_REMOVE_HEAD(&dq_example_list, elem);
 * **    SHR_DQ_REMOVE_TAIL(&dq_example_list, elem);
 * **    if (SHR_DQ_EMPTY(&dq_example_list)) {
 * **        SHR_DQ_INIT(&dq_example_list2);
 * **    } else {
 * **        SHR_DQ_SWAP_HEAD(&dq_example_list2, &dq_example_list);
 * **    }
 * **    dq_example_print_list(unit, &dq_example_list2, 3);
 * **
 * **    search_data.xxx = 0x4;
 * **    search_data.listnode = NULL;
 * **    SHR_DQ_FIND(&dq_example_list2, dq_example_cmp, &search_data);
 * **
 * **    SHR_DQ_INSERT_PREV(&user_datas[2].listnode, &user_datas[0].listnode);
 * **    SHR_DQ_INSERT_NEXT(&user_datas[4].listnode, &user_datas[8].listnode);
 * **
 * **    SHR_DQ_TRAVERSE(&dq_example_list2, elem) {
 * **        dq_example_data_t *data;
 * **        data = SHR_DQ_ELEMENT_GET(dq_example_data_t*, elem, listnode);
 * **        SHR_DQ_REMOVE(&data->listnode);
 * **    } SHR_DQ_TRAVERSE_END(&dq_example_list2, elem);
 * **
 * **}
 * **\endcode
 */
/*
 * Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
 * Broadcom Limited and/or its subsidiaries.
 * 
 * Broadcom Switch Software License
 * 
 * This license governs the use of the accompanying Broadcom software. Your 
 * use of the software indicates your acceptance of the terms and conditions 
 * of this license. If you do not agree to the terms and conditions of this 
 * license, do not use the software.
 * 1. Definitions
 *    "Licensor" means any person or entity that distributes its Work.
 *    "Software" means the original work of authorship made available under 
 *    this license.
 *    "Work" means the Software and any additions to or derivative works of 
 *    the Software that are made available under this license.
 *    The terms "reproduce," "reproduction," "derivative works," and 
 *    "distribution" have the meaning as provided under U.S. copyright law.
 *    Works, including the Software, are "made available" under this license 
 *    by including in or with the Work either (a) a copyright notice 
 *    referencing the applicability of this license to the Work, or (b) a copy 
 *    of this license.
 * 2. Grant of Copyright License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    copyright license to reproduce, prepare derivative works of, publicly 
 *    display, publicly perform, sublicense and distribute its Work and any 
 *    resulting derivative works in any form.
 * 3. Grant of Patent License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    patent license to make, have made, use, offer to sell, sell, import, and 
 *    otherwise transfer its Work, in whole or in part. This patent license 
 *    applies only to the patent claims licensable by Licensor that would be 
 *    infringed by Licensor's Work (or portion thereof) individually and 
 *    excluding any combinations with any other materials or technology.
 *    If you institute patent litigation against any Licensor (including a 
 *    cross-claim or counterclaim in a lawsuit) to enforce any patents that 
 *    you allege are infringed by any Work, then your patent license from such 
 *    Licensor to the Work shall terminate as of the date such litigation is 
 *    filed.
 * 4. Redistribution
 *    You may reproduce or distribute the Work only if (a) you do so under 
 *    this License, (b) you include a complete copy of this License with your 
 *    distribution, and (c) you retain without modification any copyright, 
 *    patent, trademark, or attribution notices that are present in the Work.
 * 5. Derivative Works
 *    You may specify that additional or different terms apply to the use, 
 *    reproduction, and distribution of your derivative works of the Work 
 *    ("Your Terms") only if (a) Your Terms provide that the limitations of 
 *    Section 7 apply to your derivative works, and (b) you identify the 
 *    specific derivative works that are subject to Your Terms. 
 *    Notwithstanding Your Terms, this license (including the redistribution 
 *    requirements in Section 4) will continue to apply to the Work itself.
 * 6. Trademarks
 *    This license does not grant any rights to use any Licensor's or its 
 *    affiliates' names, logos, or trademarks, except as necessary to 
 *    reproduce the notices described in this license.
 * 7. Limitations
 *    Platform. The Work and any derivative works thereof may only be used, or 
 *    intended for use, with a Broadcom switch integrated circuit.
 *    No Reverse Engineering. You will not use the Work to disassemble, 
 *    reverse engineer, decompile, or attempt to ascertain the underlying 
 *    technology of a Broadcom switch integrated circuit.
 * 8. Termination
 *    If you violate any term of this license, then your rights under this 
 *    license (including the license grants of Sections 2 and 3) will 
 *    terminate immediately.
 * 9. Disclaimer of Warranty
 *    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 *    KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
 *    NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
 *    THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
 *    IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
 * 10. Limitation of Liability
 *    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
 *    THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
 *    SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
 *    INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
 *    OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
 *    (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
 *    LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
 *    COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
 *    THE POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef SHR_DQ_H
#define SHR_DQ_H

/*******************************************************************************
 * Singly linked list (stack)
 */
/*!
 * \brief
 * Singly linked list structure
 */
typedef struct shr_sl_s {
    /*! Next link */
    struct shr_sl_s *next_node;
} shr_sl_t;

/*! Initialize singly linked list (stack) head */
#define SHR_SL_INIT(l)  do { ((l)->next_node) = NULL; } while (0)

/*! True if list empty */
#define SHR_SL_EMPTY(l) (!(l->next_node))

/*! Insert new head to the list */
#define SHR_SL_INSERT_HEAD(l, e)        \
do                                      \
{                                       \
    shr_sl_t *p_elem;                   \
    p_elem = (shr_sl_t*)(e);            \
    p_elem->next_node = (l)->next_node; \
    (l)->next_node = p_elem;            \
} while (0)

/*! Remove head from the list */
#define SHR_SL_REMOVE_HEAD(l, e)        \
do                                      \
{                                       \
    shr_sl_t *p_elem;                   \
    p_elem = (shr_sl_t*)(e);            \
    p_elem = (l)->next_node;            \
    e = (void *)p_elem;                 \
    (l)->next_node = p_elem->next_node; \
} while (0)


/*******************************************************************************
 * Doubly linked list (aka, double-ended queue)
 */

/*!
 * \brief Doubly linked list structure
 */
typedef struct shr_dq_s {
    /*! Forward link */
    struct shr_dq_s *flink;
    /*! Backward link */
    struct shr_dq_s *blink;
} shr_dq_t;


/*! Initialize doubly linked list */
#define SHR_DQ_INIT(q)  \
do                      \
{                       \
    (q)->flink = (q);   \
    (q)->blink = (q);   \
} while (0)

/*! Initialize element */
#define SHR_DQ_ELEM_INIT(q)  \
do                      \
{                       \
    (q)->flink = NULL;  \
    (q)->blink = NULL;  \
} while (0)

/*! True if element (e) is not in a list (null) */
#define SHR_DQ_NULL(e) ((e)->flink == NULL)

/*! True if queue (q) is empty */
#define SHR_DQ_EMPTY(q) ((q)->flink == (q))

/*! Get head of the queue */
#define SHR_DQ_HEAD(q, t) ((t) (q)->flink)

/*! Get tail of the queue */
#define SHR_DQ_TAIL(q, t) ((t) (q)->blink)

/*! Get next element of a given element */
#define SHR_DQ_NEXT(q, t) ((t) (q)->flink)

/*! Get prev element of a given element  */
#define SHR_DQ_PREV(q, t) ((t) (q)->blink)


/*!
 * Insert element to become new head to the doubly linked list
 *
 * \param [in] q pointer to head of queue
 * \param [in] e pointer to element (DQ) to insert, must not be in the list
 */
#define SHR_DQ_INSERT_HEAD(q, e)    \
do                                  \
{                                   \
    shr_dq_t *p_element;            \
    p_element = (shr_dq_t*) (e);    \
    p_element->flink = (q)->flink;  \
    p_element->blink =  (q);        \
    (q)->flink->blink = p_element;  \
    (q)->flink = p_element;         \
} while (0)


/*!
 * Insert element to become new tail to the doubly linked list
 *
 * \param [in] q pointer to head of queue
 * \param [in] e pointer to element (DQ) to insert, must not be in the list
 */
#define SHR_DQ_INSERT_TAIL(q, e)     \
do                                   \
{                                    \
    shr_dq_t *p_element;             \
    p_element = (shr_dq_t *) (e);    \
    p_element->flink = (q);          \
    p_element->blink = (q)->blink;   \
    (q)->blink->flink = p_element;   \
    (q)->blink = p_element;          \
} while (0)


/*!
 * Insert element previous to a given element in the doubly linked list
 *
 * \param [in] e pointer to previous element
 * \param [in] n pointer to new element to insert, must not be in the list
 */
#define SHR_DQ_INSERT_PREV(e, n)        \
do                                      \
{                                       \
    shr_dq_t *p_element;                \
    shr_dq_t *_p_new;                   \
    shr_dq_t *p_prev;                   \
    p_element = (shr_dq_t *) (e);       \
    _p_new = (shr_dq_t *) (n);          \
    p_prev = p_element->blink;          \
    p_prev->flink = _p_new;             \
    _p_new->blink =  p_prev;            \
    _p_new->flink = p_element;          \
    p_element->blink = _p_new;          \
} while (0)

/*!
 * Insert element previous to a given element in the doubly linked list
 *
 * \param [in] e pointer to next element
 * \param [in] n pointer to new element to insert, must not be in the list
 */
#define SHR_DQ_INSERT_NEXT(e, n)        \
do                                      \
{                                       \
    shr_dq_t *p_element;                \
    shr_dq_t *_p_new;                   \
    shr_dq_t *p_next;                   \
    p_element = (shr_dq_t *) (e);       \
    _p_new = (shr_dq_t *) (n);          \
    p_next = p_element->flink;          \
    p_next->blink = _p_new;             \
    _p_new->flink =  p_next;            \
    p_element->flink = _p_new;          \
    _p_new->blink = p_element;          \
} while (0)

/*!
 * \brief Remove a given element from its list
 *
 * \param [in] e Pointer to element (DQ) to remove
 */
#define SHR_DQ_REMOVE(e)                            \
do                                                  \
{                                                   \
    shr_dq_t *p_element;                            \
    p_element = (shr_dq_t *) (e);                   \
    p_element->blink->flink = p_element->flink;     \
    p_element->flink->blink = p_element->blink;     \
} while (0)

/*!
 * \brief Swap (update) an element with a new element.
 *
 * \param [in] e Pointer to element (DQ) to be replaced
 * \param [in] n Pointer to element (DQ) to replace with
 */
#define SHR_DQ_SWAP(e, n)                           \
do                                                  \
{                                                   \
    shr_dq_t *p_element;                            \
    shr_dq_t *_p_new;                               \
    p_element = (shr_dq_t *) (e);                   \
    _p_new = (shr_dq_t *) (n);                      \
    _p_new->flink = p_element->flink;               \
    _p_new->blink = p_element->blink;               \
    p_element->blink->flink = _p_new;               \
    p_element->flink->blink = _p_new;               \
} while (0)

/*!
 * \brief Remove the head from its list
 *
 * \param [in] q Pointer to queue block
 * \param [out] e Pointer to element (DQ) removed from q, the head
 */
#define SHR_DQ_REMOVE_HEAD(q, e)                    \
do                                                  \
{                                                   \
    shr_dq_t *p_element;                            \
    p_element = (q)->flink;                         \
    e = (void *)p_element;                          \
    p_element->blink->flink = p_element->flink;     \
    p_element->flink->blink = p_element->blink;     \
} while (0)

/*!
 * \brief Remove the tail from its list
 *
 * \param [in] q Pointer to queue block
 * \param [out] e Pointer to element (DQ) removed from q, the tail
 */
#define SHR_DQ_REMOVE_TAIL(q, e)                    \
do                                                  \
{                                                   \
    shr_dq_t *p_element;                            \
    p_element = (q)->blink;                         \
    e = (void *)p_element;                          \
    p_element->blink->flink = p_element->flink;     \
    p_element->flink->blink = p_element->blink;     \
} while (0)

/*!
 * Used to copy list head from one memory area
 * to a different memory area without affecting
 * contents of list
 *
 * \param [in] n new list head
 * \param [in] o old list head
 */
#define SHR_DQ_SWAP_HEAD(n, o)           \
do                                       \
{                                        \
    (n)->flink = (o)->flink;             \
    (n)->blink = (o)->blink;             \
    (o)->flink->blink = (n);             \
    (o)->blink->flink = (n);             \
} while (0)

/*!
 * This macro is a tidy way of performing subtraction to move from a
 * pointer within an object to a pointer to the object.
 *
 * \param [in] t  type of object to recover (e.g. shr_dq_t *)
 * \param [in] p  pointer to object from which to recover element pointer
 * \param [in] ep pointer to an object of type t
 * \param [in] f  name of the DQ field in t through which the queue is linked
 *
 * \return a pointer to the object, of type t
 */
#define SHR_DQ_ELEMENT(t, p, ep, f) \
    ((t) (((char *) (p)) - (((char *) &((ep)->f)) - ((char *) (ep)))))

/*!
 * SHR_DQ_ELEMENT_GET performs the same function as SHR_DQ_ELEMENT, but does not
 * require a pointer of type (t).  This form is preferred as SHR_DQ_ELEMENT
 * typically generate Coverity errors, and the (ep) argument is unnecessary.
 *
 * \param [in] t type of object to recover (e.g. shr_dq_t *)
 * \param [in] p pointer to object from which to recover element pointer
 * \param [in] f name of the DQ field in t through which the queue is linked
 *
 * \return a pointer to the object, of type t
 */
#define SHR_DQ_ELEMENT_GET(t, p, f) \
    ((t) (((char *) (p)) - (((char *) &(((t)(0))->f)))))

/*!
 * Traverse the list and apply a user specified function for each element.
 *
 * \param [in] q head of queue on which to map f
 * \param [in] f function to apply to each element of q
 * \param [in] a user data
 */
#define SHR_DQ_MAP(q, f, a)                              \
do                                                       \
{                                                        \
    shr_dq_t *e, *e0, *q0;                               \
    q0 = (q);                                            \
    for (e=q0->flink; e != q0; e = e0) {                 \
        e0 = e->flink;                                   \
        (void) (f)(e, a);                                \
    }                                                    \
} while (0)

/*!
 * Traverse the list and apply a user specified function for each element,
 * break on user specified function returning 1.
 * Can be used to search an element in the list.
 *
 * \param [in] q head of queue on which to map f
 * \param [in] f function to apply to each element of q; should return 1 when
 *       desired queue element has been found.
 * \param [in] a user data
 */
#define SHR_DQ_FIND(q, f, a)                             \
do                                                       \
{                                                        \
    shr_dq_t *e, *e0, *q0;                               \
    q0 = (q);                                            \
    for (e=q0->flink; e != q0; e = e0) {                 \
        e0 = e->flink;                                   \
        if ((f)(e, a)) break;                            \
    }                                                    \
} while (0)


/*!
 * Join two lists
 *
 * Queue 1 joins to queue 0 in the tail.
 *
 * \param [in] q0 pointer to head of queue 0
 * \param [in] q1 pointer to head of queue 1

 */
#define SHR_DQ_JOIN_TAIL(q0, q1)                         \
do                                                       \
{                                                        \
    shr_dq_t *h0, *t0, *pq;                              \
    pq = (q0);                                           \
    h0 = (q1)->flink;                                    \
    t0 = (q1)->blink;                                    \
    if (h0 && t0) {                                      \
        pq->blink->flink = h0;                           \
        h0->blink = pq->blink;                           \
        t0->flink = pq;                                  \
        pq->blink = t0;                                  \
    }                                                    \
}                                                        \
while (0)


/*!
 * \brief Get length of the doubly linked list
 *
 * \param [in] q Head of queue to determine length
 * \param [out] cnt Number of elements in the queue
 */
#define SHR_DQ_LENGTH(q, cnt)                            \
do                                                       \
{                                                        \
    shr_dq_t *e, *q0;                                    \
    (cnt) = 0;                                           \
    q0 = (q);                                            \
    for (e = q0->flink; e != q0; e = e->flink) {         \
        (cnt)++;                                         \
    }                                                    \
} while (0)                                              \

/*!
 * \brief Start to traverse the doubly linked list
 *
 * \param [in] q Head of queue
 * \param [in] e Each element during traverse
 */
#define SHR_DQ_TRAVERSE(q, e)                            \
do                                                       \
{                                                        \
    shr_dq_t *e0, *q0;                                   \
    q0 = (q);                                            \
    for (e=q0->flink; e != q0; e = e0) {                 \
        e0 = e->flink;                                   \

/*!
 * \brief Start to traverse the doubly linked list with a start node
 *
 * \param [in] q Head of queue
 * \param [in] s Start node
 * \param [in] e Each element during traverse
 */
#define SHR_DQ_TRAVERSE_FROM(q, s, e)                    \
do                                                       \
{                                                        \
    shr_dq_t *e0, *q0, *s0;                              \
    q0 = (q);                                            \
    s0 = (s);                                            \
    for (e=s0; e != q0; e = e0) {                        \
        e0 = e->flink;                                   \


/*!
 * \brief Start to traverse the doubly linked list with a start node
 *
 * \param [in] q Head of queue
 * \param [in] s Start node
 * \param [in] e Each element during traverse
 */
#define SHR_DQ_TRAVERSE_FROM2(q, s, e)                   \
do                                                       \
{                                                        \
    shr_dq_t *e1, *q1, *s1;                              \
    q1 = (q);                                            \
    s1 = (s);                                            \
    for (e=s1; e != q1; e = e1) {                        \
        e1 = e->flink;                                   \


/*!
 * \brief Start to back traverse the doubly linked list
 *
 * \param [in] q Head of queue
 * \param [in] e Each element during traverse
 */
#define SHR_DQ_BACK_TRAVERSE(q, e)                       \
do                                                       \
        {                                                \
            shr_dq_t *e0, *q0;                           \
            q0 = (q);                                    \
            for (e=q0->blink; e != q0; e = e0) {         \
                e0 = e->blink;                           \


/*!
 * \brief Start to back traverse the doubly linked list with a start node
 *
 * \param [in] q Head of queue
 * \param [in] s Start node
 * \param [in] e Each element during traverse
 */
#define SHR_DQ_BACK_TRAVERSE_FROM(q, s, e)               \
do                                                       \
        {                                                \
            shr_dq_t *e0, *q0, *s0;                      \
            q0 = (q);                                    \
            s0 = (s);                                    \
            for (e=s0; e != q0; e = e0) {                \
                e0 = e->blink;                           \


/*!
 * \brief Start to back traverse the doubly linked list with a start node
 *
 * \param [in] q Head of queue
 * \param [in] s Start node
 * \param [in] e Each element during traverse
 */
#define SHR_DQ_BACK_TRAVERSE_FROM2(q, s, e)              \
do                                                       \
        {                                                \
            shr_dq_t *e1, *q1, *s1;                      \
            q1 = (q);                                    \
            s1 = (s);                                    \
            for (e=s1; e != q1; e = e1) {                \
                e1 = e->blink;                           \


/*!
 * \brief Complete traversing the doubly linked list
 *
 * \param [in] q Head of queue
 * \param [in] e Each element during traverse
 */
#define SHR_DQ_TRAVERSE_END(q, e)                        \
            }                                            \
        } while (0)

#endif /* SHR_DQ_H */
