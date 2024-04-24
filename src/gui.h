/*
 * gui.h
 *
 *  Created on: Apr 24, 2024
 *      Author: James DESTINE
 */

#ifndef GUI_H_
#define GUI_H_


#include <string.h>
#include <directfb.h>

#include "helpers.h"


typedef enum 
{
	NUM_ONLY, ALPHA
} EntryText;


typedef enum 
{
	NORMAL, HIDDEN
} EntryType;



typedef struct 
{
    int x;
    int y;
    int width;
    int height;
    char text[256];
    int cursor_pos;
    int active;
    int max_length;
    EntryText type;
    EntryType etype;
} Entry;

typedef struct 
{
    int x;
    int y;
    int width;
    int height;
    char text[11];
    int cursor_pos;
    int active;
} DDate;

void handleEntryInput(int keysym, Entry *entry);
void handleDDateInput(int keysym, DDate *entry);
void renderDDate(IDirectFBSurface *surface, DDate *entry);
void renderEntry(IDirectFBSurface *surface, Entry* entry);


#endif /* GUI_H_ */
