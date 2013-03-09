/*
 * esyslog_helper.h
 *
 *  Created on: Nov 2, 2010
 *      Author: mralex
 */

#ifndef ESYSLOG_HELPER_H_
#define ESYSLOG_HELPER_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/module.h>

#define OS_ALLOC(size) ({ \
    void* ptr; \
    if(in_interrupt()) { \
      ptr = kmalloc(size, GFP_ATOMIC); \
   } else { \
      ptr = kmalloc(size, GFP_KERNEL); \
   } \
   ptr; \
   })
/** Free kernel memory. */
#define OS_FREE(ptr)                \
   do {if ((ptr) != NULL) {kfree((void*)ptr); ptr = NULL;}}while(0)

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	OS_ALLOC(size) 	malloc(size)
#define	OS_FREE(ptr) 	free(ptr)
#endif

/**
 * \brief access to object container
 * данный макрос используется в случаях если в структуру потомка
 * вставлен указатель на предка. Когда мы находимся в перегруженной
 * функции предка и у нас есть указатель на него, то получить указатель на потомка
 * можно указав данному макросу этот указатель, тип потомка и имя поля предка
 * в структуре потомка.
 * \param pObj указатель на аггрегированный объект (предок)
 * \param containerType тип потомка
 * \param element имя поля в котором находится предок
 * \return указатель на потомка
 */
#define CMN_GetContainer(pObj, containerType, element)\
	((containerType*)((char*)(pObj)-(unsigned long)(&((containerType*)0)->element)))

/*
 * These are non-CMN_NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#ifndef CMN_LIST_POISON1
#define CMN_LIST_POISON1  ((void *) 0x00100100)
#endif

#ifndef CMN_LIST_POISON2
#define CMN_LIST_POISON2  ((void *) 0x00200200)
#endif

/* prefetch support*/
#ifndef CMN_PREFETCH
#define CMN_PREFETCH
static inline void CMN_prefetch(const void *x) {;}
#endif

/**
 * @brief Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
typedef struct CMN_list_head {
	struct CMN_list_head *next, *prev;
} *pCMN_list_head_t;


#define CMN_LIST_HEAD_INIT(name) { &(name), &(name) }

#define CMN_LIST_HEAD(name) \
	struct CMN_list_head name = CMN_LIST_HEAD_INIT(name)

static inline void CMN_INIT_LIST_HEAD(struct CMN_list_head *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __CMN_list_add(struct CMN_list_head *newList,
			      struct CMN_list_head *prevList,
			      struct CMN_list_head *nextList)
{
	nextList->prev = newList;
	newList->next = nextList;
	newList->prev = prevList;
	prevList->next = newList;
}

/**
 * @brief CMN_list_add - add a new entry
 * @param new new entry to be added
 * @param head list head to add it after
 * @remark
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void CMN_list_add(struct CMN_list_head *newList, struct CMN_list_head *headList)
{
	__CMN_list_add(newList, headList, headList->next);
}


/**
 * @brief CMN_list_add_tail - add a new entry
 * @param new new entry to be added
 * @param head list head to add it before
 * @remark
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void CMN_list_add_tail(struct CMN_list_head *newList, struct CMN_list_head *headList)
{
	__CMN_list_add(newList, headList->prev, headList);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __CMN_list_del(struct CMN_list_head * prevList, struct CMN_list_head * nextList)
{
	nextList->prev = prevList;
	prevList->next = nextList;
}

/**
 * @brief Deletes entry from list.
 * @param entry the element to delete from the list.
 * @remark list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void CMN_list_del(struct CMN_list_head *entry)
{
	__CMN_list_del(entry->prev, entry->next);
	entry->next = (pCMN_list_head_t)CMN_LIST_POISON1;
	entry->prev = (pCMN_list_head_t)CMN_LIST_POISON2;
}

/**
 * @brief CMN_list_empty - tests whether a list is empty
 * @param head the list to test.
 */
static inline int CMN_list_empty(const struct CMN_list_head *head)
{
	return head->next == head;
}

/**
 * @brief CMN_list_entry - get the struct for this entry
 * @param ptr points to struct CMN_list_head pointer.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the list_struct within the struct.
 */
#define CMN_list_entry(ptr, type, member) \
	CMN_GetContainer(ptr, type, member)

/**
 * @brief CMN_list_first_entry - get the first element from a list
 * @param ptr	the list head to take the element from.
 * @param type	the type of the struct this is embedded in.
 * @param member	the name of the list_struct within the struct.
 * @remark
 * Note, that list is expected to be not empty.
 */
#define CMN_list_first_entry(ptr, type, member) \
	CMN_list_entry((ptr)->next, type, member)

/**
 * @brief CMN_list_for_each	-	iterate over a list
 * @param pos the &struct CMN_list_head to use as a loop cursor.
 * @param head the head for your list.
 */
#define CMN_list_for_each(pos, head) \
	for (pos = (head)->next; CMN_prefetch(pos->next), pos != (head); \
        	pos = pos->next)



#endif /* ESYSLOG_HELPER_H_ */
