// main.c

#include "list.h"
#include <stdio.h>

void freeInt(void *pItem)
{
    int *intItem = (int *)pItem;
    printf("\nFreeing integer: %d", *intItem);
    pItem = NULL;
}

bool intSearch(void *pItem, void *item)
{
    return ((*(int *)(pItem)) == (*(int *)item));
}

bool test_list_create()
{
    List *list1 = List_create();
    List *list2 = List_create();
    List *list3 = List_create();
    List_free(list1, freeInt);
    List_free(list2, freeInt);
    List *list4 = List_create();
    return (list1 && list2 && list3);
}

bool test_list_count(List *pList)
{
    int count = List_count(pList);
    return (count == 0);
}

bool test_append(List *pList)
{
    int item0 = 0;
    int item1 = 1;
    int item2 = 2;
    int item3 = 3;
    int item4 = 4;

    List_append(pList, &item0);
    List_append(pList, &item1);
    List_append(pList, &item2);
    List_append(pList, &item3);
    List_append(pList, &item4);

    List_first(pList);
    while (List_curr(pList) && *(int *)List_curr(pList) == item0)
    {
        // printf("\nlook %d ", *(int *)List_curr(pList));
        item0++;
        List_next(pList);
    }
    return (pList->current_item == NULL);
}

bool test_prepend(List *pList, void *item5, void *item6, void *item7, void *item8, void *item9)
{

    List_prepend(pList, item5);
    List_prepend(pList, item6);
    List_prepend(pList, item7);
    List_prepend(pList, item8);
    List_prepend(pList, item9);

    // List_first(pList);
    // printf("\nlook %d ", *(int*)List_curr(pList));
    while (List_curr(pList))
    {
        // printf("\nlook my gawd%d %d ", *((int*)List_curr(pList)), pList->state);
        List_next(pList);
    }
    printf("\n");
    return (pList->current_item == NULL);
}

bool test_insert_after(List *pList, void *item)
{
    List_insert_after(pList, item);
    List_first(pList);
    while (List_curr(pList))
    {
        // printf("\nlook my gawd %d %d ", *(int*)List_curr(pList), pList->state);
        List_next(pList);
    }
    printf("\n");
    return (pList->current_item == NULL);
}

bool test_insert_before(List *pList, void *item)
{
    List_first(pList);
    int val = List_insert_before(pList, item);
    List_first(pList);
    while (List_curr(pList))
    {
        // printf("\nlook my gawd test before%d %d ", *(int*)List_curr(pList), pList->state);
        List_next(pList);
    }
    printf("\n");
    return (!val);
}

bool test_first(List *pList)
{
    void *first = List_first(pList);
    int item0 = 0;
    List_append(pList, &item0);
    void *second = List_first(pList);
    return (first == NULL && *((int *)second) == 0);
}

bool test_remove(List *pList)
{
    //current is at end of list
    if (pList->state == LIST_OOB_END)
    {
        List_prev(pList);   //current is at the end
        List_remove(pList); //remove last node current is now NULL OOB
        List_first(pList);
        while (List_curr(pList))
        {
            // printf("\nlook my gawd test remove%d %d ", *(int*)List_curr(pList), pList->state);
            List_next(pList);
        }
        printf("\n");
    }
    List_first(pList);
    List_prev(pList);
    List_prev(pList);
    List_next(pList); // current should be the first number
    // printf("removing from list: %d", *((int *)List_remove(pList)));
    while (List_curr(pList))
    {
        // printf("\nlook my gawd test remove 2 %d %d ", *(int*)List_curr(pList), pList->state);
        List_next(pList);
    }
    List_first(pList);
    List_next(pList);
    List_next(pList);
    List_remove(pList);
    while (List_curr(pList))
    {
        // printf("\nlook my gawd test remove 3 %d %d ", *(int*)List_curr(pList), pList->state);
        List_next(pList);
    }
    return 1;
}

