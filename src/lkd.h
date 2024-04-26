/*
 * lkd.h
 *
 *  Created on: Apr 23, 2024
 *      Author: James DESTINE
 */

#ifndef LKD_H_
#define LKD_H_
#include <stdio.h>
#include <stdlib.h>



typedef struct
{
	int id;
	char boul[6];
	char pri[7];
	char lotto[3];
	char option[2];
} BouleItem;



typedef struct {
    BouleItem* items;
    int capacity;
    int size;
} List;

List* createList(void);
void destroyList(List* list);
void addElement(List* list, BouleItem element);
void deleteByIndex(List* list, int index);
int getElement(List *list, BouleItem item);


void addBoulePaire(List *list, char *pri);
void addBouleTriple(List *list, char *pri);
void addBouleRevers(List *list, char *pri);
void addBouleMarriage(List *list, char *pri);
void addBouleL4(List *list, char *pri);
void addPwent(List *list, char *pri, unsigned int pwent);
#endif /* LKD_H_ */
