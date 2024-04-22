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

#define FONT_ROBOTO "/home/user0/fonts/Roboto-Regular.ttf"
#define TOKEN_FILE "tk.jd"
#define USERNAME_FILE "user.jd"
#define TIRAGES_FILE "tirages.jd"

typedef enum
{
	ALG_CENTER, ALG_LEFT, ALG_RIGHT
} LCD_ALG;

int string_len(const char *text);
const char *string_covert( const char *text, int textlen);

int get_current_y(void);
void lcdclean(void);
void lcd_draw_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, int isFIll);

void lcd_header(LCD_ALG alg, const char *pszFmt);
void lcdprintf(LCD_ALG alg, const char * pszFmt,...);
void lcdprintfex(LCD_ALG alg, unsigned y, const char * pszFmt,...);

int lcdmenu(const char *pszTitle, const char menu[][25], unsigned int count, int select);

int drawpicture(int x, int y, int w, int h, const char *filename, IDirectFBSurface *target);

#endif /* HELPERS_H_ */