bool test_trim(List *pList, List *emptyList)
{
    //current is beyond state
    // printf("trimming from list: %d", *((int *)List_trim(pList)));
    List_trim(pList); //removes last item (15) //current is now 16
    List_first(pList);
    while (List_curr(pList))
    {
        // printf("\nlook my gawd test trim %d %d ", *(int*)List_curr(pList), pList->state);
        List_next(pList);
    }
    List_prev(pList);
    void *m = List_trim(emptyList);
    return (16 == *((int *)pList->current_item->item) && (m == NULL));
}

bool test_concat(List *pList1, List *pList2)
{
    List_concat(pList1, pList2);
    List_first(pList1);
    while (List_curr(pList1))
    {
        // printf("\nlook my gawd test concat %d %d ", *(int*)List_curr(pList1), pList1->state);
        List_next(pList1);
    }
    return 1;
}

bool test_search(List *pList, void *item)
{
    List_first(pList);
    List_search(pList, intSearch, item);

    return (1);
}

int main()
{
    List *list5 = List_create();
    List *list6 = List_create();
    int item = 21;
    int item10 = 22;
    int item5 = 15;
    int item6 = 16;
    int item7 = 17;
    int item8 = 18;
    int item9 = 19;

    int item21 = 50;
    int item22 = 51;
    int item23 = 52;
    int item24 = 53;
    int item25 = 54;
    int item26 = 55;
    int item27 = 56;
    // printf("test list count: %d\n",test_list_count(list5));
    // printf("test list append: %d\n",test_append(list5));    
    printf("test list prepend: %d\n", test_prepend(list5, &item5, &item6, &item7, &item8, &item9));
    printf("test list insert after: %d\n", test_insert_after(list5, &item));
    printf("test list insert before: %d\n", test_insert_before(list5, &item10));
    printf("\ntest list remove: %d\n", test_remove(list5));
    printf("\ntest list trim %d", test_trim(list5, list6));

    List_append(list6, &item21);
    List_append(list6, &item22);
    List_append(list6, &item23);
    List_prepend(list6, &item24);
    List_insert_after(list6, &item25);
    List_insert_before(list6, &item26);
    List_prev(list6);
    List_prev(list6);
    List_next(list6);
    List_insert_before(list6, &item27); // all nodes are now occupied
    List_first(list6);

    while (List_curr(list6))
    {
        // printf("\nlook my gawd test main %d %d ", *(int*)List_curr(list6), list6->state);
        List_next(list6);
    }

    printf("\ntest list concat %d", test_concat(list6, list5));
    printf("\ntest list search %d", test_search(list6, &item8));
    List_free(list6,freeInt);
    

    // List *list4 = List_create();
    // printf("list4 create %d\n", list4 == NULL);
    // List *list7 = List_create();
    // printf("list7 create %d\n", list7 == NULL);
    // List *list8 = List_create();
    // printf("list8 create  %d\n", list8 == NULL);
    // printf("list5:%d list6:%d list4:%d list7: %d\n", list5->count, list6->count, list4->count, list7->count);
    // List_append(list7, &item10);
    // List_append(list7, &item10);
    // List_append(list7, &item10);
    // List_append(list7, &item10);
    // List_append(list7, &item10);
    // List_append(list7, &item10);


    // List_append(list7, &item10);
    // printf("list5:%d list6:%d list4:%d list7: %d\n", list5->count, list6->count, list4->count, list7->count);
    // List_free(list6, freeInt);
    // List_append(list7, &item10);
    // printf("list5:%d list6:%d list4:%d list7: %d\n", list5->count, list6->count, list4->count, list7->count);
    // // printf("\nremoving from list 7: %d",*((int*)List_remove(list7)));
    // List_concat(list4,list7);
    // printf("\nlist5:%d list6:%d list4:%d list7: %d\n", list5->count, list6->count, list4->count, list7->count); //list 5will show as 1 because list4 and list1 poit to the same pointer
    // List_concat(list4, list7);
    // printf("\nlist5:%d list6:%d list4:%d list7: %d\n", list5->count, list6->count, list4->count, list7->count); //list 5will show as 1 because list4 and list1 poit to the same pointer

    // printf("test list first: %d\n",test_first(list5)); // adding an element inside the test

    return 0;
}
