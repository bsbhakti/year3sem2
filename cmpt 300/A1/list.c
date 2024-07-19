#include "list.h"
#include <stddef.h>
#include <stdio.h>

List listOfHeads[LIST_MAX_NUM_HEADS];
Node listOfNodes[LIST_MAX_NUM_NODES];

static bool initialized = false;
static int countOfHeads = 0;
static int countOfNodes = 0;
static Node *headFreeNode;
static List *headFreeList;

Node *find_free_node(void *pItem)
{
    Node *newNode;
    if (headFreeNode != NULL)
    {
        newNode = headFreeNode;
        // printf("\nlook here %ld", newNode - listOfNodes);
        newNode->item = pItem;
        headFreeNode = headFreeNode->next;
        newNode->next = NULL;
        // printf("\ndone with find free node %d\n", *((int*)newNode->item));
        return newNode;
    }
    else
    {
        newNode = &listOfNodes[countOfNodes];
        listOfNodes[countOfNodes].item = pItem;
    }
    // printf("\ndone with find free node %d\n", *((int*)newNode->item));
    return &listOfNodes[countOfNodes];
    // return newNode;
}

void insert_free_node(Node *removed)
{
    if (headFreeNode == NULL)
    {
        // printf("\n removing %d",*((int*)pList->current_item->item));
        headFreeNode = removed;
        headFreeNode->next = NULL;
        // printf("\n removing %d", *((int *)headFreeNode->item));
    }
    else
    {
        // Node *temp = headFreeNode;
        removed->next = headFreeNode;
        headFreeNode = removed;
        // printf("\n removing %d", *((int *)headFreeNode->item));
    }
}

List *find_free_list()
{
    List *newList;
    // printf("\nlook here inside find free list");
    if (headFreeList != NULL)
    {
        newList = headFreeList;
        headFreeList = headFreeList->nextFreeList;
        // printf("\nlook here inside find free list at index %ld", newList - listOfHeads);
    }
    else
    {
        // printf("\ngoing here1 at index %d", countOfHeads);
        newList = &listOfHeads[countOfHeads];
    }
    newList->count = 0;
    newList->current_item = NULL;
    newList->head = NULL;
    newList->nextFreeList = NULL;
    newList->state = 2;
    newList->tail = NULL;
    // printf("\n end");
    return newList;
}

List *List_create()
{
    if (!initialized)
    {
        for (int i = 0; i < LIST_MAX_NUM_HEADS; i++)
        {
            listOfHeads[i].head = NULL;
            listOfHeads[i].current_item = NULL;
            listOfHeads[i].tail = NULL;
            listOfHeads[i].count = 0;
            listOfHeads[i].state = LIST_OOB_START;
            listOfHeads[i].nextFreeList = NULL;
        }
        for (int i = 0; i < LIST_MAX_NUM_NODES; i++)
        {
            listOfNodes[i].item = NULL;
            listOfNodes[i].next = NULL;
            listOfNodes[i].prev = NULL;
        }
        initialized = true;
        headFreeNode = NULL;
        headFreeList = NULL;
    }
    if (countOfHeads < LIST_MAX_NUM_HEADS)
    {
        List *newList = find_free_list();
        newList->head = NULL;
        newList->tail = NULL;
        newList->count = 0;
        newList->nextFreeList = NULL;
        newList->current_item = NULL;
        newList->state = LIST_OOB_START;
        countOfHeads++;
        return newList;
    }
    return NULL;
}

