/*
 * lkd.c
 *
 *  Created on: Apr 23, 2024
 *      Author: James DESTINE
 */

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "lkd.h"



List* createList(void) 
{
    List* list = malloc(sizeof(List));
    if (list == NULL) {
        printf("Failed to allocate memory for the list.\n");
        return NULL;
    }

    list->capacity = 2; // Initial capacity
    list->size = 0;
    list->items = malloc(list->capacity * sizeof(BouleItem));
    if (list->items == NULL) {
        printf("Failed to allocate memory for the list elements.\n");
        free(list);
        return NULL;
    }
    return list;
}

void destroyList(List* list) 
{
    if (list != NULL) {
        free(list->items);
        free(list);
    }
}

int getElement(List *list, BouleItem item)
{
    int i = 0;
    while (i < list->size)
    {
        if (strcmp(list->items[i].boul, item.boul) == 0 && strcmp(list->items[i].lotto, item.lotto) == 0 && strcmp(list->items[i].option, item.option) == 0)
            return 0;
        i++;
    }
    return -1;
}


void addElement(List* list, BouleItem element) 
{
    int s = getElement(list, element);
    BouleItem* newElements = NULL;

    if (s == 0)
    {
        return;
    } else
    {
        if (list->size >= list->capacity) {
            list->capacity *= 2;
            newElements = (BouleItem*)realloc(list->items, list->capacity * sizeof(BouleItem));
            if (newElements == NULL) {
                printf("Failed to reallocate memory for the list elements.\n");
                return;
            }
            list->items = newElements;
        }

        list->items[list->size] = element;
        list->size++;
    }
}

void deleteByIndex(List* list, int index) 
{
	int i = 0;
    if (index < 0 || index >= list->size) {
        printf("Invalid index.\n");
        return;
    }
    i = (int)index;
    // Shift elements to the left to overwrite the element to be deleted
    while (i < list->size - 1) {
        list->items[i] = list->items[i + 1];
        i++;
    }
    list->size--; // Decrement the size to reflect the deleted element
}

