/*
 * helpers.h
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */

#ifndef HELPERS_H_
#define HELPERS_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <convert.h>
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>
#include <wnet.h>
#include <posapi.h>
#include <ppp.h>
#include <led.h>
#include <ped.h>
#include <seos.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "constants.h"
#include "lkd.h"

#define FONT_ROBOTO "/home/user0/fonts/Mukta-Regular.ttf"
#define TOKEN_FILE "tk.jd"
#define USERNAME_FILE "user.jd"
#define TIRAGES_FILE "tirages.jd"


#define SERVER_FILE "/home/user0/mgnco/server.txt"

typedef struct
{
	char *response;
	size_t size;
} Memory;


typedef enum
{
	ALG_CENTER, ALG_LEFT, ALG_RIGHT
} LCD_ALG;

typedef struct
{
	char *name;
	char id[32];
} Tirage;

int check_connection(void);
int make_get_request(const char *url, long *status_code, char **buffer, const char *token);
int make_post_request(const char *url, const char *data, long *status_code, char **buffer, const char *token);
int make_http_request(const char *url, const char *data, long *status_code, char **buffer, const char *token, const char *verb);
int write_to_file(const char* file, const char* contents);
int read_from_file(const char* file, char** buffer);

int string_len(const char *text);
const char *string_covert( const char *text, int textlen);

void addItem(Tirage** array, int* size, char *id, const char* name);
int deleteInfoTirageByIndex(Tirage* list, int index, int size) ;
void freeItems(Tirage* array, int size);
int deleteInfoTirageByName(Tirage *array, int size, const char *name);
int hasInfoTirageByName(Tirage *array, int size, const char *name);

int get_current_y(void);
void lcdclean(void);
void lcd_draw_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, int isFIll);

void lcd_header(LCD_ALG alg, const char *pszFmt);
void lcdprintf(LCD_ALG alg, const char * pszFmt,...);
void lcdprintfex(LCD_ALG alg, unsigned y, const char * pszFmt,...);
int lcdprintfon(LCD_ALG alg, IDirectFBSurface *main_surface, int width, int height, int current_y, const char * pszFmt,...);

int lcdmenu(const char *pszTitle, const char menu[][25], unsigned int count, int select);

int lcdmenu_tirage(const char *pszTitle, const Tirage *menu, unsigned int count, int select, int *id, const Tirage *selectedTirage, int sizeSelected);
int drawpicture(int x, int y, int w, int h, const char *filename, IDirectFBSurface *target);


void Beep(unsigned int frequency, unsigned int keepms);
void bouleItemPrintf(const BouleItem *item, const char *tip, int icount);
int editableList(const BouleItem items[], unsigned int count, int select, const char *tip, int *s, Tirage *tirages, int sizeTirage);
int readServer(char ** data);

void removeCharAtIndex(char* str, const int index);
void setOptionBouleItem(BouleItem *item);


void setPrnY(int _y);
int getPrnY();
void printerprintf(LCD_ALG alg, IDirectFBSurface *surface, const char * pszFmt,...);
#endif /* HELPERS_H_ */