int List_count(List *pList)
{
    return pList->count;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void *List_first(List *pList)
{
    if (pList->count == 0)
    {
        // printf("\n in list first if");
        pList->current_item = NULL;
        return NULL;
    }
    pList->current_item = pList->head;
    pList->state = 2;
    // printf("\n in list first %d", *((int *)pList->current_item->item));
    return pList->current_item->item;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void *List_last(List *pList)
{
    if (pList->count == 0)
    {
        pList->current_item = NULL;
        pList->state = LIST_OOB_START;
        return NULL;
    }
    pList->current_item = pList->tail;
    pList->state = 2;
    return pList->current_item->item;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer
// is returned and the current item is set to be beyond end of pList.
void *List_next(List *pList)
{
    Node *current = pList->current_item;

    if (current == pList->tail)
    {
        // printf("\n in list next if tail ");

        pList->current_item = NULL;
        pList->state = LIST_OOB_END;
        return NULL;
    }
    else if (pList->state == LIST_OOB_START)
    {
        // printf("\n in in list next else if");

        pList->current_item = pList->head;
        pList->state = 2;
        return pList->current_item->item;
    }
    else if (pList->state == LIST_OOB_END)
    {
        pList->current_item = NULL;
        pList->state = LIST_OOB_END;
        // printf("\n in next else if end");
        return NULL;
    }
    else
    {
        pList->current_item = pList->current_item->next;
        // printf("\n in next after %d head:%d ", *((int*)pList->current_item->item),*((int*)pList->head->item));
        return pList->current_item->item;
    }
}

// Backs up pList's current item by one, and returns a pointer to the new current item.
// If this operation backs up the current item beyond the start of the pList, a NULL pointer
// is returned and the current item is set to be before the start of pList.
void *List_prev(List *pList)
{
    Node *current = pList->current_item;
    if (current == pList->head)
    {
        pList->current_item = NULL;
        pList->state = LIST_OOB_START;
        return NULL;
    }
    else if (pList->state == LIST_OOB_START)
    {
        return NULL;
    }
    else if (pList->state == LIST_OOB_END)
    {
        pList->current_item = pList->tail;
        pList->state = 2;
        return pList->current_item->item;
    }
    else
    {
        current = current->prev;
        pList->current_item = current;
        return pList->current_item->item;
    }
}

// Returns a pointer to the current item in pList.
void *List_curr(List *pList)
{
    if (pList->current_item == NULL)
    {
        return NULL;
    }
    return pList->current_item->item;
}

// Adds the new item to pList directly after the current item, and makes item the current item.
// If the current pointer is before the start of the pList, the item is added at the start. If
// the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_after(List *pList, void *pItem)
{

    if (countOfNodes < LIST_MAX_NUM_NODES)
    {
        Node *newNode = find_free_node(pItem);

        if (pList->count == 0)
        {
            pList->head = newNode;
            pList->tail = newNode;
            pList->current_item = newNode;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            return LIST_SUCCESS;
        }

        else if (pList->state == LIST_OOB_START)
        {
            // printf("\n in insert after start");
            Node *temp = pList->head;
            pList->head = newNode;
            pList->head->next = temp;
            pList->head->prev = NULL;
            temp->prev = pList->head;
            pList->current_item = pList->head;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            return LIST_SUCCESS;
        }
        else if (pList->state == LIST_OOB_END)
        {
            Node *temp = pList->tail;
            pList->tail = newNode;
            pList->tail->prev = temp;
            pList->tail->next = NULL;
            pList->current_item = pList->tail;
            pList->state = 2;
            temp->next = pList->tail;
            pList->count++;
            countOfNodes++;
            // printf("\n in insert after end %d",*((int *)newNode->item));
            return LIST_SUCCESS;
        }
        else if (pList->current_item == pList->tail)
        {
  
            pList->tail->next = newNode;
            newNode->prev = pList->tail;
            newNode->next = NULL;
            pList->tail = newNode;
            pList->current_item = newNode;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            return LIST_SUCCESS;
        }
        else
        {
            Node *temp = pList->current_item->next;
            pList->current_item->next = newNode;
            newNode->prev = pList->current_item;
            pList->current_item = newNode;
            pList->state = 2;
            pList->current_item->next = temp;
            pList->count++;
            countOfNodes++;
            // printf("\nin insert after look %d at index %ld", *((int*)pList->current_item->item), newNode - listOfNodes);

            return LIST_SUCCESS;
        }
    }

    return LIST_FAIL;
}

// Adds item to pList directly before the current item, and makes the new item the current one.
// If the current pointer is before the start of the pList, the item is added at the start.
// If the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_before(List *pList, void *pItem)
{
    if (countOfNodes < LIST_MAX_NUM_NODES)
    {

        Node *newNode = find_free_node(pItem);

        if (pList->count == 0)
        {
            pList->head = newNode;
            pList->tail = newNode;
            pList->current_item = newNode;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            return LIST_SUCCESS;
        }
        else if (pList->state == LIST_OOB_START)
        {
            Node *temp = pList->head;
            pList->head = newNode;
            pList->head->next = temp;
            pList->head->prev = NULL;
            temp->prev = pList->head;
            pList->count++;
            countOfNodes++;
            pList->current_item = pList->head;
            pList->state = 2;
            return LIST_SUCCESS;
        }
        else if (pList->state == LIST_OOB_END)
        {
            Node *temp = pList->tail;
            pList->tail = newNode;
            pList->tail->prev = temp;
            pList->tail->next = NULL;
            temp->next = pList->tail;
            pList->count++;
            countOfNodes++;
            pList->current_item = pList->tail;
            pList->state = 2;
            return LIST_SUCCESS;
        }
        else if (pList->current_item == pList->head)
        {
            pList->head->prev = newNode;
            newNode->prev = NULL;
            newNode->next = pList->head;
            pList->head = newNode;
            pList->current_item = newNode;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            return LIST_SUCCESS;
        }
        else
        {
            Node *temp = pList->current_item->prev;
            newNode->next = pList->current_item;
            newNode->prev = temp;
            temp->next = newNode;
            pList->current_item->prev = newNode;
            pList->current_item = newNode;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            // printf("\nin insert before look %d at index %d", *((int*)pList->current_item->item), newNode - listOfNodes);
            return LIST_SUCCESS;
        }
    }
    return LIST_FAIL;
}

// Adds item to the end of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_append(List *pList, void *pItem)
{

    if (countOfNodes < LIST_MAX_NUM_NODES)
    {
        Node *newNode = find_free_node(pItem);

        if (pList->head == NULL)
        {
            pList->head = newNode;
            pList->tail = newNode;
            pList->current_item = newNode;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            // printf("\n going here first %d at tail and head %d ",*((int*)pList->head->item),*((int*)cur->item));
            return LIST_SUCCESS;
        }

        else
        {
            Node *temp = pList->tail;
            pList->tail->next = newNode;
            pList->tail = newNode;
            pList->tail->prev = temp;
            pList->current_item = pList->tail;
            pList->state = 2;
            pList->count++;
            countOfNodes++;
            Node *cur = pList->current_item;
            // printf("\n going here second %d  %d",*((int*)pList->tail->item),*((int*)cur->item));
            return LIST_SUCCESS;
        }
    }
    return LIST_FAIL;
}

// Adds item to the front of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_prepend(List *pList, void *pItem)
{
    // printf("\n going here prepend %d",countOfNodes);
    if (countOfNodes < LIST_MAX_NUM_NODES)
    {
        Node *newNode = find_free_node(pItem);

        if (pList->head == NULL)
        {
            pList->head = newNode;
            pList->tail = newNode;
            pList->count++;
            countOfNodes++;
            // printf("look herein prepend inside if %d \n",*( (int*)pList->head->item));
        }
        else
        {
            Node *temp = pList->head;
            pList->head->prev = newNode;
            pList->head = newNode;
            newNode->next = temp;
            pList->count++;
            countOfNodes++;
        }
        pList->current_item = pList->head;
        pList->state = 2;
        Node *cur = pList->current_item;
        // printf("\n going here prepend last %d  %d",*((int*)pList->head->item),*((int*)(pList->current_item->item)));
        return LIST_SUCCESS;
    }
    return LIST_FAIL;
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void *List_remove(List *pList)
{
    if (pList->state == LIST_OOB_END || pList->state == LIST_OOB_START)
    {
        return NULL;
    }

    if (pList->current_item == pList->head && pList->count != 1)
    {
        Node *temp = pList->head;
        temp->next->prev = NULL;
        pList->head = temp->next;
        insert_free_node(pList->current_item);
        pList->current_item = pList->head;
        pList->state = 2;
        temp->next = NULL;
        temp->prev = NULL;
        pList->count--;
        countOfNodes--;
        return temp->item;
    }
    else if (pList->current_item == pList->head && pList->count == 1)
    {
        void *temp = pList->head->item;
        insert_free_node(pList->current_item);
        pList->head = NULL;
        pList->tail = NULL;
        pList->current_item = NULL;
        pList->state = LIST_OOB_END;
        pList->count--;
        countOfNodes--;
        return temp;
    }
    else if (pList->current_item == pList->tail)
    {
        // printf("\n in else if of remove %d",*((int*)headFreeNode->item));
        Node *temp = pList->current_item;
        pList->tail = pList->tail->prev;
        pList->tail->next = NULL;
        pList->current_item = NULL;
        pList->state = LIST_OOB_END;
        temp->next = NULL;
        temp->prev = NULL;
        insert_free_node(temp);
        pList->count--;
        countOfNodes--;
        return temp->item;
    }
    else
    {
        Node *temp = pList->current_item;
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        pList->current_item = temp->next;
        temp->next = NULL;
        temp->prev = NULL;
        insert_free_node(temp);
        pList->count--;
        countOfNodes--;
        return temp->item;
    }

}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void *List_trim(List *pList)
{
    if (pList->count == 0)
    {
        return NULL;
    }

    void *item;

    if (pList->count == 1)
    {
        item = pList->tail->item;
        pList->tail->next = NULL;
        pList->head->next = NULL;
        insert_free_node(pList->tail);
        pList->tail = NULL;
        pList->head = NULL;
        pList->current_item = NULL;
        pList->state = LIST_OOB_END;
    }
    else
    {
        Node *temp = pList->tail;
        item = temp->item;
        pList->tail = temp->prev;
        pList->tail->next = NULL;
        pList->current_item = pList->tail;
        temp->prev = NULL;
        temp->next = NULL;
        insert_free_node(temp);
    }
    pList->count--;
    countOfNodes--;

    return item;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1.
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List *pList1, List *pList2)
{
    if (pList2->head != NULL && pList1->head != NULL)
    {
        pList1->tail->next = pList2->head;
        pList1->tail = pList2->tail;
        pList2->head->prev = pList1->tail;
        pList1->count += pList2->count;

        pList2->count = 0;
        pList2->current_item = NULL;
        pList2->head = NULL;
        pList2->state = 2;
        pList2->tail = NULL;
        countOfHeads--;
    }
    else if (pList2->head != NULL && pList1->head == NULL)
    {
        pList1->tail = pList2->tail;
        pList1->head = pList2->head;
        pList1->count += pList2->count;

        pList2->count = 0;
        pList2->current_item = NULL;
        pList2->head = NULL;
        pList2->state = LIST_OOB_START;
        pList2->tail = NULL;
        countOfHeads--;
    }
    if (headFreeList == NULL)
    {
        headFreeList = pList2;
        headFreeList->nextFreeList = NULL;
    }
    else
    {
        pList2->nextFreeList = headFreeList;
        headFreeList = pList2;
    }
    return;
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item.
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are
// available for future operations.
typedef void (*FREE_FN)(void *pItem);
void List_free(List *pList, FREE_FN pItemFreeFn)
{
    Node *current = pList->head;
    while (current != NULL)
    {
        Node *next = current->next;
        if (pItemFreeFn != NULL)
        {
            (*pItemFreeFn)(current->item);
        }
       
        current->next = NULL;
        current->prev = NULL;
        insert_free_node(current);
        current = next;
        countOfNodes--;
    }
    if (headFreeList == NULL)
    {
        headFreeList = pList;
        headFreeList->nextFreeList = NULL;
    }
    else
    {
        pList->nextFreeList = headFreeList;
        headFreeList = pList;
    }
    pList->head = NULL;
    pList->tail = NULL;
    pList->current_item = NULL;
    pList->count = 0;
    pList->state = LIST_OOB_START;
    countOfHeads--;
}

typedef bool (*COMPARATOR_FN)(void *pItem, void *pComparisonArg);
void *List_search(List *pList, COMPARATOR_FN pComparator, void *pComparisonArg)
{
    if (pList->head != NULL && pList->state != LIST_OOB_END)
    {
        {
            Node *current;
            if (pList->state == LIST_OOB_START)
            {
                current = pList->head;
            }
            else
            {
                current = pList->current_item;
            }
            while (current != NULL)
            {
                if (pComparator(current->item, pComparisonArg))
                {
                    pList->current_item = current;
                    // printf("match found %d\n",*((int*)pList->current_item->item));
                    return pList->current_item->item;
                }
                current = current->next;
            }
            pList->current_item = NULL;
            pList->state = LIST_OOB_END;
        }
    }

    return NULL;
}