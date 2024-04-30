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


static int getRevers(int boul)
{
	int r = 0;
	int _boul = boul;
	while (_boul > 0)
	{
		r = r*10 + _boul % 10;
		_boul /= 10;
	}
	return r;
}


void addBoulePaire(List *list, char *pri)
{
	int i = 0;
	while (i < 10)
	{
		BouleItem item = {0, "", "", "10", "1"};
		sprintf(&item.boul, "%d%d", i, i);
		sprintf(&item.pri, "%s", pri);
		setOptionBouleItem(&item);
		addElement(list, item);
		i++;
	}
    return;
}


void addBouleTriple(List *list, char *pri)
{
	int i = 0;
	while (i < 10)
	{
		BouleItem item = {0, "", "", "10", "1"};
		sprintf(&item.boul, "%d%d%d", i, i, i);
		sprintf(&item.pri, "%s", pri);
		setOptionBouleItem(&item);
		addElement(list, item);
		i++;
	}
    return;
}


void addBouleRevers(List *list, char *pri)
{
	int i = 0, i2 = 0;
	int size = list->size;
	while (i < size)
	{
		BouleItem item = list->items[i];
		if (strcmp(item.lotto, "BO") == 0)
		{
			int r = getRevers(atoi(item.boul));
			i2 = i;
			while (i2 < size)
			{
				BouleItem item2 = list->items[i2];
				if ((atoi(item2.boul) - r) != 0 && r > 0)
				{
					BouleItem nitem = {0, "", "", "10", "1"};
					sprintf(&nitem.boul, "%02d", r);
					sprintf(&nitem.pri, "%s", pri);
					setOptionBouleItem(&nitem);
					addElement(list, nitem);
				}
				i2++;
			}
		}
		i++;
	}
    return;
}


void addBouleMarriage(List *list, char *pri)
{
	int i = 0, y = 0;
	while (i < list->size - 1)
	{
		y = i;
		while (y < list->size)
		{
			// Only reverse number of two digits
			if (strcmp(list->items[i].lotto, "BO") == 0 && strcmp(list->items[y + 1].lotto, "BO") == 0)
			{
				BouleItem nitem = {0, "", "", "10", ""};
				sprintf(&nitem.boul, "%s%s", list->items[i].boul, list->items[y + 1].boul);
				sprintf(&nitem.pri, "%s", pri);
				sprintf(&nitem.lotto, "%s", "MA");
				setOptionBouleItem(&nitem);
				addElement(list, nitem);
			}
			y++;
		}
		i++;
	}
	return;
}


void addBouleL4(List *list, char *pri)
{
	int i = 0, y = 0;
	while (i < list->size - 1)
	{
		y = i;
		while (y < list->size)
		{
			// Only add number of two digits
			if (strcmp(list->items[i].lotto, "BO") == 0 && strcmp(list->items[y + 1].lotto, "BO") == 0)
			{
				BouleItem nitem = {0, "", "", "10", ""};
				sprintf(&nitem.boul, "%s%s", list->items[i].boul, list->items[y + 1].boul);
				sprintf(&nitem.pri, "%s", pri);
				sprintf(&nitem.lotto, "%s", "L4");
				sprintf(&nitem.option, "%s", "1");
				setOptionBouleItem(&nitem);
				addElement(list, nitem);
				sprintf(&nitem.boul, "%s%s", list->items[y+1].boul, list->items[i].boul);
				addElement(list, nitem);


			}
			y++;
		}
		i++;
	}
	return;
}


void addPwent(List *list, char *pri, unsigned int pwent)
{
	int i = 0;
	while (i < 10)
	{
		BouleItem item = {0, "", "", "10", "1"};
		sprintf(&item.boul, "%d%d", i, pwent);
		sprintf(&item.pri, "%s", pri);
		setOptionBouleItem(&item);
		addElement(list, item);
		i++;
	}
    return;
}

